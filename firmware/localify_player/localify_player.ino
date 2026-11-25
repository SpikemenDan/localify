/*
  Localify breadboard player
  ESP32 + microSD card + I2S amplifier + screen + 4 buttons.

  Plays the playlists that Localify writes to the card
  (Settings -> Your data -> Export to SD card):
    /music/...              the songs (MP3 or WAV)
    /playlists/<name>.m3u   one file per playlist
  Songs copied to /music by hand also show up under "All Songs".

  Parts, wiring and flashing: docs/BREADBOARD.md
  Pins and screen type: config.h

  Buttons
    Lists        UP / DOWN move (hold to scroll), OK opens or plays, BACK goes back
    Now Playing  OK play/pause, UP previous, DOWN next,
                 hold UP / DOWN for volume, BACK for the menu
    Anywhere     hold BACK to jump to Now Playing
*/

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Preferences.h>
#include <vector>
#include <map>
#include <algorithm>
#include "config.h"

#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"

#if DISPLAY_TYPE == DISPLAY_LCD_I2C
  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>
#elif DISPLAY_TYPE == DISPLAY_ST7735
  #include <Adafruit_GFX.h>
  #include <Adafruit_ST7735.h>
  typedef Adafruit_ST7735 TftChip;
#else
  #include <Adafruit_GFX.h>
  #include <Adafruit_ST7789.h>
  typedef Adafruit_ST7789 TftChip;
#endif

// ======================================================================
// Types (kept above every function so the Arduino IDE is happy)
// ======================================================================

static const char *ALL_FILE = "/.localify/all.m3u";
static const uint32_t NO_INFO = 0xFFFFFFFF;
static const int MAX_SONGS = 3000;
static const int MAX_ROWS = 16;
static const int MAX_VOL = 21;

// Latin-1 letters (U+00C0..U+00FF) without their accents, for screens that only show plain ASCII.
static const char LATIN1[] = "AAAAAAACEEEEIIIIDNOOOOOxOUUUUYPsaaaaaaaceeeeiiiidnooooo/ouuuuypy";
static_assert(sizeof(LATIN1) == 65, "LATIN1 table must have 64 letters");

struct Entry { uint32_t path; uint32_t info; };   // byte offsets of a song's lines in its .m3u
struct Playlist { String file; String name; std::vector<Entry> items; };
struct SongInfo { String path; String title; String artist; int dur = -1; };

enum MenuKind : uint8_t { M_NOW, M_SHUFFLE, M_ALL, M_LIST };
struct MenuItem { MenuKind kind; String label; String path; };

enum Screen : uint8_t { SCR_HOME, SCR_SONGS, SCR_NOW, SCR_MSG };
enum Ev : uint8_t { EV_PRESS, EV_SHORT, EV_LONG, EV_REPEAT };
enum Btn : uint8_t { B_UP, B_DOWN, B_OK, B_BACK };

struct Button {
  uint8_t pin;
  bool down;
  bool longFired;
  uint32_t at, rep, edge;
};

enum Style : uint8_t { S_TEXT, S_TITLE, S_ITEM, S_SEL, S_DIM, S_BAR };
struct Row { String text; Style style; float frac; };

// ---------- Screens: everything is drawn as rows of text ----------
class Display {
 public:
  int cols = 16, rows = 2;
  virtual ~Display() {}
  virtual void begin() = 0;
  // Redraws only the rows that changed.
  void show(const Row *r) {
    for (int i = 0; i < rows; i++) {
      String key = String((int)r[i].style) + '|' + r[i].text;
      if (r[i].style == S_BAR) key += String((int)(r[i].frac * 100));
      if (key != cache[i]) { cache[i] = key; drawRow(i, r[i]); }
    }
  }
  void invalidate() { for (int i = 0; i < MAX_ROWS; i++) cache[i] = "\x01"; }
 protected:
  virtual void drawRow(int i, const Row &r) = 0;
  String cache[MAX_ROWS];
};

#if DISPLAY_TYPE == DISPLAY_LCD_I2C
class LcdDisplay : public Display {
  LiquidCrystal_I2C lcd{LCD_ADDR, LCD_COLS, LCD_ROWS};
 public:
  void begin() override {
    Wire.begin(PIN_LCD_SDA, PIN_LCD_SCL);
    lcd.init();
    lcd.backlight();
    cols = LCD_COLS;
    rows = std::min(LCD_ROWS, MAX_ROWS);
  }
 protected:
  void drawRow(int i, const Row &r) override {
    String t;
    if (r.style == S_BAR) {
      int n = (int)(r.frac * cols + 0.5f);
      for (int k = 0; k < cols; k++) t += k < n ? (char)0xFF : '-';   // 0xFF is a full block
    } else if (r.style == S_SEL) t = String('>') + r.text;
    else if (r.style == S_ITEM) t = String(' ') + r.text;
    else t = r.text;
    while ((int)t.length() < cols) t += ' ';
    lcd.setCursor(0, i);
    lcd.print(t.substring(0, cols));
  }
};
#else
class TftDisplay : public Display {
  SPIClass spi{HSPI};
  TftChip tft{&spi, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST};
  int S = 2, lineH = 22, top = 0;
  static const uint16_t C_BLACK = 0x0000, C_WHITE = 0xFFFF, C_ACCENT = 0x1DCA, C_GREY = 0xA514, C_DARK = 0x2945;
 public:
  void begin() override {
    if (PIN_TFT_BL >= 0) { pinMode(PIN_TFT_BL, OUTPUT); digitalWrite(PIN_TFT_BL, HIGH); }
    spi.begin(PIN_TFT_SCK, -1, PIN_TFT_MOSI, PIN_TFT_CS);   // before init, so the library keeps these pins
#if DISPLAY_TYPE == DISPLAY_ST7735
    tft.initR(ST7735_TAB);
#else
    tft.init(TFT_W, TFT_H, TFT_SPI_MODE);
#endif
    tft.setSPISpeed(40000000);
    tft.setRotation(TFT_ROTATION);
    tft.setTextWrap(false);
    tft.fillScreen(C_BLACK);
    int w = tft.width(), h = tft.height();
    S = w >= 200 ? 2 : 1;
    lineH = 8 * S + (S == 2 ? 6 : 4);
    rows = std::min(MAX_ROWS, h / lineH);
    cols = (w - 6 * S) / (6 * S);
    top = (h - rows * lineH) / 2;
  }
 protected:
  void drawRow(int i, const Row &r) override {
    int y = top + i * lineH, w = tft.width();
    uint16_t bg = C_BLACK, fg = C_WHITE;
    if (r.style == S_SEL) { bg = C_ACCENT; fg = C_BLACK; }
    else if (r.style == S_TITLE) fg = C_ACCENT;
    else if (r.style == S_DIM) fg = C_GREY;
    tft.fillRect(0, y, w, lineH, bg);
    if (r.style == S_BAR) {
      int x = 3 * S, bw = w - 6 * S, bh = 3 * S + 1, by = y + (lineH - bh) / 2;
      tft.fillRoundRect(x, by, bw, bh, bh / 2, C_DARK);
      int fw = (int)(bw * r.frac);
      if (fw > bw) fw = bw;
      if (fw > 0) tft.fillRoundRect(x, by, std::max(fw, bh), bh, bh / 2, C_ACCENT);
      return;
    }
    tft.setTextSize(S);
    tft.setTextColor(fg);
    tft.setCursor(3 * S, y + (lineH - 8 * S) / 2);
    tft.print(r.text.substring(0, cols));
  }
};
#endif

// ======================================================================
// State
// ======================================================================

#if DISPLAY_TYPE == DISPLAY_LCD_I2C
LcdDisplay disp;
#else
TftDisplay disp;
#endif

Preferences prefs;
AudioOutputI2S *out = nullptr;
AudioGenerator *gen = nullptr;          // only the main loop creates or deletes these,
AudioFileSourceSD *src = nullptr;       // always while holding audioLock
AudioFileSourceID3 *id3 = nullptr;
SemaphoreHandle_t audioLock;
volatile bool trackEnded = false;
volatile bool paused = true;

Playlist browse, playing;
std::vector<MenuItem> home;
std::map<int, String> labels;           // titles of the songs on screen
std::vector<int> history;               // for "previous" while shuffling

Screen screen = SCR_MSG;
int cursor = 0, topRow = 0, homeCursor = 0;
uint32_t cursorAt = 0;

int playIdx = -1;
SongInfo cur;
uint32_t playedMs = 0, tickAt = 0, titleAt = 0;
int volume = START_VOLUME;
bool shuffle = false;
uint32_t volSaveAt = 0;

bool sdOk = false;
uint32_t lastSdTry = 0, lastRender = 0;
String msg1, msg2;
bool dirty = true;

Row rowsBuf[MAX_ROWS];
Button btns[4] = {
  {PIN_BTN_UP, false, false, 0, 0, 0},
  {PIN_BTN_DOWN, false, false, 0, 0, 0},
  {PIN_BTN_OK, false, false, 0, 0, 0},
  {PIN_BTN_BACK, false, false, 0, 0, 0},
};

File lineFile;
String lineFileName;

// ======================================================================
// Text helpers
// ======================================================================

String toAscii(const String &in) {
  String o;
  o.reserve(in.length());
  const uint8_t *p = (const uint8_t *)in.c_str();
  while (*p) {
    uint32_t c = *p++;
    if (c < 0x80) { if (c >= 0x20) o += (char)c; continue; }
    int extra = c >= 0xF0 ? 3 : c >= 0xE0 ? 2 : c >= 0xC0 ? 1 : 0;
    if (!extra) { o += '?'; continue; }
    c &= (0x3F >> extra);
    while (extra-- > 0 && (*p & 0xC0) == 0x80) c = (c << 6) | (*p++ & 0x3F);
    if (c >= 0xC0 && c <= 0xFF) o += LATIN1[c - 0xC0];
    else if (c == 0x2018 || c == 0x2019) o += '\'';
    else if (c == 0x201C || c == 0x201D) o += '"';
    else if (c == 0x2013 || c == 0x2014) o += '-';
    else if (c == 0x2026) o += "...";
    else if (c == 0xA0) o += ' ';
    else o += '?';
  }
  return o;
}

String lower(String s) { s.toLowerCase(); return s; }
bool ciLess(const String &a, const String &b) { return lower(a) < lower(b); }

String baseName(const String &path) {
  int slash = path.lastIndexOf('/');
  String n = slash >= 0 ? path.substring(slash + 1) : path;
  int dot = n.lastIndexOf('.');
  return dot > 0 ? n.substring(0, dot) : n;
}

bool isWav(const String &name) { return lower(name).endsWith(".wav"); }
bool isPlayable(const String &name) { String l = lower(name); return l.endsWith(".mp3") || l.endsWith(".wav"); }

String fmtTime(int sec) {
  if (sec < 0) return "-:--";
  char b[12];
  snprintf(b, sizeof b, "%d:%02d", sec / 60, sec % 60);
  return String(b);
}

// Left text, right text, padded to the screen width.
String spread(const String &a, const String &b, int w) {
  int room = w - (int)b.length() - 1;
  String l = (int)a.length() > room ? a.substring(0, std::max(room, 0)) : a;
  while ((int)(l.length() + b.length()) < w) l += ' ';
  return l + b;
}

// Scrolls text that doesn't fit: waits, slides to the end, waits, starts again.
String marquee(const String &s, int w, uint32_t since) {
  int extra = (int)s.length() - w;
  if (extra <= 0) return s;
  int k = ((millis() - since) / 350) % (extra + 8);
  int off = k < 4 ? 0 : (k < 4 + extra ? k - 4 : extra);
  return s.substring(off, off + w);
}

// ======================================================================
// Playlists on the card
// ======================================================================

String readLineAt(const String &file, uint32_t off) {
  if (!lineFile || lineFileName != file) {
    if (lineFile) lineFile.close();
    lineFile = SD.open(file);
    lineFileName = file;
    if (!lineFile) return "";
  }
  lineFile.seek(off);
  String s = lineFile.readStringUntil('\n');
  if (s.startsWith("\xEF\xBB\xBF")) s.remove(0, 3);
  s.trim();
  return s;
}

void forgetLineFile() {
  if (lineFile) lineFile.close();
  lineFileName = "";
}

String normPath(String p) {
  p.replace('\\', '/');
  if (p.startsWith("/")) return p;
  if (p.startsWith("../")) return p.substring(2);
  return String("/playlists/") + p;
}

bool loadPlaylist(const String &path, Playlist &pl) {
  File f = SD.open(path);
  if (!f) return false;
  pl.file = path;
  pl.name = toAscii(baseName(path));
  pl.items.clear();
  uint32_t info = NO_INFO;
  while (f.available() && (int)pl.items.size() < MAX_SONGS) {
    uint32_t pos = f.position();
    String line = f.readStringUntil('\n');
    if (line.startsWith("\xEF\xBB\xBF")) line.remove(0, 3);
    line.trim();
    if (!line.length()) continue;
    if (line.startsWith("#EXTINF:")) info = pos;
    else if (line.startsWith("#PLAYLIST:")) pl.name = toAscii(line.substring(10));
    else if (line[0] == '#') continue;
    else { pl.items.push_back({pos, info}); info = NO_INFO; }
  }
  f.close();
  return true;
}

String playlistTitle(const String &path) {
  File f = SD.open(path);
  if (!f) return "";
  String t;
  for (int i = 0; i < 4 && f.available(); i++) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.startsWith("#PLAYLIST:")) { t = toAscii(line.substring(10)); break; }
  }
  f.close();
  t.trim();
  return t;
}

SongInfo getInfo(const Playlist &pl, int i) {
  SongInfo s;
  if (i < 0 || i >= (int)pl.items.size()) return s;
  const Entry &e = pl.items[i];
  s.path = normPath(readLineAt(pl.file, e.path));
  String label;
  if (e.info != NO_INFO) {
    String line = readLineAt(pl.file, e.info);   // #EXTINF:215,Artist - Title
    int comma = line.indexOf(',');
    if (comma > 8) { s.dur = line.substring(8, comma).toInt(); label = line.substring(comma + 1); }
  }
  if (!label.length()) label = baseName(s.path);
  label = toAscii(label);
  label.trim();
  int dash = label.indexOf(" - ");
  if (dash > 0) { s.artist = label.substring(0, dash); s.title = label.substring(dash + 3); }
  else s.title = label;
  if (!s.title.length()) s.title = "(no name)";
  return s;
}

// Writes every song under /music into one playlist, folder by folder, A to Z.
void scanDir(const String &dir, File &outFile, int depth, int &count) {
  File d = SD.open(dir);
  if (!d || !d.isDirectory()) return;
  std::vector<String> files, dirs;
  File f;
  while ((f = d.openNextFile())) {
    String n = f.name();
    int slash = n.lastIndexOf('/');
    if (slash >= 0) n = n.substring(slash + 1);
    if (!n.startsWith(".")) {
      if (f.isDirectory()) dirs.push_back(n);
      else if (isPlayable(n)) files.push_back(n);
    }
    f.close();
  }
  d.close();
  std::sort(files.begin(), files.end(), ciLess);
  std::sort(dirs.begin(), dirs.end(), ciLess);
  for (const String &n : files) {
    if (count >= MAX_SONGS) return;
    outFile.print(dir + "/" + n + "\n");
    count++;
  }
  if (depth < 6)
    for (const String &n : dirs) scanDir(dir + "/" + n, outFile, depth + 1, count);
}

int scanMusic() {
  forgetLineFile();
  SD.mkdir("/.localify");
  File o = SD.open(ALL_FILE, FILE_WRITE);
  if (!o) return 0;
  o.print("#EXTM3U\n#PLAYLIST:All Songs\n");
  int count = 0;
  scanDir("/music", o, 0, count);
  o.close();
  return count;
}

void buildHome() {
  home.clear();
  home.push_back({M_NOW, "Now Playing", ""});
  home.push_back({M_SHUFFLE, "", ""});
  home.push_back({M_ALL, "All Songs", ALL_FILE});
  std::vector<MenuItem> lists;
  File d = SD.open("/playlists");
  if (d && d.isDirectory()) {
    File f;
    while ((f = d.openNextFile()) && lists.size() < 64) {
      String n = f.name();
      int slash = n.lastIndexOf('/');
      if (slash >= 0) n = n.substring(slash + 1);
      if (!f.isDirectory() && !n.startsWith(".") && lower(n).endsWith(".m3u")) {
        String path = String("/playlists/") + n;
        String t = playlistTitle(path);
        lists.push_back({M_LIST, t.length() ? t : toAscii(baseName(n)), path});
      }
      f.close();
    }
    d.close();
  }
  std::sort(lists.begin(), lists.end(), [](const MenuItem &a, const MenuItem &b) {
    bool la = a.label == "Liked Songs", lb = b.label == "Liked Songs";
    if (la != lb) return la;
    return ciLess(a.label, b.label);
  });
  for (const MenuItem &m : lists) home.push_back(m);
}

// ======================================================================
// Audio
// ======================================================================

// Runs on core 0 and keeps the decoder fed.
void audioTask(void *) {
  for (;;) {
    xSemaphoreTake(audioLock, portMAX_DELAY);
    if (gen && !paused && gen->isRunning()) {
      if (!gen->loop()) { gen->stop(); trackEnded = true; }
    }
    xSemaphoreGive(audioLock);
    vTaskDelay(1);
  }
}

void stopAudioLocked() {
  if (gen) { if (gen->isRunning()) gen->stop(); delete gen; gen = nullptr; }
  if (id3) { delete id3; id3 = nullptr; }
  if (src) { delete src; src = nullptr; }
}

bool startTrack(int i) {
  int n = playing.items.size();
  if (!n) return false;
  i = ((i % n) + n) % n;
  SongInfo s = getInfo(playing, i);
  bool ok = false;
  xSemaphoreTake(audioLock, portMAX_DELAY);
  stopAudioLocked();
  if (s.path.length() && isPlayable(s.path) && SD.exists(s.path)) {
    src = new AudioFileSourceSD(s.path.c_str());
    if (src->isOpen()) {
      if (isWav(s.path)) { gen = new AudioGeneratorWAV(); ok = gen->begin(src, out); }
      else { id3 = new AudioFileSourceID3(src); gen = new AudioGeneratorMP3(); ok = gen->begin(id3, out); }
    }
  }
  if (!ok) stopAudioLocked();
  trackEnded = false;
  paused = !ok;
  xSemaphoreGive(audioLock);

  playIdx = i;
  cur = s;
  playedMs = 0;
  tickAt = titleAt = millis();
  dirty = true;
  if (ok) {
    prefs.putString("list", playing.file);
    prefs.putInt("idx", i);
  }
  return ok;
}

void showMessage(const String &a, const String &b) {
  msg1 = a;
  msg2 = b;
  screen = SCR_MSG;
  dirty = true;
}

// Plays song i, or the next one that works.
void playWithSkip(int i, int dir) {
  int tries = std::min((int)playing.items.size(), 10);
  for (int t = 0; t < tries; t++, i += dir)
    if (startTrack(i)) return;
  showMessage("Can't play these", "MP3 or WAV only");
}

void pushHistory(int i) {
  history.push_back(i);
  if (history.size() > 64) history.erase(history.begin());
}

void nextTrack() {
  int n = playing.items.size();
  if (playIdx < 0 || !n) return;
  int j = (playIdx + 1) % n;
  if (shuffle && n > 1) { do j = random(n); while (j == playIdx); }
  pushHistory(playIdx);
  playWithSkip(j, 1);
}

void prevTrack() {
  int n = playing.items.size();
  if (playIdx < 0 || !n) return;
  if (playedMs > 3000) { startTrack(playIdx); return; }   // back to the start of this song
  int j = (playIdx - 1 + n) % n;
  if (shuffle && !history.empty()) { j = history.back(); history.pop_back(); }
  playWithSkip(j, -1);
}

void togglePlay() {
  if (playIdx < 0) return;
  if (!gen) { playWithSkip(playIdx, 1); return; }
  xSemaphoreTake(audioLock, portMAX_DELAY);
  paused = !paused;
  if (paused) out->flush();
  xSemaphoreGive(audioLock);
  tickAt = millis();
}

void setVolume(int v) {
  volume = std::max(0, std::min(MAX_VOL, v));
  xSemaphoreTake(audioLock, portMAX_DELAY);
  out->SetGain(MAX_GAIN * volume / (float)MAX_VOL);
  xSemaphoreGive(audioLock);
  volSaveAt = millis() + 1500;
}

// ======================================================================
// Screens
// ======================================================================

String homeLabel(int i) {
  const MenuItem &m = home[i];
  if (m.kind == M_SHUFFLE) return shuffle ? "Shuffle: On" : "Shuffle: Off";
  return m.label;
}

String songLabel(int i) {
  auto it = labels.find(i);
  if (it != labels.end()) return it->second;
  if (labels.size() > 48) labels.clear();
  String t = getInfo(browse, i).title;
  labels[i] = t;
  return t;
}

void clearRows() {
  for (Row &r : rowsBuf) { r.text = ""; r.style = S_TEXT; r.frac = 0; }
}

void drawList(const String &title, int count, String (*label)(int)) {
  int header = disp.rows >= 3 ? 1 : 0;
  int vis = disp.rows - header;
  if (cursor >= count) cursor = std::max(0, count - 1);
  if (cursor < topRow) topRow = cursor;
  if (cursor >= topRow + vis) topRow = cursor - vis + 1;
  if (header) rowsBuf[0] = {spread(title, count ? String(String(cursor + 1) + "/" + String(count)) : String(""), disp.cols), S_TITLE, 0};
  if (!count) {
    rowsBuf[header] = {"No songs here.", S_TEXT, 0};
    if (header + 1 < disp.rows) rowsBuf[header + 1] = {"Export again", S_DIM, 0};
    return;
  }
  for (int r = 0; r < vis; r++) {
    int i = topRow + r;
    if (i >= count) break;
    String t = label(i);
    bool sel = i == cursor;
    rowsBuf[header + r] = {sel ? marquee(t, disp.cols - 1, cursorAt) : t, sel ? S_SEL : S_ITEM, 0};
  }
}

void drawNow() {
  int R = disp.rows, C = disp.cols;
  if (playIdx < 0) {
    rowsBuf[0] = {"Nothing playing", S_TITLE, 0};
    rowsBuf[1] = {"BACK: pick songs", S_DIM, 0};
    return;
  }
  float frac = 0;
  xSemaphoreTake(audioLock, portMAX_DELAY);
  if (src && src->getSize()) frac = (float)src->getPos() / src->getSize();
  xSemaphoreGive(audioLock);
  int el = playedMs / 1000;
  int tot = cur.dur > 0 ? cur.dur : (frac > 0.03f ? (int)(el / frac) : -1);
  if (tot > 0 && el > tot) el = tot;
  String icon = !gen ? "-" : paused ? "||" : ">";
  String state = !gen ? "Stopped" : paused ? "Paused" : "Playing";

  if (R >= 9) {
    rowsBuf[0] = {playing.name, S_TITLE, 0};
    String l1 = cur.title, l2 = "";
    if ((int)l1.length() > C) {
      int cut = cur.title.lastIndexOf(' ', C);
      if (cut <= 0) cut = C;
      l1 = cur.title.substring(0, cut);
      l2 = cur.title.substring(cut);
      l2.trim();
    }
    rowsBuf[2] = {l1, S_TEXT, 0};
    rowsBuf[3] = {marquee(l2, C, titleAt), S_TEXT, 0};
    rowsBuf[4] = {marquee(cur.artist, C, titleAt), S_DIM, 0};
    rowsBuf[6] = {"", S_BAR, frac};
    rowsBuf[7] = {spread(fmtTime(el), fmtTime(tot), C), S_DIM, 0};
    rowsBuf[8] = {spread(state, String(shuffle ? "Shuf " : "") + "Vol " + String(volume), C), S_TEXT, 0};
  } else if (R >= 4) {
    rowsBuf[0] = {marquee(cur.title, C, titleAt), S_TEXT, 0};
    rowsBuf[1] = {marquee(cur.artist, C, titleAt), S_DIM, 0};
    rowsBuf[2] = {"", S_BAR, frac};
    rowsBuf[3] = {spread(icon + " " + fmtTime(el) + "/" + fmtTime(tot), String(shuffle ? "S " : "") + "V" + String(volume), C), S_TEXT, 0};
  } else {
    rowsBuf[0] = {marquee(cur.title, C, titleAt), S_TEXT, 0};
    if (R > 1) rowsBuf[1] = {spread(icon + " " + fmtTime(el) + "/" + fmtTime(tot), "V" + String(volume), C), S_TEXT, 0};
  }
}

void render() {
  clearRows();
  switch (screen) {
    case SCR_HOME: drawList("Localify", home.size(), homeLabel); break;
    case SCR_SONGS: drawList(browse.name, browse.items.size(), songLabel); break;
    case SCR_NOW: drawNow(); break;
    case SCR_MSG: {
      int r = disp.rows >= 3 ? disp.rows / 2 - 1 : 0;
      rowsBuf[r] = {msg1, S_TITLE, 0};
      if (r + 1 < disp.rows) rowsBuf[r + 1] = {msg2, S_DIM, 0};
      break;
    }
  }
  disp.show(rowsBuf);
}

void moveCursor(int to) {
  cursor = to;
  cursorAt = millis();
}

void goHome() {
  screen = SCR_HOME;
  topRow = 0;
  moveCursor(homeCursor);
}

void openList(const String &path) {
  showMessage("Opening...", "");
  render();
  homeCursor = cursor;
  if (!loadPlaylist(path, browse)) { showMessage("Can't open list", "Export again"); return; }
  labels.clear();
  screen = SCR_SONGS;
  topRow = 0;
  moveCursor(playing.file == path && playIdx >= 0 && playIdx < (int)browse.items.size() ? playIdx : 0);
}

void selectItem() {
  if (screen == SCR_HOME) {
    if (cursor >= (int)home.size()) return;
    const MenuItem m = home[cursor];
    if (m.kind == M_NOW) { homeCursor = cursor; screen = SCR_NOW; }
    else if (m.kind == M_SHUFFLE) { shuffle = !shuffle; prefs.putBool("shuf", shuffle); history.clear(); }
    else openList(m.path);
  } else if (screen == SCR_SONGS && !browse.items.empty()) {
    playing = browse;
    history.clear();
    playWithSkip(cursor, 1);
    if (screen != SCR_MSG) screen = SCR_NOW;
  }
}

void onButton(int b, Ev ev) {
  dirty = true;
  if (b == B_BACK && ev == EV_LONG && screen != SCR_MSG) { if (screen == SCR_HOME) homeCursor = cursor; screen = SCR_NOW; return; }
  switch (screen) {
    case SCR_MSG:
      if (ev == EV_SHORT && sdOk) goHome();
      break;
    case SCR_HOME:
    case SCR_SONGS: {
      int count = screen == SCR_HOME ? home.size() : browse.items.size();
      if (!count) { if (b == B_BACK && ev == EV_SHORT) goHome(); break; }
      if (b == B_UP || b == B_DOWN) {
        int step = b == B_UP ? -1 : 1;
        if (ev == EV_PRESS) moveCursor((cursor + step + count) % count);            // wraps around
        else if (ev == EV_REPEAT) moveCursor(std::max(0, std::min(count - 1, cursor + step)));   // stops at the ends
      } else if (b == B_OK && ev == EV_SHORT) selectItem();
      else if (b == B_BACK && ev == EV_SHORT) {
        if (screen == SCR_SONGS) goHome();
        else if (playIdx >= 0) { homeCursor = cursor; screen = SCR_NOW; }
      }
      break;
    }
    case SCR_NOW:
      if (b == B_OK && ev == EV_SHORT) togglePlay();
      else if (b == B_UP && ev == EV_SHORT) prevTrack();
      else if (b == B_DOWN && ev == EV_SHORT) nextTrack();
      else if ((b == B_UP || b == B_DOWN) && (ev == EV_LONG || ev == EV_REPEAT)) setVolume(volume + (b == B_UP ? 1 : -1));
      else if (b == B_BACK && ev == EV_SHORT) goHome();
      break;
  }
}

void pollButtons() {
  uint32_t now = millis();
  uint32_t every = screen == SCR_NOW ? 200 : 90;
  for (int b = 0; b < 4; b++) {
    Button &k = btns[b];
    bool raw = digitalRead(k.pin) == LOW;
    if (raw != k.down) {
      if (now - k.edge < 25) continue;   // ignore contact bounce
      k.edge = now;
      k.down = raw;
      if (raw) { k.at = k.rep = now; k.longFired = false; onButton(b, EV_PRESS); }
      else if (!k.longFired) onButton(b, EV_SHORT);
    } else if (k.down) {
      if (!k.longFired && now - k.at >= 500) { k.longFired = true; k.rep = now; onButton(b, EV_LONG); }
      else if (k.longFired && now - k.rep >= every) { k.rep = now; onButton(b, EV_REPEAT); }
    }
  }
}

// ======================================================================
// Start-up
// ======================================================================

bool trySd() {
  SD.end();
  if (!SD.begin(PIN_SD_CS, SPI, SD_SPI_HZ)) {
    sdOk = false;
    showMessage("No SD card", "Insert one...");
    return false;
  }
  sdOk = true;
  showMessage("Reading card", "One moment...");
  render();
  int songs = scanMusic();
  buildHome();

  // Pick up where we left off (paused).
  String last = prefs.getString("list", "");
  int idx = prefs.getInt("idx", 0);
  if (last.length() && SD.exists(last) && loadPlaylist(last, playing) && idx >= 0 && idx < (int)playing.items.size()) {
    playIdx = idx;
    cur = getInfo(playing, idx);
    titleAt = millis();
  }
  homeCursor = playIdx >= 0 ? 0 : 2;   // "All Songs" when there's nothing to resume
  if (!songs && home.size() <= 3) showMessage("No songs found", "Export from app");
  else if (playIdx >= 0) screen = SCR_NOW;
  else goHome();
  return true;
}

void setup() {
  Serial.begin(115200);
  for (Button &k : btns) pinMode(k.pin, INPUT_PULLUP);
  disp.begin();
  disp.invalidate();
  showMessage("Localify", "Starting...");
  render();

  prefs.begin("localify", false);
  volume = std::max(0, std::min(MAX_VOL, (int)prefs.getUChar("vol", START_VOLUME)));
  shuffle = prefs.getBool("shuf", false);
  randomSeed(esp_random());

  audioLock = xSemaphoreCreateMutex();
  out = new AudioOutputI2S(0, 0 /* external I2S DAC */, 16);
  out->SetPinout(PIN_I2S_BCLK, PIN_I2S_LRC, PIN_I2S_DIN);
  out->SetGain(MAX_GAIN * volume / (float)MAX_VOL);
  xTaskCreatePinnedToCore(audioTask, "audio", 10240, nullptr, 3, nullptr, 0);

  SPI.begin(PIN_SD_SCK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
  trySd();
}

void loop() {
  uint32_t now = millis();
  if (!sdOk) {
    if (now - lastSdTry > 2000) { lastSdTry = now; trySd(); }
    render();
    delay(50);
    return;
  }
  pollButtons();
  if (trackEnded) { trackEnded = false; nextTrack(); }   // repeats the list at the end
  if (gen && !paused) playedMs += now - tickAt;
  tickAt = now;
  if (volSaveAt && now > volSaveAt) { prefs.putUChar("vol", volume); volSaveAt = 0; }
  if (dirty || now - lastRender > 100) { render(); lastRender = now; dirty = false; }
  delay(5);
}

# Localify on a breadboard (ESP32 MP3 player)

Build a small music player that plays your Localify playlists without a computer. It uses an ESP32, a microSD card, a little amplifier with a speaker, a screen and four buttons.

1. In Localify, copy your playlists (and their songs) onto a microSD card.
2. Put the card in the player.
3. Pick a playlist with the buttons and press play.

The player runs on its own. It doesn't need Wi-Fi, and nothing syncs back to Localify.

---

## What you need

| Part | Notes | About |
| --- | --- | --- |
| **ESP32 dev board** | "ESP32 DevKit V1" or "ESP32-WROOM-32 DevKitC", 30 or 38 pins. A **WROVER** board also works, but move the DOWN and OK buttons off GPIO 16 and 17 (see [Changing pins](#changing-pins)). | $5–10 |
| **MAX98357A I2S amplifier** | Small board, about 3 W. Plays straight from the ESP32 with no extra parts. | $3–6 |
| **Speaker** | 4 Ω or 8 Ω, 2–3 W. | $2–5 |
| **microSD card module** | The SPI kind with 6 pins (GND, VCC, MISO, MOSI, SCK, CS). | $1–3 |
| **microSD card** | Up to 32 GB, formatted **FAT32**. | |
| **A screen** (pick one) | **ST7789** 240×240 or 240×320 colour screen (recommended), **ST7735** 1.8" 128×160 colour screen, or a **16×2 / 20×4 character LCD with an I2C backpack**. | $3–8 |
| **4 push buttons** | The small 4-leg tactile kind. | |
| **Breadboard and jumper wires** | A full-size breadboard (830 holes) fits everything. | |
| **USB cable** | To power and program the ESP32. | |

> [!TIP]
> Already have a different screen or amp? Tell Claude (or open [`config.h`](../firmware/localify_player/config.h)) and change the type and pins there.

---

## Wiring

Every part needs power: connect each **GND** to an ESP32 **GND** and each **VCC/VIN** as shown. Use the breadboard's long side rails for 3.3V, 5V and GND.

### microSD card module

| Module pin | ESP32 pin |
| --- | --- |
| GND | GND |
| VCC | **5V** (VIN) if the module has a regulator (most do), otherwise 3.3V |
| MISO | GPIO 19 |
| MOSI | GPIO 23 |
| SCK | GPIO 18 |
| CS | GPIO 5 |

### MAX98357A amplifier

| Amp pin | ESP32 pin |
| --- | --- |
| VIN | 5V (VIN) |
| GND | GND |
| BCLK | GPIO 26 |
| LRC | GPIO 25 |
| DIN | GPIO 32 |
| GAIN, SD | leave unconnected |

Connect the speaker to the amp's **+** and **−** screw terminals.

### Screen: ST7789 or ST7735 colour screen

| Screen pin | ESP32 pin |
| --- | --- |
| GND | GND |
| VCC | 3.3V |
| SCL (or SCK, CLK) | GPIO 14 |
| SDA (or MOSI, DIN) | GPIO 13 |
| CS | GPIO 15 (some 240×240 screens have no CS pin; see below) |
| DC (or RS, A0) | GPIO 27 |
| RES (or RST) | 3.3V |
| BLK (or BL, LED) | 3.3V |

A **240×240 screen with no CS pin**: in `config.h`, set `TFT_SPI_MODE` to `SPI_MODE3` and `PIN_TFT_CS` to `-1`.

### Screen: 16×2 or 20×4 character LCD with I2C backpack

| LCD backpack pin | ESP32 pin |
| --- | --- |
| GND | GND |
| VCC | 5V (VIN) |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

In `config.h`, set `DISPLAY_TYPE` to `DISPLAY_LCD_I2C`, and `LCD_COLS` / `LCD_ROWS` to `20` / `4` for the bigger one. If the screen lights up but shows only blocks, turn the small blue screw on the backpack (contrast). If it stays blank, try `LCD_ADDR 0x3F`.

### Buttons

Each button goes across the breadboard's middle gap. Connect one leg to the ESP32 pin and the leg diagonally opposite to **GND**. No resistors are needed.

| Button | ESP32 pin |
| --- | --- |
| UP | GPIO 4 |
| DOWN | GPIO 16 |
| OK | GPIO 17 |
| BACK | GPIO 33 |

### All pins at a glance

```
            ESP32 DevKit
   3.3V  ── screen VCC, RES, BLK
   5V    ── SD VCC, amp VIN, (LCD VCC)
   GND   ── every GND, and the other leg of every button

   GPIO 18 ─ SD SCK       GPIO 14 ─ screen SCL
   GPIO 19 ─ SD MISO      GPIO 13 ─ screen SDA
   GPIO 23 ─ SD MOSI      GPIO 15 ─ screen CS
   GPIO  5 ─ SD CS        GPIO 27 ─ screen DC

   GPIO 26 ─ amp BCLK     GPIO 21 ─ LCD SDA  (character LCD only)
   GPIO 25 ─ amp LRC      GPIO 22 ─ LCD SCL  (character LCD only)
   GPIO 32 ─ amp DIN

   GPIO  4 ─ UP   GPIO 16 ─ DOWN   GPIO 17 ─ OK   GPIO 33 ─ BACK
```

> [!WARNING]
> Don't connect anything to **GPIO 12**, and keep GPIO 0, 2, 5 and 15 free of buttons. The ESP32 reads these pins at power-on, and the wrong level stops it from starting.

---

## Put the software on the ESP32

Use **one** of these.

### Arduino IDE (easiest)

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. **Tools → Board → Boards Manager**: search **esp32** and install **esp32 by Espressif Systems**, version **2.0.17** (a 2.x version; 3.x won't build this yet).
3. **Tools → Manage Libraries**: install
   - **ESP8266Audio** by Earle Philhower, version **1.9.7** (it works on the ESP32 too)
   - for a colour screen: **Adafruit ST7735 and ST7789 Library** (say yes to installing **Adafruit GFX** with it)
   - for a character LCD: **LiquidCrystal I2C** by Frank de Brabander
4. Open `firmware/localify_player/localify_player.ino`. Change `config.h` (the second tab) if your screen or pins differ.
5. **Tools → Board**: **ESP32 Dev Module**. **Tools → Partition Scheme**: **Huge APP (3MB No OTA)**.
6. Plug in the ESP32, pick its port under **Tools → Port**, and press **Upload**. If it waits at "Connecting…", hold the board's **BOOT** button until the upload starts.

### PlatformIO

```
cd firmware
pio run -t upload
pio device monitor
```

---

## Fill the SD card from Localify

1. Put the microSD card in your computer.
2. Open Localify in **Chrome or Edge** and load the folder with your music.
3. **Settings → Your Data → Export to SD card → Export…**
4. Tick the playlists you want. Leave **Copy the songs too** on.
5. Press **Choose SD card** and pick the card itself (its top level, not a folder on it). Allow Localify to edit it.
6. Wait for **Done**, then eject the card safely.

Localify writes:

```
SD card
├── music/        your songs, in the same folders as on your computer
└── playlists/    one .m3u file per playlist, Liked Songs too
```

Run the export again any time. Songs that are already on the card are skipped, so it's quick, and the playlists are rewritten.

- Only **MP3** and **WAV** play on the player. Songs in other formats (M4A, FLAC, OGG…) are left out of the export.
- Names with letters outside A–Z, such as é or 日本, are simplified on the card (é becomes e). The screen shows plain letters too.
- You can also copy MP3s into `music/` by hand. They show up under **All Songs**.

---

## Using the player

The home screen lists **Now Playing**, **Shuffle**, **All Songs**, then your playlists (Liked Songs first).

| Where | Button | Does |
| --- | --- | --- |
| Lists | UP / DOWN | Move (hold to scroll) |
| | OK | Open the playlist, or play the song |
| | BACK | Back |
| Now Playing | OK | Play / pause |
| | UP / DOWN | Previous / next song |
| | Hold UP / DOWN | Volume up / down |
| | BACK | Home screen |
| Anywhere | Hold BACK | Now Playing |

- When a playlist ends, it starts again from the top.
- **Previous** goes back to the start of the song if it's been playing for more than 3 seconds.
- The player remembers the last song, the volume and shuffle. When you turn it on again, it's paused on that song: press OK.

---

## Changing pins

All pins and the screen type are in [`firmware/localify_player/config.h`](../firmware/localify_player/config.h). Good spare pins on a WROOM board: 4, 13, 14, 16, 17, 21, 22, 25, 26, 27, 32 and 33. GPIO 34–39 work for buttons only with an external 10 kΩ resistor to 3.3V, because they have no pull-up of their own.

---

## Troubleshooting

<details>
<summary><b>The screen says "No SD card"</b></summary>

Check the SD module's wiring and that the card is FAT32 and 32 GB or smaller. Some modules need 5V on VCC. If it still isn't found, set `SD_SPI_HZ` to `4000000` in `config.h` and upload again.
</details>

<details>
<summary><b>"No songs found"</b></summary>

Export from Localify again, or check that the card has a `music` folder with MP3 or WAV files in it.
</details>

<details>
<summary><b>The colour screen stays white or black</b></summary>

Check SCL, SDA and DC. For a 240×240 screen without a CS pin, use `SPI_MODE3` and `PIN_TFT_CS -1`. For a 240×320 screen set `TFT_H` to `320`. If the picture is sideways, change `TFT_ROTATION`. On an ST7735 with odd colours or a stripe at the edge, try `INITR_GREENTAB`.
</details>

<details>
<summary><b>No sound, or it buzzes or crackles</b></summary>

Check BCLK, LRC and DIN, and that the amp has 5V. Crackling usually means the gain is too high: lower `MAX_GAIN`. If it's too quiet at full volume, raise it (up to `4.0`). A USB port that can't give enough power can also cause dropouts; try a phone charger.
</details>

<details>
<summary><b>A song is skipped</b></summary>

The player skips songs it can't open. Check that it's a real MP3 or WAV (not M4A renamed to .mp3). Very high bitrate files and 24-bit WAVs may not play.
</details>

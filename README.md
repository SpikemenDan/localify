<p align="center">
  <img src="icons/icon-192.png" width="96" height="96" alt="Localify icon">
</p>

<h1 align="center">Localify</h1>

<p align="center">
  <b>A music player for the songs already on your device.</b><br>
  No account, no uploads, no streaming. Works offline.
</p>

---

## Contents

- [Getting started](#getting-started)
- [What it can do](#what-it-can-do)
- [Tidy Names & Tags (AI)](#tidy-names--tags-ai)
- [Lock-in focus timer](#lock-in-focus-timer)
- [Profiles and backups](#profiles-and-backups)
- [Install it as an app](#install-it-as-an-app)
- [Breadboard player (ESP32)](#breadboard-player-esp32)
- [Keyboard shortcuts](#keyboard-shortcuts)
- [Privacy](#privacy)
- [Troubleshooting](#troubleshooting)

---

## Getting started

1. Open **`localify_offline_mp3_player.html`** in Chrome or Edge. Double-clicking it works.
2. Click **Load folder** and pick the folder that has your music. Localify asks you to give it a name, an icon and a color (or just skip).
3. Press **Play**, or **Shuffle play**.

That's all. Localify remembers your likes, settings, folders and the last song you played the next time you open it. Your folders stay in the sidebar under **Your folders**. Hover one and press the pencil to rename or restyle it. Your playlists are remembered the same way, and fill back in when you load the folder with their songs. After a restart, the browser may ask you to allow the folder again; in the installed app, choose **Allow on every visit** so it stops asking.

> [!TIP]
> Chrome and Edge work best. Other browsers can play music, but they can't save changes back into your files.

---

## What it can do

| | |
| --- | --- |
| 🎵 **Library** | All Tracks, **Albums** and **Artists** views, plus search. |
| 📃 **Playlists** | Liked Songs plus your own playlists. Press **+** next to *Playlists* in the sidebar, or use **⋯ → Add to playlist** on any song. Give each one a name, an icon (any emoji, or your own picture) and a color; hover it and press the pencil to rename or delete it. |
| 🎨 **Looks** | Dark, Light or System mode, your choice of accent color, and compact rows. |
| 🎤 **Lyrics** | Press **Y** or tap the lyrics icon. Synced lyrics highlight as the song plays; tap a line to jump there. Uses a matching `.lrc` file or the song's own tags first, then looks them up on [LRCLIB](https://lrclib.net). Lyrics you've opened are saved for offline use. |
| 🌈 **Visualizer** | A full-screen visualizer that moves with the music. |
| 🎚️ **Sound** | Equalizer presets (Bass boost, Vocal, Treble boost, Loudness) or your own Bass/Mid/Treble, volume leveling and crossfade. |
| ⏩ **Speed & pitch** | Playback speed from 0.25x to 3x, plus a **Reverb** slider. Turn *Change pitch with speed* off to keep the original key. |
| 😴 **Sleep timer** | Fades out after 15, 30, 45 or 60 minutes, or at the end of the song. |
| ✂️ **Audio editor** | Trim, cut, silence, reverse, fade in/out, gain, normalize, speed and EQ. Press **E** on a song. |
| 🔊 **Audacity** | On Windows, send a song straight to Audacity (set up under **Settings → Integrations**). |
| 📟 **SD card export** | Copy playlists and their songs to a microSD card for the [ESP32 breadboard player](#breadboard-player-esp32). **Settings → Your Data → Export to SD card**. |

Find everything else in **Settings** (the gear icon, or press **,**).

---

## Tidy Names & Tags (AI)

Messy file names like `shape_of_you_FINAL (1).mp3` get cleaned up automatically.

**How it works**

- **New songs are filled in for you.** Title, artist, album, year, genre and album cover are looked up when you add them.
- **Offline? No problem.** Songs added without internet go to the **Unsorted** list and are filled in once you're back online.
- **Songs with useless names are recognised by their sound.** If the name and tags don't say what a song is, Localify can identify it from a fingerprint of its sound (needs a free AcoustID key, see below).
- **It asks when it isn't sure.** When the chip at the top says *questions*, click it and answer a few quick ones together.
- **Want to check a song?** Hover it: a small dot on its cover means it's still unsorted (grey) or has a question (green, click to answer). Its **⋯** menu says the same, and **See what Localify filled in** shows what changed.
- **Your files are safe.** Nothing in the files changes until you press **Apply** in Tidy, and you can undo from there.

To tidy one song by hand, select it and press **I**, or use the **⋯** menu on its row. To identify a song by its sound, choose **Identify by sound** from the **⋯** menu, or press **By Sound** on its row in Tidy.

**Settings → Tidy Names & Tags** lets you:

- choose the file name pattern (for example `{artist} - {title}`)
- turn online lookups or AI suggestions on or off
- let Localify write details straight into your files automatically
- add an **AcoustID key** so songs can be identified by their sound. Get one free at [acoustid.org/new-application](https://acoustid.org/new-application) and paste it in. The key is saved in your profile (and its backups).

> [!NOTE]
> The AI model is about **500 MB** and downloads **once**, the first time it's needed. It runs on your own computer. Remove it any time with **Remove model**.

---

## Lock-in focus timer

Press **T** (or tap the timer icon) to start a focus session. Your music keeps playing; when time's up, it dips and an alarm rings.

- **Alarm sounds:** Soft chime, Bell, Marimba, Digital beep, Gentle rise, Gong, or **your own sound file**.
- Choose how loud the alarm is and how quiet the music gets while it rings.
- Optional desktop notification, and vibration on phones.

---

## Profiles and backups

Share a computer? Each person can have their own **profile** with their own likes, settings and history. Click your avatar in the top-right corner to switch or add one.

To move your data to another device, open **Settings → Your Data**:

- **Export backup** saves a small `.localify.json` file. Songs are never included.
- **Import backup** previews the file first. You can then add it as a new profile, merge it, or replace the current one.
- **Export to SD card** copies your playlists and their MP3/WAV songs to a microSD card for the [breadboard player](#breadboard-player-esp32).
- **Keep data on this device** shows whether the browser has promised not to clear Localify's data when space runs low. Localify asks for this by itself; press **Keep it** to ask again.

Everything is stored in your browser on this device, never on a server. Clearing the site's data or using a private window removes it, so keep a backup.

---

## Install it as an app

If Localify is published at a web address, you can install it so it opens like a normal app, even offline.

| Device | How |
| --- | --- |
| **Windows / Mac** (Chrome, Edge) | Click **Install app** in **Settings → App**, or the install icon in the address bar. |
| **Android** (Chrome) | **Settings → App → Install app**, or Chrome's **⋮ → Add to Home screen**. |
| **iPhone / iPad** (Safari) | **Share** → **Add to Home Screen**. You'll pick songs again after restarting, because iOS doesn't let web apps remember folders. |

Publishing it yourself? See [docs/DEPLOY.md](docs/DEPLOY.md).

---

## Breadboard player (ESP32)

Take your playlists away from the computer on a small music player you build yourself: an ESP32, a microSD card, a MAX98357A amp with a speaker, four buttons, and a colour screen (ST7789 or ST7735) or a 16×2 / 20×4 character LCD.

1. Build it and upload the code in [`firmware/`](firmware). The [breadboard guide](docs/BREADBOARD.md) has the parts list, wiring and steps.
2. In Localify, **Settings → Your Data → Export to SD card**, pick the playlists, and choose the card.
3. Put the card in the player. It shows your playlists, Liked Songs and All Songs.

Exporting needs Chrome or Edge on a computer.

---

## Keyboard shortcuts

| Key | Action |
| --- | --- |
| <kbd>Space</kbd> | Play / pause |
| <kbd>←</kbd> <kbd>→</kbd> | Skip back / forward 5 seconds |
| <kbd>Shift</kbd> + <kbd>←</kbd> <kbd>→</kbd> | Previous / next song |
| <kbd>↑</kbd> <kbd>↓</kbd> | Volume |
| <kbd>[</kbd> <kbd>]</kbd> | Slower / faster |
| <kbd>M</kbd> | Mute |
| <kbd>S</kbd> | Shuffle |
| <kbd>R</kbd> | Repeat mode |
| <kbd>L</kbd> | Like the current song |
| <kbd>V</kbd> | Visualizer |
| <kbd>T</kbd> | Focus timer |
| <kbd>Y</kbd> | Lyrics |
| <kbd>/</kbd> | Search |
| <kbd>,</kbd> | Settings |

**In the song list:**

| Key | Action |
| --- | --- |
| <kbd>↑</kbd> <kbd>↓</kbd> <kbd>Enter</kbd> | Move and play |
| <kbd>Ctrl</kbd> + <kbd>A</kbd> | Select all songs |
| <kbd>Ctrl</kbd> + click, <kbd>Ctrl</kbd> + <kbd>Space</kbd> | Pick songs one at a time |
| <kbd>Shift</kbd> + click, <kbd>Shift</kbd> + <kbd>↑</kbd> <kbd>↓</kbd> | Pick a range of songs |
| <kbd>Enter</kbd> | Play the picked songs |
| <kbd>Q</kbd> / <kbd>Shift</kbd> + <kbd>Q</kbd> | Add to queue / play next |
| <kbd>L</kbd> | Like (or unlike) |
| <kbd>Delete</kbd> | Remove from this playlist (or from Liked Songs) |
| <kbd>Esc</kbd> | Clear the selection |
| <kbd>E</kbd> | Edit audio |
| <kbd>I</kbd> | Tidy name & tags |

When you pick more than one song, a bar at the bottom lets you play them, queue them, like them or add them to a playlist. Right-click (or **⋯**) on a picked song does the same.

---

## Privacy

- 🎧 **Your music never leaves your device.** Nothing is uploaded, ever.
- 💾 **Your likes, settings and profiles** stay in your browser on this device.
- 🌐 **Online lookups** send only a song's title and artist text to MusicBrainz and iTunes. No file names, folders or audio are sent. You can turn this off.
- 🎤 **Lyrics lookups** send only a song's title, artist, album text and length to LRCLIB (lrclib.net), and only when you open the lyrics panel. `.lrc` files and lyrics in your tags are never sent. Turn it off with **Settings → Lyrics → Find lyrics online**.
- 🔊 **Identify by sound** (only with your own AcoustID key) sends a short fingerprint of the first two minutes and the song's length to AcoustID. The fingerprint is worked out on your computer; the audio itself is never sent.
- 🤖 **The AI** runs on your own computer and only sees names and tags, never the audio.

---

## Troubleshooting

<details>
<summary><b>Songs aren't being filled in</b></summary>

Check that you're online. The music database is sometimes busy, so filling in songs can take around 10 seconds each. Localify keeps retrying on its own. Songs waiting their turn show up under **Unsorted**.
</details>

<details>
<summary><b>No lyrics for a song</b></summary>

Put a `.lrc` file with the same name next to the song (for example `Song.mp3` and `Song.lrc`) and load the folder again. Online lyrics also need **Find lyrics online** and **Look up songs online** turned on in Settings.
</details>

<details>
<summary><b>"Apply" won't change my files</b></summary>

Use Chrome or Edge, and allow Localify to edit the folder when it asks.
</details>

<details>
<summary><b>My songs are gone after a reload or update</b></summary>

They're still saved. After a reload, the browser asks once more before Localify may open your folders. Click anywhere on the page and choose **Allow** (or **Allow on every visit**, so it stops asking), and the songs from all your folders come back.

**Firefox and Safari** can't reopen folders, so Localify saves its own copy of the songs you add (in the browser's private storage, about the size of your music) and loads them from there after a reload. The same goes for single songs added with **Add files** in any browser. Removing a folder from **Your folders**, or clearing the library, deletes the copy.
</details>

<details>
<summary><b>My likes or playlists disappeared</b></summary>

You may be in a different profile. Click the avatar in the top-right to switch. Likes and playlists are also saved per browser, so a different browser or device starts fresh unless you import a backup.
</details>

<details>
<summary><b>Start over without losing my likes</b></summary>

**Settings → General → Reset all settings** resets settings only. Your library, likes and playlists stay.
</details>

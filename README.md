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
- [Tidy names & tags (AI)](#tidy-names--tags-ai)
- [Lock-in focus timer](#lock-in-focus-timer)
- [Profiles and backups](#profiles-and-backups)
- [Install it as an app](#install-it-as-an-app)
- [Keyboard shortcuts](#keyboard-shortcuts)
- [Privacy](#privacy)
- [Troubleshooting](#troubleshooting)

---

## Getting started

1. Open **`localify_offline_mp3_player.html`** in Chrome or Edge. Double-clicking it works.
2. Click **Load Folder** and pick the folder that has your music.
3. Press **Play**, or **Shuffle Play**.

That's all. Localify remembers your likes, settings and the last song you played the next time you open it.

> [!TIP]
> Chrome and Edge work best. Other browsers can play music, but they can't save changes back into your files.

---

## What it can do

| | |
| --- | --- |
| 🎵 **Library** | All Tracks, Liked Songs, **Albums** and **Artists** views, plus search. |
| 🎨 **Looks** | Dark, Light or System mode, your choice of accent color, and compact rows. |
| 🌈 **Visualizer** | A full-screen visualizer that moves with the music. |
| 🎚️ **Sound** | Equalizer presets (Bass Boost, Vocal, Treble Boost, Loudness) or your own Bass/Mid/Treble, volume leveling and crossfade. |
| ⏩ **Speed & pitch** | Playback speed with one-tap **Nightcore**, **Daycore** and **Slowed + reverb** presets. Turn *Change pitch with speed* off to keep the original key. |
| 😴 **Sleep timer** | Fades out after 15, 30, 45 or 60 minutes, or at the end of the song. |
| ✂️ **Audio editor** | Trim, cut, silence, reverse, fade in/out, gain, normalize, speed and EQ. Press **E** on a song. |
| 🔊 **Audacity** | On Windows, send a song straight to Audacity (set up under **Settings → Integrations**). |

Find everything else in **Settings** (the gear icon, or press **,**).

---

## Tidy names & tags (AI)

Messy file names like `shape_of_you_FINAL (1).mp3` get cleaned up automatically.

**How it works**

- **New songs are filled in for you.** Title, artist, album, year, genre and album cover are looked up when you add them.
- **Offline? No problem.** Songs added without internet go to the **Unsorted** list and are filled in once you're back online.
- **It asks when it isn't sure.** When the chip at the top says *questions*, click it and answer a few quick ones together.
- **Your files are safe.** Nothing in the files changes until you press **Apply** in Tidy, and you can undo from there.

To tidy one song by hand, select it and press **I**, or use the **⋯** menu on its row.

**Settings → Tidy names & tags** lets you:

- choose the file name pattern (for example `{artist} - {title}`)
- turn online lookups or AI suggestions on or off
- let Localify write details straight into your files automatically

> [!NOTE]
> The AI model is about **500 MB** and downloads **once**, the first time it's needed. It runs on your own computer. Remove it any time with **Remove model**.

---

## Lock-in focus timer

Press **T** (or tap the timer icon) to start a focus session. Your music keeps playing; when time's up, it dips and an alarm rings.

- **Alarm sounds:** Soft Chime, Bell, Marimba, Digital Beep, Gentle Rise, Gong, or **your own sound file**.
- Choose how loud the alarm is and how quiet the music gets while it rings.
- Optional desktop notification, and vibration on phones.

---

## Profiles and backups

Share a computer? Each person can have their own **profile** with their own likes, settings and history. Click your avatar in the top-right corner to switch or add one.

To move your data to another device, open **Settings → Your data**:

- **Export backup** saves a small `.localify.json` file. Songs are never included.
- **Import backup** previews the file first. You can then add it as a new profile, merge it, or replace the current one.

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
| <kbd>/</kbd> | Search |
| <kbd>,</kbd> | Settings |

**In the song list:**

| Key | Action |
| --- | --- |
| <kbd>↑</kbd> <kbd>↓</kbd> <kbd>Enter</kbd> | Move and play |
| <kbd>L</kbd> | Like |
| <kbd>E</kbd> | Edit audio |
| <kbd>I</kbd> | Tidy name & tags |

---

## Privacy

- 🎧 **Your music never leaves your device.** Nothing is uploaded, ever.
- 💾 **Your likes, settings and profiles** stay in your browser on this device.
- 🌐 **Online lookups** send only a song's title and artist text to MusicBrainz and iTunes. No file names, folders or audio are sent. You can turn this off.
- 🤖 **The AI** runs on your own computer and only sees names and tags, never the audio.

---

## Troubleshooting

<details>
<summary><b>Songs aren't being filled in</b></summary>

Check that you're online. The music database is sometimes busy, so filling in songs can take around 10 seconds each. Localify keeps retrying on its own. Songs waiting their turn show up under **Unsorted**.
</details>

<details>
<summary><b>"Apply" won't change my files</b></summary>

Use Chrome or Edge, and allow Localify to edit the folder when it asks.
</details>

<details>
<summary><b>My likes disappeared</b></summary>

You may be in a different profile. Click the avatar in the top-right to switch. Likes are also saved per browser, so a different browser or device starts fresh unless you import a backup.
</details>

<details>
<summary><b>Start over without losing my likes</b></summary>

**Settings → General → Reset All Settings** resets settings only. Your library and likes stay.
</details>

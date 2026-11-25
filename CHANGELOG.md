# What's new in Localify

## 1.0.1 — polish

- **Your songs come back after a reload or an update.** The browser asks again before Localify may open your folders; now one click anywhere asks for all of them at once, and every folder's songs return to All Tracks.
- Folders you **drag onto the page** are now remembered after a reload too, like ones opened with **Load Folder**. 
- **Firefox and Safari keep your songs after a reload.** They can't reopen folders, so Localify saves its own copy of your songs in the browser and loads them from there. Single songs added with **Add Files** are kept the same way in every browser.
- **Pick up where you left off**: after a reload, the song you were playing is ready at the same spot, with your Up Next order (including songs you queued) and the list it came from.
- **A cleaner speed and sound menu**: speed and sound now sit in their own sections, and reverb is a slider (0 to 100%) instead of an on/off switch. The Nightcore, Daycore and Slowed + reverb buttons are gone: set the speed and reverb yourself. Each setting has a short line saying what it does.
- Buttons, menus and headings now use sentence case ("Load folder", "Up next", "Shuffle play").
- Notices now have a short title, an icon and a line of detail.
- **All Tracks shows each song's folder** (its name and icon) next to the artist.
- **Pick many songs at once**: <kbd>Ctrl</kbd>+<kbd>A</kbd>, <kbd>Ctrl</kbd>+click and <kbd>Shift</kbd>+click, then play, queue, like, add to a playlist or remove them together from the bar at the bottom. New keys in the song list: <kbd>Q</kbd> (add to queue), <kbd>Shift</kbd>+<kbd>Q</kbd> (play next), <kbd>Delete</kbd> and <kbd>Esc</kbd>.
- **Your own pictures and icons** for playlists and folders: upload a picture (it's cropped square), or type or paste any emoji.
- The *Auto* and *?* tags on songs are gone. Hover a song to see a small dot instead, or open its **⋯** menu to answer a question or see what Localify filled in.
- **Breadboard player**: build your own ESP32 music player with a colour screen or a character LCD, and fill its microSD card from **Settings → Your data → Export to SD card**. The export copies your playlists and their MP3/WAV songs, and skips songs already on the card. See the [breadboard guide](docs/BREADBOARD.md).
- **Playlists**: make your own, with a name, icon and color. Add songs from a song's **⋯** menu (**Add to playlist**). Liked Songs now sits at the top of the new **Playlists** area in the sidebar. Playlists are kept after reloads, per profile and in backups.
- Name your folders: when you add a folder, give it your own name, icon and color. It's remembered after reloads, kept in backups, and you can change it any time from **Your Folders** in the sidebar.
- Localify now asks the browser to keep its data, so a full device doesn't quietly clear your settings, likes and folders. **Settings → Your data** shows whether it said yes.
- Cleaner settings: sliders now fill with your accent color as you drag them, and the equalizer and sleep timer choices sit in a tidy grid.
- The song count and total time in the sidebar are now two small cards.
- Compact rows are slimmer: one line per song with the artist next to the title. On phones the title gets the whole line.

## 1.0.0 — first public release

**Playing music**
- Play the music on your own device from a folder or single files. Nothing is uploaded.
- All Tracks, Liked Songs, Albums, Artists and Unsorted views, plus search.
- Full-screen visualizer, equalizer presets, volume leveling, crossfade and a sleep timer.
- Speed control with Nightcore, Daycore and Slowed + reverb presets.

**Lyrics**
- Synced lyrics that highlight as the song plays; tap a line to jump there.
- Uses `.lrc` files and lyrics in your tags first, then LRCLIB online. Saved for offline use.

**Tidy names & tags**
- New songs get their title, artist, album, year, genre and cover filled in automatically when online.
- Songs added offline wait in **Unsorted** and are filled in once you're back online.
- When it isn't sure, Localify asks you a few quick questions instead of guessing.
- Optional: identify songs by their sound with your own free AcoustID key.
- A small AI model that runs on your own computer helps clean up messy names.

**Focus**
- Lock-in focus timer with built-in alarm sounds or your own.

**Editing**
- Built-in audio editor: trim, cut, fade, normalize, speed, EQ and more.
- On Windows, send a song to Audacity in one click.

**Your stuff**
- Profiles for everyone who shares the device, each with their own likes and settings.
- Backups you can move to another device. Songs are never included.
- Dark, Light or System mode and your own accent color.

**Everywhere**
- Install it as an app on Windows, Mac, Android and iPhone. Works offline.
- Works on phones and big screens alike.

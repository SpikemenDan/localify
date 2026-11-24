# Open in Audacity (Windows helper)

Browsers can't start programs on their own. This small helper registers a
`localify-audacity:` link for your Windows account. When you click **Open in
Audacity** in Localify, Windows runs the helper. The helper checks the file and
then opens it in Audacity.

The helper only ever starts Audacity. It refuses any path that:
- is outside the music folders you list in it,
- isn't an existing audio file, or
- contains `..`, wildcards, quotes or control characters.

## Install (once, no admin rights needed)

1. Create the folder `%LOCALAPPDATA%\Localify Audacity Helper\` (paste that
   into the Explorer address bar).
2. Copy `localify-audacity.ps1` into that folder.
3. Open the copied file in Notepad and set `$MusicRoots` to your music
   folder(s). Localify shows the exact line to paste under
   **Settings → Integrations → Audacity**, for example:
   ```powershell
   $MusicRoots = @('D:\Music', 'C:\Users\you\Music')
   ```
   If Audacity isn't installed in a standard location, also set
   `$AudacityPath`.
4. Double-click `install-localify-audacity.reg` and confirm. This only writes
   under `HKEY_CURRENT_USER\Software\Classes\localify-audacity`.

In Localify, the first click on **Open in Audacity** opens
**Settings → Integrations → Audacity** and asks for the full path of the folder
you loaded (for example `D:\Music`). Change or forget saved paths there at any
time; Shift+click the button also jumps there.

## Try it without Localify

Run the helper in dry-run mode. It prints what it would do and opens nothing:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File "$env:LOCALAPPDATA\Localify Audacity Helper\localify-audacity.ps1" -DryRun "localify-audacity:D%3A%5CMusic%5Csong.mp3"
```

After installing the `.reg` file, test the real link by pressing Win+R and
running:

```
localify-audacity:D%3A%5CMusic%5Csong.mp3
```

Use a real file from one of your `$MusicRoots`.

## Uninstall

1. Double-click `uninstall-localify-audacity.reg`. This removes the link.
2. Delete `%LOCALAPPDATA%\Localify Audacity Helper\`.

## Notes

- Tracks added with the file picker (rather than Open Folder or dropping a
  folder) have no known folder, so their Audacity button is disabled.
- Localify stores the folder paths you enter in the browser's localStorage
  under `localify_audacity_roots_v1`.

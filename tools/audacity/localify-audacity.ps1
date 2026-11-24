<#
  Localify -> Audacity helper.

  Windows runs this script when a "localify-audacity:" link is opened (see
  install-localify-audacity.reg). The link carries one URL-encoded absolute
  file path. The script opens that file in Audacity only if ALL of these hold:
    * the path is inside one of the folders listed in $MusicRoots below
    * it is an existing file (not a folder) with an audio extension
    * it contains no "..", wildcards, quotes or control characters
  Anything else is refused with a message box. Nothing but Audacity is ever
  started, and the path is passed to it as a single argument.

  Test without the registry:
    powershell -NoProfile -ExecutionPolicy Bypass -File .\localify-audacity.ps1 -DryRun "localify-audacity:C%3A%5CUsers%5Cyou%5CMusic%5Csong.mp3"
#>
param(
  [Parameter(Position = 0)] [string] $Url = '',
  [switch] $DryRun
)

# ---------------------------------------------------------------- Configuration
# Folders Localify may open files from. Use the paths the app shows in its
# "Open in Audacity" setup dialog. One entry per music folder you load.
$MusicRoots = @(
  "$env:USERPROFILE\Music"
)

# Leave empty to find Audacity automatically, or set the full path to audacity.exe.
$AudacityPath = ''
# ------------------------------------------------------------------------------

$ErrorActionPreference = 'Stop'
$AudioExtensions = @('.mp3', '.wav', '.flac', '.ogg', '.oga', '.opus', '.m4a', '.aac', '.wma', '.aif', '.aiff', '.webm', '.mp4')

function Show-Message([string] $text, [int] $icon = 48) {
  if ($DryRun) { Write-Output "MESSAGE: $text"; return }
  try {
    (New-Object -ComObject WScript.Shell).Popup($text, 0, 'Localify - Open in Audacity', $icon) | Out-Null
  } catch { }
}

function Fail([string] $text) {
  Show-Message $text 48
  exit 1
}

function Find-Audacity {
  if ($AudacityPath) {
    if (Test-Path -LiteralPath $AudacityPath -PathType Leaf) { return $AudacityPath }
    Fail "Audacity was not found at the path set in the helper:`n$AudacityPath"
  }
  $candidates = @(
    (Join-Path $env:ProgramFiles 'Audacity\audacity.exe'),
    (Join-Path ${env:ProgramFiles(x86)} 'Audacity\audacity.exe'),
    (Join-Path $env:LOCALAPPDATA 'Programs\Audacity\audacity.exe')
  )
  foreach ($c in $candidates) {
    if ($c -and (Test-Path -LiteralPath $c -PathType Leaf)) { return $c }
  }
  foreach ($hive in 'HKCU:', 'HKLM:') {
    try {
      $p = (Get-ItemProperty -LiteralPath "$hive\Software\Microsoft\Windows\CurrentVersion\App Paths\audacity.exe" -ErrorAction Stop).'(default)'
      if ($p) {
        $p = $p.Trim('"')
        if (Test-Path -LiteralPath $p -PathType Leaf) { return $p }
      }
    } catch { }
  }
  return $null
}

# ---------------------------------------------------------------- Parse the link
$raw = $Url.Trim()
if ($raw -notmatch '^localify-audacity:') { Fail 'This helper only accepts localify-audacity: links.' }
$encoded = $raw.Substring('localify-audacity:'.Length).TrimStart('/')
# Some browsers add a trailing slash to custom links
if ($encoded.EndsWith('/')) { $encoded = $encoded.TrimEnd('/') }
try {
  $path = [System.Uri]::UnescapeDataString($encoded)
} catch {
  Fail 'The link could not be read.'
}

# ---------------------------------------------------------------- Validate the path
if ([string]::IsNullOrWhiteSpace($path)) { Fail 'The link did not contain a file path.' }
if ($path -match '[\x00-\x1f"*?<>|]') { Fail "Refused: the path contains characters that are not allowed.`n$path" }
$path = $path -replace '/', '\'
if ($path -notmatch '^[A-Za-z]:\\' -and $path -notmatch '^\\\\[^\\]+\\[^\\]+') { Fail "Refused: not an absolute Windows path.`n$path" }
if ($path.Substring(2).Contains(':')) { Fail "Refused: alternate data streams are not allowed.`n$path" }
if ($path -match '(^|\\)\.{1,2}(\\|$)') { Fail "Refused: the path contains . or .. segments.`n$path" }

try { $full = [System.IO.Path]::GetFullPath($path) } catch { Fail "Refused: invalid path.`n$path" }

$inRoot = $false
foreach ($root in $MusicRoots) {
  if ([string]::IsNullOrWhiteSpace($root)) { continue }
  try { $r = [System.IO.Path]::GetFullPath($root).TrimEnd('\') + '\' } catch { continue }
  if ($full.StartsWith($r, [System.StringComparison]::OrdinalIgnoreCase)) { $inRoot = $true; break }
}
if (-not $inRoot) {
  Fail ("Refused: this file is outside the music folders the helper allows.`n$full`n`n" +
        "Add its folder to `$MusicRoots in:`n$PSCommandPath")
}

$ext = [System.IO.Path]::GetExtension($full).ToLowerInvariant()
if ($AudioExtensions -notcontains $ext) { Fail "Refused: not an audio file.`n$full" }
if (-not (Test-Path -LiteralPath $full -PathType Leaf)) { Fail "The file was not found. It may have been moved or renamed.`n$full" }

# ---------------------------------------------------------------- Launch
$exe = Find-Audacity
if (-not $exe) { Fail 'Audacity was not found. Install it from audacityteam.org, or set $AudacityPath in the helper.' }

if ($DryRun) {
  Write-Output "OK: would run `"$exe`" with `"$full`""
  exit 0
}
Start-Process -FilePath $exe -ArgumentList ('"' + $full + '"')

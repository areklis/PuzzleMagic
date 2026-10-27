<#
Builds the test packages: compiles the editor target, cooks and packages Windows and/or Android, and makes the Windows zip.

  powershell -File Tools\build_packages.ps1 [-Win] [-Android] [-Demo] [-Ue "C:\Epic Games\UE_5.8"] [-Out "D:\PuzzleMagic_Builds"]

-Demo also makes WyrdHandsBlockPuzzle_Demo_Windows.zip, from a separate Shipping build (no firewall prompt); a DemoBuild.txt marker in it
makes the game the demo. -DemoOnly builds just that.
-Release makes WyrdHandsBlockPuzzle_Windows.zip: the full game (no demo switches) as a Shipping build with the branded launcher that keeps saves
in the game folder. -ReleaseApk makes WyrdHandsBlockPuzzle_Android.apk: the full game, Shipping. -Distribution adds the engine's -distribution flag to
the release APK (not debuggable); it needs the release keystore set in Project Settings > Android. Either one alone builds just that.

With neither -Win nor -Android, both are built. Defaults: the engine named by the .uproject's EngineAssociation (looked up in the
registry, then under the usual folders) and the folder PuzzleMagic_Builds next to the repo. Android needs the Android SDK (adb and
the NDK 27.2.12479018) and JDK 21 (found under Program Files, or pass -Jdk); set ANDROID_HOME if the SDK is not in its usual place.
Close the editor and any running game first, and never run two builds at once. The cook sometimes dies at teardown with an engine
ensure after "Done!"; each package is retried once.
#>
param(
  [switch]$Win, [switch]$Android, [switch]$SkipCompile, [switch]$DryRun, [switch]$Demo, [switch]$DemoOnly, [switch]$DemoApk, [switch]$DemoDev, [switch]$Release, [switch]$ReleaseApk, [switch]$Distribution,
  [string]$Ue = "",
  [string]$Jdk = "",
  [string]$Out = ""
)
$ErrorActionPreference = "Stop"
$repo = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$uproject = Join-Path $repo "WyrdHandsBlockPuzzle.uproject"
if ($DemoOnly) { $Demo = $true; $Win = $false; $Android = $false }
$releaseOnly = ($Release -or $ReleaseApk) -and -not ($Win -or $Android -or $Demo -or $DemoApk -or $DemoOnly)
if ($releaseOnly) { $Win = $false; $Android = $false }
if ($DemoApk -and -not ($Win -or $Android -or $Demo)) { $Win = $false; $Android = $false } # -DemoApk alone builds just the demo APK
elseif (-not ($Win -or $Android -or $DemoOnly -or $releaseOnly)) { $Win = $true; $Android = $true }
if (-not $Out) { $Out = Join-Path (Split-Path -Parent $repo) "PuzzleMagic_Builds" }
New-Item -ItemType Directory -Force $Out | Out-Null

# --- the engine
if (-not $Ue) {
  $assoc = (Get-Content $uproject -Raw | ConvertFrom-Json).EngineAssociation
  foreach ($c in @("HKCU:\Software\Epic Games\Unreal Engine\Builds", "HKLM:\SOFTWARE\EpicGames\Unreal Engine\$assoc")) {
    if (Test-Path $c) {
      $p = Get-ItemProperty $c
      $v = $p.$assoc; if (-not $v) { $v = $p.InstalledDirectory }
      if ($v -and (Test-Path $v)) { $Ue = $v; break }
    }
  }
  if (-not $Ue) { foreach ($d in "C:\Program Files\Epic Games\UE_$assoc", "D:\Epic\UE_$assoc", "K:\Epic\UE_$assoc") { if (Test-Path $d) { $Ue = $d; break } } }
}
if (-not $Ue -or -not (Test-Path (Join-Path $Ue "Engine\Build\BatchFiles\RunUAT.bat"))) { throw "Unreal Engine not found: pass -Ue <install folder>" }
$uat = Join-Path $Ue "Engine\Build\BatchFiles\RunUAT.bat"
$buildBat = Join-Path $Ue "Engine\Build\BatchFiles\Build.bat"
Write-Host "engine: $Ue  output: $Out"
if ($DryRun) { Write-Host "dry run: would build win=$Win android=$Android compile=$(-not $SkipCompile)"; return }

function Stop-Leftovers { Get-Process UnrealEditor, UnrealEditor-Cmd, dotnet -ErrorAction SilentlyContinue | Stop-Process -Force; Start-Sleep 3 }
# Every package starts from an empty output folder: files left by an older build (the old name's folder, an older APK) would otherwise
# be zipped, branded or copied instead of the new ones.
function Clear-OutputDir($dir) {
  $full = [IO.Path]::GetFullPath($dir)
  if ($full.StartsWith([IO.Path]::GetFullPath($Out), [StringComparison]::OrdinalIgnoreCase) -and (Test-Path $full)) { Remove-Item -LiteralPath $full -Recurse -Force }
}
function Run-Uat($packageArgs, $log) {
  foreach ($try in 1..2) {
    & $uat @packageArgs *> $log
    if ($LASTEXITCODE -eq 0) { return }
    Write-Host "package attempt $try failed (exit $LASTEXITCODE), see $log"
    Stop-Leftovers
  }
  throw "packaging failed twice: $log"
}

Stop-Leftovers
if (-not $SkipCompile) {
  & $buildBat WyrdHandsBlockPuzzleEditor Win64 Development "-Project=$uproject" -WaitMutex
  if ($LASTEXITCODE -ne 0) { throw "compile failed" }
  Get-Process dotnet -ErrorAction SilentlyContinue | Stop-Process -Force
}

$common = @("BuildCookRun", "-project=$uproject", "-noP4", "-clientconfig=Development", "-build", "-cook", "-stage", "-pak", "-archive", "-utf8output", "-unattended", "-nocompileeditor")

if ($Win) {
  $dir = Join-Path $Out "WindowsPortable"
  Clear-OutputDir $dir
  Run-Uat ($common + @("-platform=Win64", "-archivedirectory=$dir")) (Join-Path $Out "win_portable_build.log")
  python (Join-Path $repo "Tools\package_windows.py") $dir (Join-Path $Out "WyrdHandsBlockPuzzle_Windows_Portable.zip")
}

# The demo is a Shipping build: Development builds open a profiler port (TraceLog, TCP 1985) that makes Windows Firewall ask the player
# to allow the game. Shipping compiles it out. (No console, no log file: use the Development package to test with commands.)
# One Shipping Windows package: the demo (-Demo, a DemoBuild.txt in the zip) or the full game (-Release). Same steps either way.
function Build-WindowsShipping([string]$Name, [string]$Zip, [string]$ZipMode) {
  $shipArgs = $common | ForEach-Object { $_ -replace '^-clientconfig=Development$', '-clientconfig=Shipping' }
  $dir = Join-Path $Out "Windows$Name"
  Clear-OutputDir $dir
  Run-Uat ($shipArgs + @("-platform=Win64", "-archivedirectory=$dir")) (Join-Path $Out "win_$($Name.ToLower())_build.log")
  # A Shipping game saves to AppData unless it runs with -NotInstalled. The engine's own launcher exe cannot carry that argument, so the
  # zip gets ours (Tools/DemoLauncher): a double-click then keeps every file in the game's folder.
  $launcher = Join-Path $Out "DemoLauncher\WyrdHandsBlockPuzzle.exe"
  New-Item -ItemType Directory -Force (Split-Path $launcher) | Out-Null
  powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $repo "Tools\DemoLauncher\build_launcher.ps1") -Out $launcher
  if ($LASTEXITCODE -ne 0 -or -not (Test-Path $launcher)) { throw "launcher build failed" }
  # The engine stamps every game exe "UnrealGame / Epic Games" and gives it its own icon: stamp the real name, company, copyright and the
  # icon into the launcher and into the game exe (Tools/brand_exe.py).
  $ico = Join-Path $repo "Build\Windows\Application.ico"
  python (Join-Path $repo "Tools\brand_exe.py") $launcher --ico $ico --original "WyrdHandsBlockPuzzle.exe"
  if ($LASTEXITCODE -ne 0) { throw "branding the launcher failed" }
  $gameExe = Get-ChildItem $dir -Recurse -Filter "*-Win64-Shipping.exe" | Select-Object -First 1
  python (Join-Path $repo "Tools\brand_exe.py") $gameExe.FullName --ico $ico --original "WyrdHandsBlockPuzzle.exe"
  if ($LASTEXITCODE -ne 0) { throw "branding the game exe failed" }
  python (Join-Path $repo "Tools\package_windows.py") $dir (Join-Path $Out $Zip) $ZipMode --launcher $launcher
  if ($LASTEXITCODE -ne 0) { throw "zipping $Zip failed" }
}
if ($Demo) { Build-WindowsShipping "Demo" "WyrdHandsBlockPuzzle_Demo_Windows.zip" "--demo" }
if ($Release) { Build-WindowsShipping "Release" "WyrdHandsBlockPuzzle_Windows.zip" "--release" }

function Initialize-Android {
  if (-not $env:ANDROID_HOME) { $env:ANDROID_HOME = Join-Path $env:LOCALAPPDATA "Android\Sdk" }
  $env:ANDROID_SDK_ROOT = $env:ANDROID_HOME
  if (-not $env:NDKROOT) { $env:NDKROOT = Join-Path $env:ANDROID_HOME "ndk\27.2.12479018" }
  $env:NDK_ROOT = $env:NDKROOT
  # Gradle needs Java 11 or newer and the engine's Android setup expects JDK 21: an old JAVA_HOME (Java 8) breaks the APK step.
  $jdk = $Jdk
  if (-not $jdk) { foreach ($pattern in "C:\Program Files\Java\jdk-21*", "C:\Program Files\Eclipse Adoptium\jdk-21*", "C:\Program Files\Microsoft\jdk-21*") { $f = Get-Item $pattern -ErrorAction SilentlyContinue | Select-Object -First 1; if ($f) { $jdk = $f.FullName; break } } }
  if ($jdk) { $env:JAVA_HOME = $jdk } elseif (-not $env:JAVA_HOME) { throw "no JDK 21 found: install it or pass -Jdk <folder>" }
  Write-Host "JAVA_HOME: $env:JAVA_HOME"
}

# UnrealBuildTool does not notice that PUZZLE_DEMO changed, so a Development or Shipping Android library built as the demo would be reused
# by the next full-game build (it came out with the DEMO 1 / DEMO 2 menu). Remember which kind each configuration was last built as and
# throw that configuration's Android objects and library away when the kind changes.
function Reset-AndroidIfFlavorChanged([string]$Config, [string]$Flavor) {
  $stampDir = Join-Path $repo "Intermediate\PuzzleDemo"
  New-Item -ItemType Directory -Force $stampDir | Out-Null
  $stamp = Join-Path $stampDir "android_$Config.txt"
  $last = if (Test-Path $stamp) { (Get-Content $stamp -Raw).Trim() } else { "" }
  if ($last -ne $Flavor) {
    Write-Host "Android $Config was last built as '$last', now '$Flavor': clearing its objects and library"
    $suffix = if ($Config -eq "Shipping") { "-Android-Shipping" } else { "" }
    Get-ChildItem (Join-Path $repo "Binaries\Android") -Filter "WyrdHandsBlockPuzzle$suffix*" -ErrorAction SilentlyContinue |
      Where-Object { $Config -eq "Shipping" -or $_.Name -notlike "*Shipping*" } | Remove-Item -Recurse -Force -ErrorAction SilentlyContinue
    Get-ChildItem (Join-Path $repo "Intermediate\Build\Android") -Recurse -Directory -Filter $Config -ErrorAction SilentlyContinue |
      Remove-Item -Recurse -Force -ErrorAction SilentlyContinue
  }
  Set-Content $stamp $Flavor
}

# The demo APK: a Shipping build (no console, no debug hooks) compiled as the demo through PUZZLE_DEMO=1 (see PuzzleDemo.h; a phone has
# no marker file). It has the same package name as the full game, so it replaces an installed test build.
if ($DemoApk) {
  Initialize-Android
  $dir = Join-Path $Out $(if ($DemoDev) { "AndroidDemoDev" } else { "AndroidDemo" })
  Clear-OutputDir $dir
  $apkName = if ($DemoDev) { "WyrdHandsBlockPuzzle_Demo_Android_dev.apk" } else { "WyrdHandsBlockPuzzle_Demo_Android.apk" }
  # -DemoDev builds the same demo as a Development APK (for the debug commands and the cap test); a release is the Shipping one.
  $demoApkArgs = if ($DemoDev) { $common } else { $common | ForEach-Object { $_ -replace '^-clientconfig=Development$', '-clientconfig=Shipping' } }
  # -Distribution signs the demo APK with the release key too (not debuggable), so it carries the same publisher as the full game
  if ($Distribution -and -not $DemoDev) { $demoApkArgs += "-distribution" }
  # the demo APK has no ACCESS_NETWORK_STATE: a patched copy of the engine's GameActivity template copes (see Tools/make_demo_activity.py)
  python (Join-Path $repo "Tools\make_demo_activity.py") $Ue $repo
  if ($LASTEXITCODE -ne 0) { throw "make_demo_activity.py failed" }
  Reset-AndroidIfFlavorChanged $(if ($DemoDev) { "Development" } else { "Shipping" }) "demo"
  $env:PUZZLE_DEMO = "1"
  try { Run-Uat ($demoApkArgs + @("-platform=Android", "-cookflavor=ASTC", "-package", "-archivedirectory=$dir")) (Join-Path $Out "android_demo_build.log") }
  finally { Remove-Item Env:PUZZLE_DEMO -ErrorAction SilentlyContinue }
  $apk = Get-ChildItem $dir -Filter "*.apk" -Recurse | Select-Object -First 1
  if (-not $apk) { throw "no APK in $dir" }
  Copy-Item $apk.FullName (Join-Path $Out $apkName) -Force
  Write-Host "demo APK: $(Join-Path $Out $apkName)"
}

# The full game for Android: a Shipping build without the demo switch. -Distribution adds the engine's -distribution flag (not
# debuggable, needs the release keystore from Project Settings > Android).
if ($ReleaseApk) {
  Initialize-Android
  $dir = Join-Path $Out "AndroidRelease"
  Clear-OutputDir $dir
  $relArgs = $common | ForEach-Object { $_ -replace '^-clientconfig=Development$', '-clientconfig=Shipping' }
  if ($Distribution) { $relArgs += "-distribution" }
  # the same lean manifest as the demo (no network or purchase permissions; the game uses none) and the patched GameActivity template
  python (Join-Path $repo "Tools\make_demo_activity.py") $Ue $repo
  if ($LASTEXITCODE -ne 0) { throw "make_demo_activity.py failed" }
  Reset-AndroidIfFlavorChanged "Shipping" "full-lean"
  $env:PUZZLE_LEAN_ANDROID = "1"
  try { Run-Uat ($relArgs + @("-platform=Android", "-cookflavor=ASTC", "-package", "-archivedirectory=$dir")) (Join-Path $Out "android_release_build.log") }
  finally { Remove-Item Env:PUZZLE_LEAN_ANDROID -ErrorAction SilentlyContinue }
  $apk = Get-ChildItem $dir -Filter "*.apk" -Recurse | Select-Object -First 1
  if (-not $apk) { throw "no APK in $dir" }
  Copy-Item $apk.FullName (Join-Path $Out "WyrdHandsBlockPuzzle_Android.apk") -Force
  Write-Host "release APK: $(Join-Path $Out 'WyrdHandsBlockPuzzle_Android.apk')"
}

if ($Android) {
  Initialize-Android
  $dir = Join-Path $Out "AndroidDev"
  Clear-OutputDir $dir
  Reset-AndroidIfFlavorChanged "Development" "full"
  Run-Uat ($common + @("-platform=Android", "-cookflavor=ASTC", "-package", "-archivedirectory=$dir")) (Join-Path $Out "android_dev_build.log")
  Copy-Item (Join-Path $dir "WyrdHandsBlockPuzzle-arm64.apk") (Join-Path $Out "WyrdHandsBlockPuzzle_Android_test.apk") -Force
  Write-Host "APK: $(Join-Path $Out 'WyrdHandsBlockPuzzle_Android_test.apk')"
}
Write-Host "done"

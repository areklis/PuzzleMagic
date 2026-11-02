# Review notes, 27 Oct 2026 (written while the designer slept; nothing here is committed)

Branch `wyrd-hands-release` (local only). Nothing committed or pushed. Builds are in `K:\epic2\PuzzleMagic_Builds`:
- `WyrdHandsBlockPuzzle_Windows.zip` (307 MB, Shipping, full game) and `WyrdHandsBlockPuzzle_Android.apk` (127 MB, Shipping, full game, 05:14 / 05:18).
- `WyrdHandsBlockPuzzle_Android_test.apk` is a Development build with the test commands (it is on no device now; the release APK is installed on the tablet).

## Done and verified
Rename, the whole Halloween event run (see Specs/TrickOrTreatEvent.md), and this session's changes:
- **Pumpkins:** Kenney `pumpkin-carved` (CC0), carved insides near black, tilted back toward the camera (`puzzle.PumpkinTilt`, -50). Seen
  correct on PC (landscape and portrait) and on the tablet.
- **Packaging bug found and fixed:** assets loaded by name were not cooked (`/Game/Event` is now in DirectoriesToAlwaysCook). The Windows
  package was listed to confirm the mesh, texture and material are inside; the tablet showed the real pumpkin with the Development APK.
- **Candles:** small, behind the board, sized and placed from the board (not the camera): in view in landscape, phone and tablet portrait.
- **Spots:** landscape 2x2 per side; portrait one column of four per side (the board fills the width there).
- **Layout check (PC windows 430x950, 820x1180, 1280x800/16:9):** menu, options, the longest-type riddle card with its lore line, the
  floor, the end screens: no overflow or clipping found. Tablet device (landscape): menu, floor, riddle, end screen fine.
- **Tablet frame rate** (event, bot playing, full floor, candles full, 8 s samples): median 28.6, min 24.3, max 29.8 (14 samples); final
  release APK on the event start: median 29.2. It dips to about 24-26 during riddle cards and clears; the 30 fps cap is the ceiling.
- **Flows run on PC through the real code:** five right answers -> Success screen, day saved, candles five fifths, Halloween unlocked;
  Theme option AUTO -> DESK -> HALLOWEEN (shown on the options card); Desk -> event starts after a map reload in the Halloween theme.
- **ARCADE button hidden** (`bShowArcadeButton` in PuzzleHUDWidget.cpp; the campaign code is untouched).
- **Sound stays on when the window loses focus:** `FApp::SetUnfocusedVolumeMultiplier(1)` in code plus `[Audio] UnfocusedVolumeMultiplier=1.0`.
- Release APK permissions: WAKE_LOCK, VIBRATE, MODIFY_AUDIO_SETTINGS only. Windows zip: no DemoBuild.txt, no debug files, saves stay in the game folder.

## Decisions for you
1. The APK is flagged debuggable (it needs your release keystore and `-Distribution`).
2. The Play now button is not built (its seven questions are still open).
3. Greek: event texts and riddles are English only.
4. Next year's event dates, and whether candle progress resets yearly (you said TBD). Dates are by the device's own clock.
5. Classic theme is only reachable with `-theme=classic` now.
6. The Trick-or-Treat shop and the old wheel stay in the code, off everywhere.

## Not verified (be skeptical here)
- A real touch on a riddle answer button in a live run on a device (the riddle card was shown and answered by test command; the tap on the
  end screen was done for real on the tablet).
- Real play to 300 points in the Shipping builds (the release builds have no test commands); the same code ran in the Development build.
- Portrait on the actual tablet or a phone: only PC windows of those shapes.
- The sound-on-unfocus change was not listened to (no way to hear it from here); it is set in code and config.
- The tap back to the menu from the end screen restoring the Desk theme after an event started from Desk (the reload path was checked
  going in, not coming out).
- Candle clusters in portrait are small; judge them with your own eyes.
- Test saves: the PC save was backed up and restored after the flow tests. The tablet app data was cleared before each device test, so
  the tablet has a fresh save (no best score, tutorial not yet skipped).

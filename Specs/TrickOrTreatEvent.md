# Trick or Treat: the Halloween seasonal event mode

Status: BUILD STARTED 2026-10-27 (designer: "start now"). All stages are built back to back; one report at the end. Written from the designer's messages of 2026-10-26/27.

## Where it lives
- A **Trick or Treat** button sits above the **Play now** button on the main menu.
- The button exists **only during a seasonal event**. Outside an event there is no such button.
- The Halloween event runs **from now until the end of November 2026**; dates will be fine-tuned later.
- Other seasons (Christmas, Lunar New Year, Easter, ...) will follow the same pattern later.

## The run
- Board: **6x6**. The event always plays in the **event's theme (Halloween)**, whatever the player's theme setting is.
- **Relics, luck and bonus tiles are ON** in the event mode.
- **Every 300 points** a riddle appears (same interval the old packet used; next = next multiple of 300 above the score).
- Riddles come from the **Halloween event pool: 55 riddles**, every topic except Day of the Dead and Mardi Gras / New Orleans (Halloween 19, Monsters 15, Dark folklore 11, Gothic tales 7, Myth 3). Which riddle appears is random; none repeats until all have been asked (the save remembers which).
- **Right answer = Treat!**, **wrong answer = Trick!**. Which of the outcomes shows is as follows (no extra reward or punishment: the counters below are the consequence).

### The floor and the 8 spots
- In this mode the **floor around the play area starts empty** (no cauldrons, bats or steam). Normal Halloween play is unchanged.
- There are **8 spots on the floor beside the play area: 4 on the left, 4 on the right**.
- **Treat!** spawns a **cauldron** in a random empty spot. **Trick!** spawns a **pumpkin** in a random empty spot.
- Once all 8 are full: a Treat! adds nothing more. A **Trick! replaces a random cauldron with a pumpkin**. (At most 4 pumpkins can exist before the run ends, so a cauldron is always there to replace.)

### Ending the run
- **5 Tricks = game over.** The screen **fades to black**, then the prompt **"click or touch the screen to go back"** (to the menu).
- **5 Treats, the first time that day = success.** Same prompt.
- After the first success of the day, later runs the same day **play on without the success screen** (5 Treats then does nothing special). The 5-Tricks game over still applies. The day changes with the **system's local date**.
- The run also ends **the normal way** when no piece can be placed (game over, same prompt).

### The candles (progress across days)
- A small **clutter of candles above the play area** fills from left to right.
- It fills by **about one fifth for each day's first success**: five successes on **any five different days of the event** (not consecutive).
- When it is full, the save **unlocks the Halloween theme** in the options menu (see Theme option).
- Progress is kept in the **local save**.

## Theme option (new, in the menu)
- **Auto (default):** the game's theme changes by itself to the event's theme during an event.
- **Desk:** keeps the Desk theme everywhere **except** when playing the event mode, which always uses the event's theme.
- Once the candles are full, **Halloween** is added to this list as a third choice.

## Not part of this (decided)
- The old Trick-or-Treat **shop (TREAT-shop)** and the old **TRICK wheel** are not used. The shop code stays in the project, switched off, as the baseline for a future shop.
- No luck or other extra reward for a right answer: the Treat!/Trick! outcome and its counter are enough.

## Open
1. Event dates for next year and whether candle progress resets each year: **TBD later** (designer).
2. Whether the plain Classic theme stays reachable (it is a test option in Play options today).
3. What Auto shows outside events (assumed: Desk).
4. Greek: the riddles, lore lines and event texts are English-only until the designer says otherwise.

## Decided in the last answers
- The riddle appears **when triggered** (no button to open it later; the game is not aimed at fast reflexes).
- Game-over and success screens: **fade to black**, then the click-or-touch prompt.
- Theme list: Auto, Desk, and Halloween once unlocked.

## Status 2026-10-27 (end of the build)
All five stages are built and tested on PC (screenshots in the job folder): riddle card and trigger, empty floor with the 8 spots, end
screens, candles with saved days, menu button and Theme option. Windows and Android release builds were made after them. Not yet
checked: the Android tablet pass of the new scenes, other screen shapes, the real click/touch return from the end screen,
and the Play now button (still waiting for the answers to its questions).

## Build stages (built back to back)
1. Event core + riddle card + the 300-point trigger + treat/trick counters.
2. The empty floor and the 8 spots (cauldrons and pumpkins).
3. The end screens: game over (5 tricks) and the daily success (5 treats), fade to black, the touch prompt.
4. The candles and the saved progress by date.
5. The Trick or Treat menu button (only during the event window), the Theme option, the Halloween unlock.

# Nightfall source

This is the 6502 assembly for Nightfall Beta 1, the build shown on the main menu as `BETA 1 AA0B928`. Building it produces the released `nightfall.ssd`, `NFASSETS.rom` and `EAUDIO.rom` byte for byte.

## Building

You need [BeebAsm](https://github.com/stardot/beebasm) and Python 3. From the top of this repository, run:

    ./build.sh

If BeebAsm is not on your path, set `BEEBASM` to where it is. On Windows, run the script from Git Bash or WSL. The results go in `build/`: use them with `EGFX.rom` and `EGFX2.rom` from the top of the repository, in the slots the main README lists.

If you change the game, change the version line in `build/version.inc` too, so that your build can be told apart in bug reports. The script works out the label addresses and overlay sizes each time, so code can move freely, and the assembler stops if a program outgrows its window.

`build/` also holds what the source needs to assemble:

- `eg-client.6502` is the small stub that calls EGFX: `eg_call` takes the function number in A and the parameters in EGBUF from offset 8, then calls OSWORD &C0.
- `NFSPR16`, `NFSPR4` and `NFSPR8` are the built sprite packs, with `sprite-map.inc` and `world-frames.inc` as their tables.
- `JUPITER` and `URANUS` are the built music scores.
- `PAUSEFRAME`, `PAUSETITLE` and `SCREEN` are the pause screen artwork and the loading picture, as raw screen data.
- `splash.vdu` and `splash.inc` set the boot screen's mode and palette, and `WPTEST` is the file NFBOOT uses to test that the disc can be written to.

Comments that name `tools/` scripts or other `.md` files refer to the scripts and notes that made these files, which are kept outside this repository.

## Where everything goes

| Source | Options | Builds |
|---|---|---|
| `bootstrap.6502` | | NFBOOT |
| `overlay-loader.6502` | | NFLOAD |
| `engine.6502` | `ARENA=0`, `ARENA=1` | NIGHT, NARENA |
| `ui.6502` | `CARD=0`, `CARD=1` | NIGHTUI, NCARDS |
| `reset.6502` | | NRESET |
| `players.6502` | | NPLAYER |
| `combat-overlay.6502` | | NFCOMB |
| `elite-page.6502`, `wizard-page.6502` | | PELITE, PWIZARD |
| `boss-bell.6502`, `boss-wyrm.6502`, `boss-choir.6502` | | PBELL, PWYRM, PCHOIR |
| `weapons.6502` | | NWEAPON, NWEAPX, NWEAPB |
| `weapon-slot.6502` | `SLOT` 0 or 1, `WEAPON_KIND` 1 to 4 | ORB, SEED, STORM and WARD, A and B |
| `eaudio.6502` | | `EAUDIO.rom` |
| `nf-companion.6502` | | `NFASSETS.rom` |
| `disc.6502` | `-do build/nightfall.ssd -opt 3 -title NIGHTFALL` | `nightfall.ssd` |

The `.inc` files are included by those programs: start with `nfvars.inc`, which names the shared variables and their addresses.

## How it fits together

NFBOOT tests that the disc can be written to, shows the loading picture, then loads NFLOAD and NIGHT. The game programs use only MOS calls, which is how one set of binaries runs on a BBC B, a Master and across a Tube (the EAudio ROM is the one part that touches hardware, to page its own banks).

NIGHT is the game during play. Everything else takes turns in the same window: to run a menu or a level-up, NFLOAD loads the overlay over NIGHT at &1900, calls its entry at &1908 plus 3 times the entry number, then loads NIGHT back. An overlay must return to NFLOAD, because the engine is not in memory while it runs. The table at `overlay_select` shows which file serves each entry.

Drawing goes through `eg_call` to the EGFX ROMs, and the sprites live in `NFASSETS.rom` in slot 7. The world's state sits in the sideways RAM in slot 6. Sound is `EAUDIO.rom`, which plays scores and effects from an interrupt.

## Memory

| Range | Owner |
|---|---|
| &0900..&0AFF | the current enemy phase or boss page (PELITE, PWIZARD, PBELL, PWYRM, PCHOIR) |
| &0C00..&0CFF | the enemy spawner and type speeds |
| &1900..&23FF | NIGHT during play, one overlay at a time otherwise |
| &2400..&28FF | EGFX work area |
| &2900..&29FF | EAudio workspace |
| &2A10..&2AFF | weapon slots A and B |
| &2C00..&2C3F | the EGFX request block, EGBUF |
| &2D00..&2E28 | object arrays: 41 slots each on 64-byte strides |
| &2E80..&2EFF | game state |
| &2F00..&2FFF | NFLOAD, always resident |
| &3000..&7FFF | the screen |

## Licence

Copyright (c) 2026 Richard Beech. The source and the files in `build/` are CC BY-NC-SA 4.0 ([LICENSE](../LICENSE)), except `eaudio.6502`, which is MIT ([LICENSE-EAUDIO](../LICENSE-EAUDIO)), and `build/eg-client.6502`, which is part of EGFX and MIT ([LICENSE-EGFX](../LICENSE-EGFX)).

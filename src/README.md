# Nightfall source

This is the 6502 assembly for Nightfall Beta 1, the build shown on the main menu as `BETA 1 AA0B928`. Assembled with the withheld files listed below, it produces the released `nightfall.ssd` and ROMs byte for byte.

## Assembling

You need [BeebAsm](https://github.com/stardot/beebasm). Run it from the top of this repository, as the paths are relative to there, and each program saves its binary into `build/`:

    beebasm -i src/reset.6502
    beebasm -D CARD=0 -i src/ui.6502
    beebasm -D SLOT=0 -D WEAPON_KIND=1 -i src/weapon-slot.6502

Three parts need files that are not in this repository:

- `src/engine.6502` (NIGHT and NARENA) includes `build/eg-client.6502`, the small stub that calls EGFX. It defines `eg_call`: A is the function number, the parameters go in EGBUF from offset 8, and the call is OSWORD &C0.
- `src/nf-companion.6502` (`NFASSETS.rom`) includes the built sprite packs, music scores and pause screen. The released `NFASSETS.rom` holds the result.
- `src/disc.6502` builds the disc from every program, so it needs both of the above plus the loading picture, SCREEN.

`build/` holds the small generated inputs: the sprite and world frame tables, a few label addresses from the NIGHT and NARENA builds (which is how the overlays assemble without them), `splash.vdu` (the boot screen's mode and palette) and `WPTEST`. Comments that name `tools/` scripts or other `.md` files refer to build scripts and notes kept outside this repository.

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

Copyright (c) 2026 Richard Beech. The source is CC BY-NC-SA 4.0 ([LICENSE](../LICENSE)), except `eaudio.6502`, which is MIT ([LICENSE-EAUDIO](../LICENSE-EAUDIO)).

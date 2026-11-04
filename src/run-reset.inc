; Run reset (entry11): fresh run state, resident helpers and tables. Lives in
; the paged combat overlay NFCOMB beside the boss spawn and world map.
.reset_run
    ; Sword reach limits for the base radius24; every object starts in the
    ; on-screen world-Y band.
    LDA #24: JSR reach_table
    LDA #0: LDX #40
.clear_world_y
    STA OYH,X: DEX: BPL clear_world_y
    ; Level-up XP thresholds follow the squares in resident memory.
    LDX #9
.copy_xp_table
    LDA xp_table,X: STA XP_TABLE,X: DEX: BPL copy_xp_table
    ; Weapon stats and weapon/movement run state: neither weapon owned yet.
    LDX #15
.weapon_base
    LDA weapon_defaults,X: STA SLOT_A_STATS,X: DEX: BPL weapon_base
    LDX #12
.copy_xp_helper
    LDA xp_helper,X: STA XP_THRESHOLD,X: DEX: BPL copy_xp_helper
    LDX #swcol_end-swcol_helper-1
.copy_swcol_helper
    LDA swcol_helper,X: STA SWING_COLOUR_HELPER,X: DEX: BPL copy_swcol_helper
    LDX #17
.copy_tiers
    LDA tier_tables,X: STA TIER_SPAWN,X: DEX: BPL copy_tiers
    LDX #19
.copy_xlat
    LDA kind_xlat,X: STA KIND_XLAT,X
    LDA kind_flags,X: STA KIND_FLAGS,X: DEX: BPL copy_xlat
    LDX #0
.copy_core
    LDA core_image,X: STA ENEMY_CORE,X
    INX: CPX #LO(core_end-ENEMY_CORE): BNE copy_core
    LDX #type_tables_end-type_tables-1
.copy_types
    LDA type_tables,X: STA TYPE_SPEED,X: DEX: BPL copy_types
    ; No phase code yet: KIND_HOOK leaves every kind to the engine (carry
    ; clear) and KIND_DRAW draws nothing.
    LDA #&18: STA KIND_HOOK ; CLC
    LDA #&60: STA KIND_HOOK+1: STA KIND_DRAW ; RTS
    LDA #0: LDX #63
.clear_state
    STA KIND,X: STA DIR,X: STA LIFE,X: DEX: BPL clear_state
    STA FRAME: STA XP: STA IFRAME: STA DEAD
    STA SHRINES_WON: STA PHASE_PAGE
    STA KILLS: STA KILLSH: STA RUNGOLD: STA RUNGOLDH: STA RUNTICKS: STA RUNTICKSH
    STA SPAWNCNT: STA FIRECNT: STA DRAWN
    STA BOSSN: STA FACING: STA SWINGVIS: STA SWINGREST
    STA CHAPTICKS: STA CHAPTERHI: STA SWORDARC: STA SWORDCRIT
    ; Base 90-degree cone: |cross| <= |forward| (tan45), half-width 32.
    LDX #40
.arc_base
    TXA: STA ARCLIM,X: DEX: BPL arc_base
    LDA #32: STA ARCHALF
    LDA #1: STA CHAPTER
    ; Warrior starts with the short-range sweep. The legacy four-way bolt is
    ; intentionally not selected here; it remains a later unlock.
    LDA #1: STA WEAPON
    LDA #24: STA SWORDR
    LDA #&58: STA BOSSLO: LDA #2: STA BOSSHI
    LDA #0: STA BOSSSLOT ; no boss bar until the first boss
    LDA #0: STA QUITRUN: STA MINUTE: STA STATE: STA ARENASLOT ; (STATE 0: the menu)
    LDA #1: STA BOSSDMG ; never 0: a boss touch must not consume the boss
    LDA #2: STA INVALID
    LDA #0: STA TIER
    LDA #1: STA LEVEL: STA KIND
    ; Permanent blessings stay small and avoid level-up card stats: speed,
    ; tempo and damage grow only in-run. UPGS=luck and UPGF=reroll charges
    ; are read by the card overlay; UPGP=ward lengthens post-hit protection.
    ; Twenty small levels each: vigour +1HP, ward +1 tick per two levels,
    ; magnet +1px tall and +1 unit wide per two, greed 12/256 gold per kill.
    LDA UPGV: CLC: ADC #32: STA MAXHP
    LDA MAXHP: STA HP
    LDA #16: STA MSPEED ; eighths of a pixel per tick: 2px on both axes
    LDA #32: STA FIREINT ; tempo: one swing per eight ticks
    LDA #1: STA DMG
    LDA UPGP: LSR A: CLC: ADC #12: STA IFRAMEV
    LDA UPGM: LSR A: CLC: ADC #3: STA MAGRX
    LDA UPGM: CLC: ADC #12: STA MAGRY
    ; A kill's gold: KILL_GOLD/256 (one in five) plus Greed's 10/256 a rank
    ; (20 ranks: 251/256 at most). GOLDACC carries the fractions (chests too).
    LDA UPGG: ASL A: ASL A: ADC UPGG: ASL A: ADC #KILL_GOLD: STA GREEDV
    LDA #0: STA GOLDACC
    LDA #&A5: STA RAND
    LDA #30: STA OX: LDA #112: STA OY
    LDA #90: STA CAMX: LDA #&90: STA CAMY: LDA #1: STA CAMYHI
    LDA #1: LDX #LO(CLOCK): LDY #HI(CLOCK): JSR OSWORD
    LDA CLOCK: STA STAMP
    RTS


; Empty four-byte slot stat blocks, reserved bytes, movement accumulators and
; shared hit scratch. Slot ids and weapon clocks are cleared by the DIR loop.
.weapon_defaults SKIP 16
; By TIER: spawn interval (ticks), life, step mask (1, 3: every 2nd/4th tick).
.tier_tables
EQUB 8,10,12,14,17,20
EQUB 1,1,2,2,3,4
EQUB 3,3,3,2,2,1
; Object kind to EGFX world kind+frame (bits5..6) or point (bit3); storm:
; cyan bolt; ward: holy ring; shrine: white grave; chest: closed art frame2.
.kind_xlat EQUB 0,1,2,3,4,5,4,4+32,4,1,4+64,0,32,64,96,128+6,6+64,6+32,7+32,6+96
; KIND_FLAGS: every kind at the tier's pace (the enemy types, kinds 2 and 3,
; move by TYPE_SPEED instead); the bat and the ghoul flap; the shrine
; (kind15) is drawn from ROM by ENEMY_DRAW.
; The opening roster: the bat (kind2: fast, the least life) from 0:30 and
; the ghoul. (Tier 0 adds no speed.)
.type_tables
EQUB 20,8      ; TYPE_SPEED
EQUB 0,1       ; TYPE_LIFE, added to TIER_LIFE
EQUB 96        ; TYPE_SHARE
EQUB 0,0,0,0,0,0,0,0,0 ; TYPE_ACCY, TYPE_ACCX, TYPE_MOVEY, TYPE_MOVEX, TYPE_COUNT
EQUW 300       ; TYPE_FROM: 30 seconds
EQUB 20,8      ; TYPE_BASE
EQUB 128,13    ; TYPE_DROP: a bat's heal one in two, a ghoul's one in twenty
.type_tables_end
ASSERT type_tables_end-type_tables=TYPE_DROP+2-TYPE_SPEED
.kind_flags EQUB 3,3,3+32,3+32,3,3,3,3,3,3,3,3,3,3,3,3+8,3,3,3,3

.xp_table
INCLUDE "src/xp-normal.inc"
ASSERT P%-xp_table=10
.xp_helper
    LDX LEVEL: CPX #11: BCC xp_helper_index
    LDX #10
.xp_helper_index
    LDA XP_TABLE-1,X: RTS
ASSERT P%-xp_helper=13
; Random byte enters A; latch colour with the same critical decision as damage.
.swcol_helper
    AND #31: CMP SWORDCRIT
    LDA #3: BCS swcol_normal
    ASL SWINGDMG
    LDA #2 ; EGFX colour2 = BBC logical1 (red); colour3 = white.
.swcol_normal
    STA SWINGCOLOUR: RTS
.swcol_end
ASSERT swcol_end-swcol_helper=18
INCLUDE "src/reach-table.inc"

; The enemy core runs at ENEMY_CORE; its image here is what the reset copies.
.core_image
ORG ENEMY_CORE
INCLUDE "src/enemy-core.inc"
ASSERT core_end<=&0D00
COPYBLOCK ENEMY_CORE,core_end,core_image
CLEAR ENEMY_CORE,core_end
ORG core_image+core_end-ENEMY_CORE

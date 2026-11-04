; Shared Nightfall state: resident engine + disk overlay (shared &1900 window; resident loader &2F00).
KILLSH=&2C6E ; kills bits 8..15
STATE=&2EC6: KILLS=&2EC7: RUNTICKS=&2EC8: RUNGOLD=&2EC9
UICHOICE=&2EAC
MAXHP=&2ECA: MSPEED=&2ECB: FIREINT=&2ECC: DMG=&2ECD
MAGRX=&2ECE: MAGRY=&2ECF: GREEDV=&2ED0
GOLDL=&2ED1: GOLDH=&2ED2: GEN=&2ED3
UPGV=&2ED4: UPGS=&2ED5: UPGP=&2ED6: UPGF=&2ED7: UPGM=&2ED8: UPGG=&2ED9
OFFER1=&2EDA: OFFER2=&2EDB: OFFER3=&2EDC
TIER=&2EDD: SPAWNCNT=&2EDE: FIRECNT=&2EDF: DRAWN=&2EE0
DEADWAIT=&2EE1: STR=&2EE2: RUNTICKSH=&2EE3
TMP0=&2EE4: TMP1=&2EE5: TMP2=&2EE6: RUNGOLDH=&2EE7: MAINVEC=&2EE8
SPAWNSLOT=&2EEA
EGBUF=&2C00
HEADER=&2C40
; During a run (no pack load, no saves) &2C40..&2C5F holds the boss bar's EGFX
; PROGRESS_BAR request (or the horde's warning); BOSSSLOT is its slot (0 none).
BOSSBAR=&2C40: BOSSSLOT=&2C69
MINUTE=&2C6C ; run minute: boss odd 1..5, shrine even 2..6, escalation 7+
ARENASLOT=&2C6D ; the shrine being fought (STATE6)
; DIR of a fixed object or boss: a boss's tier (1..), with ELITE set an elite
; (minute 7+: no chest); a shrine's 0, or SHRINE_FINAL for minute 6's, sealed
; until the other two are cleansed and ending the run (the beta's end) when won.
ELITE=&80: SHRINE_FINAL=&40
KILL_GOLD=51 ; 256ths of a gold per kill before Greed (balance-sim.py)
; Tables the run reset installs (run-reset.inc), in EAUDIO's unused bytes:
TIER_SPAWN=&296D: TIER_LIFE=&2973: TIER_STEP=&2979 ; 6 bytes each, by TIER
KIND_XLAT=&29E6 ; 20 bytes: object kind to EGFX world kind+frame (bits5..6)
; 20 bytes by object kind: bits0..1 AND the tier's step mask (3 keeps the
; tier's pace, lower is faster); bit5 shows world frames 0 and 1 in turn.
; bit2 calls KIND_HOOK for that kind each tick; bit3: KIND_DRAW draws it,
; not the world (see enemy-types.md).
KIND_FLAGS=&2E40
KIND_HOOK=&0900 ; &0900..&0AFF: the code the current phase loads
KIND_HOOK_END=&0B00
PHASE_PAGE=&2DFD ; the world's phase page: 0 none, 1 PELITE, 2 PWIZARD
SHRINES_WON=&2C6F ; shrines cleansed this run: picks each arena's boss
KIND_DRAW=KIND_HOOK+3 ; its once-a-frame draw, after the actors
ENEMY_CORE=&0C00 ; &0C00..&0CFF: the enemy core the run reset installs
ENEMY_SPAWN=ENEMY_CORE: ENEMY_DRAW=ENEMY_CORE+3
; The two enemy types (kinds 2 and 3), indexed 0..1 unless noted:
TYPE_SPEED=&2E54 ; eighths of a pixel a tick, the tier's bonus included
TYPE_LIFE=&2E56 ; added to TIER_LIFE at spawn
TYPE_SHARE=&2E58 ; 1 byte: chance in 256 that a spawn is kind 2
TYPE_ACCY=&2E59: TYPE_ACCX=&2E5B ; sub-pixel and sub-unit carries
TYPE_MOVEY=&2E5D: TYPE_MOVEX=&2E5F ; this tick's pixels and X units
TYPE_COUNT=&2E61
TYPE_FROM=&2E62 ; 2 bytes: the run clock (RUNTICKS) from which kind 2 spawns
TYPE_BASE=&2E64 ; the types' own speeds; TYPE_SPEED adds the tier's at each event
TYPE_DROP=&2E66 ; a heal's chance in 256 when one dies, kinds 2 and 3
QUITRUN=&2C6B ; pause Q: the next bank_and_save is a quit (main menu, no summary)
BOSSDMG=&2C6A ; HP a boss's touch costs: a quarter of max HP (at least 1)
META=&2C50
UPGZLO=&76: UPGZHI=&77
LEVEL=&2E82
CRC=&2EBC: EXPECTCRC=&2EBE
; Object pools and simulation variables shared with the UI overlay.
OX=&2D00: OY=&2D40: KIND=&2D80: LIFE=&2E00
HP=&2E80: XP=&2E81: RAND=&2E84: CX=&2E87: CY=&2E88
INVALID=&2EAB
CLOCK=&2EB0: STAMP=&2EB5
FILEHANDLE=&2EB8: LOADCOUNT=&2EB9: OFFSET=&2EBA
REMAIN=&2EC0

FILEBLOCK=&2C70: CRCINDEX=&2EEB
NUM=&2EEC: DIGIT=&2EEE: TEXTX=&2EEF: TEXTY=&2EF0
MENUINDEX=&2EF1: MENUROW=&2EF2: MENULEN=&2EF3: MENURANK=&2EF4
; Chapter run state. These bytes are outside the resident object pools and are
; carried by the run, not reset by level-up choices.
CHAPTER=&2EF5: CHAPTICKS=&2EF6: CHAPTERHI=&2EF7
WEAPON=&2EF8: SWORDR=&2EF9: SWORDARC=&2EFA: SWORDCRIT=&2EFB

; Unused enemy-direction tail (enemies use DIR slots1..10). Reset's clear
; loop zeroes these every run, so the reroll count needs no extra code.
ARCLIM=&2DCB ; DIR slots11..51: sword cone |cross| limit by |forward| 0..40
ARCHALF=&2DF4 ; sword arc half-width in 256-step turns (32 = 45 degrees)
IFRAMEV=&2DFE ; post-hit invulnerability ticks: 12 + 4 per WARD rank
REROLLUSED=&2DFF ; level-up rerolls spent this run (limit UPGF ranks)

; Weapon slot windows stay outside EGFX's &2A00..&2A0E parameter block.
; Shared code occupies only unused coordinate tails beyond real objects0..40.
SLOT_A_CODE=&2A10: SLOT_B_CODE=&2A88 ; two 120-byte windows
MOD_WEAPONS=&2D29 ; dispatcher in unused OX slots41..63 (23 bytes)
WEAPON_HIT_HELPER=&2D69: ORB_STEP=&2D73 ; unused OY slots41..63
SLOTAID=&2DF5: SLOTBID=&2DF6 ; 0 empty; run-only, cleared with DIR
ORB_ID=1: SEED_ID=2: STORM_ID=3: WARD_ID=4: WEAPON_COUNT=4
SLOT_A_STATS=&2E9A: SLOT_B_STATS=&2E9E ; 4 bytes per slot
BOX_NEAR=&2BD6 ; NWEAPB: enemy-in-box test, &2BD6..&2BFF above NF audio
NEARVEC=&2E8D ; 2 bytes: current enemy hit test for damage_enemies
GOLDACC=&2E8F ; Greed's fractional gold, zeroed each run
VOLDOWN=&2E93 ; profile volume steps below full (0 full .. 3 mute), meta byte13
INCLUDE "src/nfassets-map.inc"
; Slot stats: Orb=count/radius/damage/unused; Seeds=rate/damage/life/reach.
; Storm and Ward=rate/damage/half-size/visual-life; unique ids own their clocks.
; Ownership is the slot id, never a stat sentinel. All stats live in page &2E.
STORMACC=&2DFA: STORMNEXT=&2DFB: WARDACC=&2DFC
ORBANG=&2DF7: ORBT=&2DF8: SEEDACC=&2DF9 ; cleared with DIR each run
VACC=&2EA4: HACC=&2EA5
CAMXF=HACC ; world camera's fine X 0..3 pixels (EGFX pixel mode); the arena's own step remainder
WX=&2EA6: WY=&2EA7: WRX=&2EA8: WRY=&2EA9: WIDX=&2EAE: WCNT=&2EAF
; Object slots: slot A 11..14, slot B 15..18; kinds describe weapon art.
STORMKIND=18: WARDKIND=19
ORBKIND=10: TENTKIND=11 ; tentacle kinds 11..14 are its four animation frames
ORBTX=&2DA9 ; KIND slots41..56: orb screen X (4px units) per 1/16 turn
ORBTY=&2E29 ; LIFE slots41..56: orb screen Y per 1/16 turn

CARDVALUE=&2C60: CARDRARITY=&2C63
UPGVALUE=&2EFC: UPGRARITY=&2EFD
UPGSECOND=&2EFE

; Melee state uses the retired projectile pool (slots 25..32).
FACING=&2D19: SWINGDIR=&2D1A: SWINGVIS=&2D1B
RENDERS=&2D15 ;16-bit presented-frame count (retired X slots 21..22)
KEYS=&2D17 ;per-frame movement key bits (retired X slot 23)
DRAWPASS=&2D18 ;render pass 0 (non-kind3) / 1 (kind3, then player) (X slot 24)
SWINGREST=&2D59 ; retired projectile Yslot25: force one clear presentation
SFORWARD=&2D1C: SCROSS=&2D1D: SWINGDMG=&2D1E
SX=&2D1F: SY=&2D20
; SWORDARC is coverage steps0..27: 90+10*steps degrees, 27 = full circle.

; Outside OSFILE block &2C90..&2CA1:
REACHLIM=&2CA2 ;41 bytes: sword |dx| px limit per |dy| 0..40 (reach-table.inc)
OYH=&2CCB ;41 bytes, slots 0..40: object Y bits 8..9 (relative Y is OYH:OY mod 1024)

XP_THRESHOLD=&2C82 ;13-byte reset-installed threshold helper, before loader block
XP_TABLE=&2CF4 ;10-byte threshold table after square table

SWING_COLOUR_HELPER=&2B50 ; reset-installed critical damage/colour latch (18bytes)
SWINGCOLOUR=&2EFF
PLAYERNAME=&2B68 ;24-byte selected profile name, before host audio block

; Boss chest kinds retain their enemy slot and tier in DIR; LIFE is open ticks.
; Fixed world objects in enemy slots, never despawned or replaced, untouched by
; weapons: kinds from SHRINE up. Touching any but the open chest calls entry13.
WORLD_BANK=6 ; the writable sideways bank granted to EGFX's world mode
SHRINE=15: CHEST_CLOSED=16: CHEST_OPEN=17: CHEST_TICKS=50

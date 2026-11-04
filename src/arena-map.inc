; The arena's geometry and its boss page, shared by NARENA and the boss pages.
LEFT=2: RIGHT=58: TOP=24: BOTTOM=212 ; player top-left (X in 4-pixel units)
GUARD_X=30: GUARD_Y=28: START_X=30: START_Y=180
; A boss page (PBELL, PWYRM, PCHOIR), loaded to KIND_HOOK by NARENA on entry:
BOSS_TICK=KIND_HOOK ; JMP: once a tick, after the player, enemies and weapons
BOSS_DRAW=KIND_HOOK+3 ; JMP: once a frame, its attacks (last but the banners)
BOSS_HP=KIND_HOOK+6 ; the boss's life, then life at its second and third phases
BOSS_NAME=KIND_HOOK+9 ; its bar's title: 8 characters and a 0
BOSS_OWNED=16 ; object kinds 16..17 are the page's own: neither the engine
              ; nor the sword moves or hits them in an arena

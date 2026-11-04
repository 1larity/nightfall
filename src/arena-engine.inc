; NARENA: a shrine's arena (entry12, STATE6), run by this engine built with
; ARENA=1, so the sword, its tempo, reach, cone and crits, the orbs, the Seeds
; of Doom and every upgrade work exactly as in play. Differences: the camera
; stays put and the player walks the screen inside a red border; the world
; renders bare (no scenery); the boss and its adds are the only enemies;
; nothing spawns. The frozen world's objects are saved here and restored on
; the way out. Death ends the run (HP stays 0 and NIGHT's death path follows).
; Each shrine's boss is a page (see arena-map.inc) this kernel loads on entry:
; the Bell-Warden, the Wyrm, then the Hollow Choir, by shrines cleansed. The
; kernel keeps the phases (at the page's two life marks: a banner, two bat
; adds and a faster chase); the page attacks and draws its attacks.
REWARD_XP=50: REWARD_GOLD=20

.arena_entry
    ; Save the world's objects (player, enemies 1..10, pickups 33..40: 11..32
    ; are weapons and melee state), then clear the field.
    LDX #40: LDY #18
.a_save
    LDA KIND,X: STA a_kind,Y: LDA OX,X: STA a_ox,Y: LDA OY,X: STA a_oy,Y
    LDA OYH,X: STA a_oyh,Y: LDA LIFE,X: STA a_life,Y
    JSR a_slot_next: BPL a_save
    JSR a_clear
    LDX #a_state_end-a_state-1
.a_state_clear
    STA a_state,X: DEX: BPL a_state_clear ; A = 0
    LDA TIER: STA a_tier: LDA #2: STA TIER ; arena pace: restored on exit
    ; This shrine's boss: its page, its life and its bar's title.
    LDA SHRINES_WON: CMP #2: BCC a_boss_page
    LDA #2
.a_boss_page
    CLC: ADC #2: JSR a_page
    LDA BOSS_HP: STA LIFE+1: STA a_guard_bar+18
    LDX #8
.a_title
    LDA BOSS_NAME,X: STA a_guard_bar+22,X: DEX: BPL a_title
    ; The player (slot0) low on the screen; the boss (slot1), whose touch
    ; costs a quarter of max HP.
    LDA #1: STA KIND: LDA #START_X: STA OX: LDA #START_Y: STA OY
    LDA #9: STA KIND+1: LDA #GUARD_X: STA OX+1: LDA #GUARD_Y: STA OY+1
    LDA MAXHP: LSR A: LSR A: BNE a_bite
    LDA #1
.a_bite
    STA BOSSDMG
    LDA #2: STA INVALID ; repaint both pages: the bare arena
    LDX #10: JSR a_sfx ; the boss's fanfare
    JSR read_clock: LDA CLOCK: STA STAMP
; Play's fixed 100ms ticks (at most four per rendered frame).
.arena_loop
    LDA HP: BNE a_alive
    JMP arena_exit
.a_alive
    LDA a_won: BEQ a_fighting
    JMP arena_victory
.a_fighting
    JSR read_clock
    LDA CLOCK: SEC: SBC STAMP: STA DELTA
    CMP #10: BCS a_tick
    LDA #19: JSR OSBYTE: JMP arena_loop
.a_tick
    CMP #50: BCC a_debt
    LDA CLOCK: SEC: SBC #40: STA STAMP
.a_debt
    LDA #4: STA CATCHUP
    JSR scan_keys
.a_tick_loop
    LDA STAMP: CLC: ADC #10: STA STAMP
    JSR a_simulation
    LDA HP: BEQ a_ticks_done
    LDA a_won: BNE a_ticks_done
    DEC CATCHUP: BEQ a_ticks_done
    LDA CLOCK: SEC: SBC STAMP: CMP #10: BCS a_tick_loop
.a_ticks_done
    JSR render
    JMP arena_loop
.arena_victory
    LDA XP: CLC: ADC #REWARD_XP: BCC a_xp
    LDA #255
.a_xp
    STA XP
    LDA RUNGOLD: CLC: ADC #REWARD_GOLD: STA RUNGOLD
    BCC a_gold
    INC RUNGOLDH
.a_gold
    LDX #3: JSR a_sfx ; fanfare
    ; The message on both pages, then three seconds to read it.
    INC a_show: LDA #0: STA a_ptime
    JSR render: JSR render
    LDA #150: STA a_wait
.a_linger
    LDA #19: JSR OSBYTE ; may change X and Y
    DEC a_wait: BNE a_linger
.arena_exit
    LDA #0: TAY: JSR a_orbit_to ; the orbit tables back on 30,112
    JSR a_clear ; the arena's orbs, seeds and adds go
    LDX #40: LDY #18
.a_restore
    LDA a_kind,Y: STA KIND,X: LDA a_ox,Y: STA OX,X: LDA a_oy,Y: STA OY,X
    LDA a_oyh,Y: STA OYH,X: LDA a_life,Y: STA LIFE,X
    JSR a_slot_next: BPL a_restore
    LDA a_tier: STA TIER
    LDA a_won: BEQ a_lost
    LDX ARENASLOT: LDA #0: STA KIND,X ; the shrine is spent
    INC SHRINES_WON
    ; The page's kinds drew as its own: the shrine and chests' art back.
    LDA #6: STA KIND_XLAT+SHRINE
    LDA #6+64: STA KIND_XLAT+CHEST_CLOSED: LDA #6+32: STA KIND_XLAT+CHEST_OPEN
    ; The world's phase page, if it had one (the boss's overwrote it), or
    ; the run reset's: every kind to the engine, nothing drawn.
    LDA PHASE_PAGE: BEQ a_no_phase
    SEC: SBC #1: JSR a_page
    JMP a_left
.a_no_phase
    LDA #&18: STA KIND_HOOK ; CLC
    LDA #&60: STA KIND_HOOK+1: STA KIND_DRAW ; RTS
    BNE a_left
.a_lost
    LDA #&80: STA DEAD ; NIGHT goes straight to the death state, no world frame
.a_left
    LDA #0: STA IFRAME
    RTS ; the loader restores NIGHT, which repaints the world
.a_clear ; every object slot empty (OYH too: all on the screen); A = 0
    LDX #40: LDA #0
.a_clear_slot
    STA KIND,X: STA OYH,X: DEX: BPL a_clear_slot
    RTS

.a_simulation
    INC FRAME ; (the run clock, RUNTICKS, waits outside: the timeline pauses here)
    LDA IFRAME: BEQ a_vulnerable
    DEC IFRAME
.a_vulnerable
    JSR a_player_update: JSR enemies_update: JSR fire_update
    ; Shift the orbit table to the walking player. Seed modules read OX/OY
    ; directly; neither slot code nor a weapon-specific operand is patched.
    LDA OY: SEC: SBC #112: TAY
    LDA OX: SEC: SBC #30: JSR a_orbit_to
    JSR MOD_WEAPONS
    JMP a_boss
; Shift the orbit tables to offset A (4-pixel units), Y (pixels) from 30,112.
.a_orbit_to
    PHA: SEC: SBC a_dx: STA TMP0
    TYA: SEC: SBC a_dy: STA TMP1
    STY a_dy: PLA: STA a_dx
    LDX #15
.a_orbit
    LDA ORBTX,X: CLC: ADC TMP0: STA ORBTX,X
    LDA ORBTY,X: CLC: ADC TMP1: STA ORBTY,X
    DEX: BPL a_orbit
.a_none
    RTS

; The boss's phases (one tick); its page attacks.
.a_boss
    LDA a_won: BNE a_none
    LDA LIFE+1: LDX #0
    CMP BOSS_HP+1: BCS a_phase_known
    INX: CMP BOSS_HP+2: BCS a_phase_known
    INX
.a_phase_known
    CPX a_phase: BEQ a_attack
    ; A new phase: its banner, a faster chase and two bats from the corners.
    STX a_phase
    LDA a_phase_tier-1,X: STA TIER
    LDA #15: STA a_ptime
    LDX #2: LDA #LEFT: JSR a_add
    LDX #3: LDA #RIGHT
.a_add
    STA OX,X: LDA #TOP: STA OY,X
    LDA #2: STA KIND,X: LDA #4: STA LIFE,X
    RTS
.a_attack
    JMP BOSS_TICK
; The player hurt by A (not while protected after a hit), with its sound.
.a_hurt_by
    LDX IFRAME: BNE a_hurt_done
    STA TMP0: LDA HP: SEC: SBC TMP0: BCS a_hurt
    LDA #0
.a_hurt
    STA HP
    LDA IFRAMEV: STA IFRAME
    LDX #2: JMP a_sfx ; hurt
.a_hurt_done
    RTS
; Load page A (0 PELITE, 1 PWIZARD, 2 PBELL, 3 PWYRM, 4 PCHOIR) to KIND_HOOK.
.a_page
    TAY
    LDA #0: LDX #17
.a_page_clear
    STA FILEBLOCK,X: DEX: BPL a_page_clear
    LDA a_page_names,Y: STA FILEBLOCK: LDA #HI(a_names): STA FILEBLOCK+1
    LDA #HI(KIND_HOOK): STA FILEBLOCK+3 ; exec-low 0: load at KIND_HOOK
    LDA #&FF: LDX #LO(FILEBLOCK): LDY #HI(FILEBLOCK): JMP &FFDD
.a_page_names
    EQUB LO(a_names),LO(a_name_wizard),LO(a_name_bell),LO(a_name_wyrm),LO(a_name_choir)
.a_names EQUS "PELITE",13
.a_name_wizard EQUS "PWIZARD",13
.a_name_bell EQUS "PBELL",13
.a_name_wyrm EQUS "PWYRM",13
.a_name_choir EQUS "PCHOIR",13
ASSERT HI(a_names)=HI(a_name_choir)

; Play's movement (MSPEED eighths of a pixel per tick, 4-pixel X steps),
; moving the player inside the border instead of the camera.
.a_player_update
    LDA VACC: CLC: ADC MSPEED: TAY: AND #7: STA VACC
    TYA: LSR A: LSR A: LSR A: STA TMP0
    CLC: ADC HACC: TAY: AND #3: STA HACC
    TYA: LSR A: LSR A: STA TMP1
    LDA KEYS: AND #3: CMP #1: BNE a_down
    LDA #3: STA FACING
    LDA OY: SEC: SBC TMP0: BCC a_top
    CMP #TOP: BCS a_y
.a_top
    LDA #TOP: BNE a_y
.a_down
    CMP #2: BNE a_across
    LDA #1: STA FACING
    LDA OY: CLC: ADC TMP0: CMP #BOTTOM: BCC a_y
    LDA #BOTTOM
.a_y
    STA OY
.a_across
    LDA TMP1: BEQ a_moved
    LDA KEYS: AND #12: CMP #4: BNE a_right
    LDA #2: STA FACING
    LDA OX: CMP #LEFT+1: BCC a_moved
    DEC OX: RTS
.a_right
    CMP #8: BNE a_moved
    LDA #0: STA FACING
    LDA OX: CMP #RIGHT: BCS a_moved
    INC OX
.a_moved
    RTS

; Screen X (A low, Y high) of the player's left edge plus signed offset A.
.a_screen_x
    STA TMP2
    LDA #0: STA TMP1
    LDA OX: ASL A: ROL TMP1: ASL A: ROL TMP1
    CLC: ADC TMP2: TAX
    LDA #0: BIT TMP2: BPL a_sx_high
    LDA #&FF
.a_sx_high
    ADC TMP1: TAY: TXA
    RTS

; The red border, every frame (scenery-free restoration may clip it).
.a_frame
    LDX #a_border_end-a_border-1
.a_border_copy
    LDA a_border,X: STA EGBUF+8,X: DEX: BPL a_border_copy
    LDA #24: JMP eg_call
; POLYGON24 outline: 4 vertices, colour1 (red), signed X/Y at offset16.
.a_border EQUB 4,0,1,0,0,0,0,0
    EQUW 2,16, 253,16, 253,229, 2,229 ; (the guardian's bar is in the HUD)
.a_border_end

; Last in a frame: the boss's attacks, then a phase banner or the victory
; message.
.a_banner
    JSR BOSS_DRAW
    LDA a_ptime: BEQ a_victory_text
    ; A phase banner for a moment; when it ends, repaint both pages to clear it.
    DEC a_ptime: BNE a_phase_text
    LDA #2: STA INVALID
.a_phase_text
    LDX a_phase: LDY a_phase_texts-1,X
    LDA a_phase_x-1,X: LDX #104: JMP a_line
.a_victory_text
    LDA a_show: BEQ a_banner_done
    LDY #0: LDA #40: LDX #104: JSR a_line
    LDY #a_prize-a_won_text: LDA #64: LDX #128
; BANNER28 at X=A, Y=X (both 8-aligned) for the text at a_won_text+Y.
.a_line
    STA EGBUF+8: STX EGBUF+10
    LDA #0: STA EGBUF+9: STA EGBUF+12
    LDX #0
.a_line_copy
    LDA a_won_text,Y: STA EGBUF+22,X: BEQ a_line_send
    INY: INX: BNE a_line_copy
.a_line_send
    STX EGBUF+11 ; width in characters
    LDA #28: JMP eg_call
.a_banner_done
    RTS
.a_won_text EQUS "THE SHRINE IS CLEANSED",0
.a_prize EQUS "50 XP   20 GOLD",0
.a_phase2 EQUS "PHASE II",0
.a_phase3 EQUS "PHASE III",0

; EAUDIO effect X (the reply overwrites the command byte: set it each time).
.a_sfx
    STX a_eaud+8: LDA #3: STA a_eaud+6
    LDA #&C0: LDX #LO(a_eaud): LDY #HI(a_eaud): JMP &FFF1
.a_eaud EQUB 64,16: EQUS "EAUD": EQUB 3,0,0,0,0,0,0,0,0,0

; EGFX PROGRESS_BAR27 for the boss (as NFCOMB's boss bar), max and title from
; its page; render stores its life at +16 each frame.
; (a_guard_bar and the small tables are in the header's unused space)

.read_clock
    LDA #1: LDX #LO(CLOCK): LDY #HI(CLOCK): JMP &FFF1
.a_wait EQUB 0: .a_tier EQUB 0
.a_state ; cleared on entry
.a_won EQUB 0: .a_show EQUB 0: .a_phase EQUB 0
.a_ptime EQUB 0: .a_dx EQUB 0: .a_dy EQUB 0
.a_state_end
.a_kind SKIP 19: .a_ox SKIP 19: .a_oy SKIP 19: .a_oyh SKIP 19: .a_life SKIP 19

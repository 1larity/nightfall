; Cold boss service, entry13 with SPAWNSLOT1..10 (0 is the timed boss spawn).
; Runs only on physical contact. No reward is ever allocated from the pickup
; pool, so a full pool cannot lose a boss reward. Registers may be clobbered.
.open_chest
    LDA KIND,X: BEQ shrine_after
    CMP #SHRINE: BNE chest_touch
    JMP shrine_touch
.shrine_after
    ; Back from a won arena (the engine calls again): the shrine is spent. The
    ; final shrine ends the run: HP 0 takes the death path (bank, save, the
    ; summary, which thanks the player), DEAD negative skipping a world frame.
    LDA DIR,X: CMP #SHRINE_FINAL: BNE shrine_after_done
    LDA #0: STA HP
    LDA #&80: STA DEAD
.shrine_after_done
    RTS
.chest_touch
    ; Only a closed chest is touched here (the engine skips open ones).
    LDA #CHEST_OPEN: STA KIND,X
    TXA: PHA: LDX #9: JSR eaud_sfx: PLA: TAX ; chest sparkle
    LDA #CHEST_TICKS: STA LIFE,X
    LDA DIR,X: STA TMP0: STA TMP1
.chest_gold
    ; Existing Greed fractional-gold rule, once per boss tier.
    LDA GOLDACC: CLC: ADC GREEDV: STA GOLDACC
    LDA #1: ADC RUNGOLD: STA RUNGOLD
    LDA RUNGOLDH: ADC #0: STA RUNGOLDH
    DEC TMP1: BNE chest_gold
    LDA TMP0: ASL A: ASL A: CLC: ADC TMP0: CLC: ADC #10
    CLC: ADC XP: BCC chest_xp_store
    LDA #255
.chest_xp_store
    STA XP
    LDA TMP0: AND #1: BNE chest_magnet
    LDA HP: CLC: ADC #16: BCS chest_heal_full
    CMP MAXHP: BCC chest_heal_store
.chest_heal_full
    LDA MAXHP
.chest_heal_store
    STA HP
.chest_reward_done
    RTS
.chest_magnet
    LDX #33
.chest_vacuum
    LDA KIND,X: CMP #5: BNE chest_vacuum_next
    LDA LIFE,X: CLC: ADC XP: BCC chest_vacuum_store
    LDA #255
.chest_vacuum_store
    STA XP
    LDA #0: STA KIND,X
.chest_vacuum_next
    INX: CPX #41: BNE chest_vacuum
    RTS

; Touching a shrine: enter its arena (no way back until the guardian falls)
; or step away. Entering sets STATE6; the engine runs NARENA next.
.shrine_touch
    STX ARENASLOT
    ; The final shrine is sealed while another shrine stands: its lines 4..7.
    LDY #0
    LDA DIR,X: CMP #SHRINE_FINAL: BNE shrine_lines
    LDX #10
.shrine_seal
    LDA KIND,X: CMP #SHRINE: BNE shrine_seal_next
    LDA DIR,X: BNE shrine_seal_next
    LDY #4
.shrine_seal_next
    DEX: BNE shrine_seal
.shrine_lines
    STY shrine_set
    JSR menu_background
    LDX shrine_set
.shrine_line
    STX HELPITEM
    LDA shrine_lo,X: STA &F8: LDA shrine_hi,X: STA &F9
    LDA shrine_x,X: PHA: LDA shrine_y,X: TAX: PLA: JSR htext
    LDX HELPITEM: INX: TXA: AND #3: BNE shrine_line
    LDA #6: JSR help_gfx
    JSR key_flush
.shrine_poll
    LDA #19: JSR &FFF4
    LDA #129: LDX #0: LDY #0: JSR &FFF4: BCS shrine_poll
    JSR menu_click
    TXA: AND #&DF
    CMP #'N': BEQ shrine_back
    CMP #'Y': BNE shrine_poll
    LDY shrine_set: BEQ shrine_enter ; (sealed: N only)
    BNE shrine_poll
.shrine_back
    ; Step back: nudge the shrine aside so it does not ask again at once.
    LDX ARENASLOT: LDA OX,X: CLC: ADC #6: STA OX,X
    BNE shrine_resume
.shrine_enter
    LDA #6: STA STATE
.shrine_resume
    ; The prompt covered the world and took time: repaint, rebase the clock.
    LDA #2: STA INVALID
    LDA #1: LDX #LO(CLOCK): LDY #HI(CLOCK): JSR &FFF1
    LDA CLOCK: STA STAMP
    RTS
.shrine_set EQUB 0 ; 0 the challenge, 4 sealed
.shrine_x EQUB 64,64,80,96, 96,104,72,136 ; each line centred on x=160
.shrine_y EQUB 64,104,128,176, 64,104,128,176
.shrine_lo EQUB LO(shrine_t1),LO(shrine_t2),LO(shrine_t3),LO(shrine_t4)
EQUB LO(shrine_s1),LO(shrine_s2),LO(shrine_s3),LO(shrine_s4)
.shrine_hi EQUB HI(shrine_t1),HI(shrine_t2),HI(shrine_t3),HI(shrine_t4)
EQUB HI(shrine_s1),HI(shrine_s2),HI(shrine_s3),HI(shrine_s4)
.shrine_t1 EQUS "A SHRINE OF THE OLD GOD",0
.shrine_t2 EQUS "A GUARDIAN WAITS WITHIN.",0
.shrine_t3 EQUS "THERE IS NO ESCAPE.",0
.shrine_t4 EQUS "ENTER?  ",KEY,"Y",KEY,"ES   ",KEY,"N",KEY,"O",0
.shrine_s1 EQUS "THE LAST SHRINE",0
.shrine_s2 EQUS "IT IS SEALED.",0
.shrine_s3 EQUS "CLEANSE THE OTHER TWO.",0
.shrine_s4 EQUS KEY,"N",KEY," BACK",0

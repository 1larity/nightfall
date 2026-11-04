; The enemy core, assembled for ENEMY_CORE (&0C00) and copied there by the
; run reset: the spawner and the enemy types' speeds. Object kinds 2 and 3
; are the two enemy types the roster has on screen; the TYPE_ tables (&2E54)
; describe them. NIGHT calls ENEMY_SPAWN once a tick, before it moves the
; enemies, and ENEMY_DRAW once a frame, after the world's actors.
    JMP spawn ; ENEMY_SPAWN
; ENEMY_DRAW: each shrine on screen from the 16x16 ROM pack (the world skips
; it: KIND_FLAGS bit3), at a 4-pixel X.
    LDX #10
.shrine_draw
    LDA KIND,X: CMP #SHRINE: BNE shrine_draw_next
    LDA OYH,X: BNE shrine_draw_next
    LDA OX,X: CMP #61: BCS shrine_draw_next
    LDA OY,X: CMP #225: BCS shrine_draw_next
    STA EGBUF+10
    LDA OX,X: ASL A: ASL A: STA EGBUF+8
    TXA: PHA
    LDA #SPR_SHRINE: JSR NIGHT_ROM_SPRITE
    PLA: TAX
.shrine_draw_next
    DEX: BNE shrine_draw
    RTS
.spawn
    ; This tick's moves for each type: TYPE_SPEED (its own and the tier's,
    ; set at each event) in eighths of a pixel, carried in TYPE_ACCY; X moves
    ; in 4-pixel units carried in TYPE_ACCX.
    LDX #1
.type_speed
    LDA TYPE_SPEED,X: CLC: ADC TYPE_ACCY,X: PHA
    AND #7: STA TYPE_ACCY,X
    PLA: LSR A: LSR A: LSR A: STA TYPE_MOVEY,X
    CLC: ADC TYPE_ACCX,X: PHA
    AND #3: STA TYPE_ACCX,X
    PLA: LSR A: LSR A: STA TYPE_MOVEX,X
    DEX: BPL type_speed
    ; A spawn when SPAWNCNT runs out, every TIER_SPAWN+1 ticks.
    DEC SPAWNCNT: BPL spawn_done
    LDX TIER: LDA TIER_SPAWN,X: STA SPAWNCNT
    LDX #1
.find_enemy
    LDA KIND,X: BEQ spawn_here
    INX
    CPX #11
    BNE find_enemy
.spawn_done
    RTS
.spawn_here
    LDA #0: STA DIR,X ; a reused boss slot must become an ordinary enemy
    STA OYH,X
    ; Kind 2 with chance TYPE_SHARE in 256, from TYPE_FROM on the run clock
    ; and only while no other kind 2 is alive (the fast bat mobs); else kind
    ; 3. (Carry set: kind 3.)
    JSR NIGHT_RANDOM: CMP TYPE_SHARE: BCS spawn_kind
    LDA TYPE_FROM: CMP RUNTICKS: LDA TYPE_FROM+1: SBC RUNTICKSH: BCS spawn_kind
    LDY #10
.spawn_solo
    LDA KIND,Y: CMP #2: BEQ spawn_kind
    DEY: BPL spawn_solo ; (slot 0, the player's kind 1, leaves carry clear)
.spawn_kind
    LDA #2: ADC #0: STA KIND,X: TAY
    LDA TYPE_LIFE-2,Y: LDY TIER: CLC: ADC TIER_LIFE,Y: STA LIFE,X
    ; Half the enemies come from the edge the player faces (FACING: 0 right,
    ; 1 down, 2 left, 3 up), so a player who keeps running meets them; the
    ; rest from the top or bottom edge.
    JSR NIGHT_RANDOM: LSR A: BCS spawn_ahead
    AND #2: ORA #1 ; 1 (bottom) or 3 (top)
    BNE spawn_edge
.spawn_ahead
    LDA FACING
.spawn_edge
    LSR A: TAY: BCC spawn_side ; Y: 0 bottom or right, 1 top or left
    ; Top or bottom: a random column.
    JSR NIGHT_RANDOM: AND #63: CMP #48: BCC spawn_x
    SBC #40
.spawn_x
    ADC #6: STA OX,X ; 6..53 of the 64 columns
    LDA #216: DEY: BNE spawn_y
    LDA #8
.spawn_y
    STA OY,X: RTS
.spawn_side
    ; Left or right: a random height, 40..168.
    JSR NIGHT_RANDOM: AND #127: ADC #40: STA OY,X
    LDA #58: DEY: BNE spawn_column ; the right edge
    LDA #0
.spawn_column
    STA OX,X: RTS
.core_end

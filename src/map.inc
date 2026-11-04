; World map (NFCOMB entry4, M in play): XP, heal/magnet pickups and bosses at
; their world positions on a 1/8-scale view of the 1024x1024 torus, centred on
; the player. Uses the help screen's EGFX transport and pause-style frame.
MAPX=96: MAPY=56 ; top-left of the 128x128 map square
.map_screen
    JSR menu_background
    LDA #LO(map_title): STA &F8: LDA #HI(map_title): STA &F9
    LDA #120: LDX #32: JSR help_text
    LDA #0: STA EGBUF+9: STA EGBUF+11: STA EGBUF+17
    LDX #0
.map_line
    STX HELPITEM
    LDA map_lines,X: STA EGBUF+8: LDA map_lines+1,X: STA EGBUF+10
    LDA map_lines+2,X: STA EGBUF+16: LDA map_lines+3,X: STA EGBUF+20
    LDA map_lines+4,X: STA EGBUF+13
    LDA #10: JSR help_gfx
    LDA HELPITEM: CLC: ADC #5: TAX
    CPX #map_lines_end-map_lines: BNE map_line
    ; Player at the centre: a white ring.
    LDA #MAPX+64: STA EGBUF+8: LDA #MAPY+64: STA EGBUF+10
    LDA #3: STA EGBUF+13: LDA #0: STA EGBUF+14: LDA #3: STA EGBUF+15
    LDA #22: JSR help_gfx
    ; Bosses (enemy slots1..10) and XP/pickups (slots33..40).
    LDX #1
.map_object
    STX HELPITEM
    LDA KIND,X: CMP #9: BEQ map_place
    CMP #SHRINE: BEQ map_place
    CPX #33: BCC map_next
    CMP #5: BCC map_next
    CMP #8: BCS map_next
.map_place
    ; X: (OX-30)/2 wraps the 256-unit ring onto 128 pixels.
    LDA OX,X: SEC: SBC #30: LSR A: CLC: ADC #64: AND #127
    CLC: ADC #MAPX: STA EGBUF+8
    ; Y: (OYH:OY - 112) mod 1024, /8 onto 128 pixels.
    LDA OY,X: SEC: SBC #112: STA TMP0
    LDA OYH,X: SBC #0: AND #3
    ASL A: ASL A: ASL A: ASL A: ASL A: STA TMP1
    LDA TMP0: LSR A: LSR A: LSR A: ORA TMP1
    CLC: ADC #64: AND #127: CLC: ADC #MAPY: STA EGBUF+10
    LDA #0: STA EGBUF+9: STA EGBUF+11
    LDY LIFE,X: LDA KIND,X: JSR map_marker
.map_next
    LDX HELPITEM: INX: CPX #41: BNE map_object
    ; Legend: one marker of each kind beside its label.
    LDX #0
.map_legend
    STX HELPITEM
    LDA map_key_x,X: STA EGBUF+8: LDA #204: STA EGBUF+10
    LDA #0: STA EGBUF+9: STA EGBUF+11
    LDY #1: LDA map_key_kind,X: JSR map_marker
    LDX HELPITEM
    LDA map_key_lo,X: STA &F8: LDA map_key_hi,X: STA &F9
    LDA map_key_x,X: CLC: ADC #6: AND #&F8: CLC: ADC #8: LDX #200: JSR help_text
    LDX HELPITEM: INX: CPX #5: BNE map_legend
    LDA #LO(map_keys): STA &F8: LDA #HI(map_keys): STA &F9
    LDA #72: LDX #216: JSR help_text
    LDA #6: JSR help_gfx
    JSR key_flush
.map_poll
    LDA #19: JSR &FFF4
    LDA #129: LDX #0: LDY #0: JSR &FFF4: BCS map_poll
    JSR menu_click
    TXA: AND #&DF ; letters to upper case
    CMP #77: BEQ map_return
    CPX #32: BEQ map_return
    CPX #27: BNE map_poll
.map_return
    RTS
; A = kind (5 XP, 6 heal, 7 magnet, 9 boss, 15 shrine), Y = XP value,
; EGBUF+8/10 centre.
.map_marker
    CMP #9: BEQ map_boss
    CMP #SHRINE: BEQ map_shrine
    CMP #5: BEQ map_xp
    SEC: SBC #6: STA EGBUF+12 ; SPR_HEAL, SPR_MAGNET
    LDA EGBUF+8: SEC: SBC #2: STA EGBUF+8
    LDA EGBUF+10: SEC: SBC #2: STA EGBUF+10
    LDA #0: STA EGBUF+14
    LDA #SWORD_BANK: STA EGBUF+16: LDA #NFSPR4_PAGE: STA EGBUF+17
    LDA #37: JMP help_gfx
.map_xp
    ; Merged XP (value above 10) is white, as in play.
    LDA #2: CPY #11: BCC map_dot
    LDA #3
.map_dot
    STA EGBUF+13: LDA #9: JMP help_gfx
.map_boss
    LDA #1: STA EGBUF+13: STA EGBUF+14: LDA #3: STA EGBUF+15
    LDA #22: JMP help_gfx
.map_shrine
    ; A white ring, bigger than a boss.
    LDA #3: STA EGBUF+13: LDA #0: STA EGBUF+14: LDA #4: STA EGBUF+15
    LDA #22: JMP help_gfx
; Lines: x0,y0,x1,y1,colour. Map outline (white), then the current view
; (cyan, the 320x240 screen at 1/8 around the player).
.map_lines
EQUB MAPX,MAPY,MAPX+126,MAPY,3
EQUB MAPX+126,MAPY,MAPX+126,MAPY+127,3
EQUB MAPX+126,MAPY+127,MAPX,MAPY+127,3
EQUB MAPX,MAPY+127,MAPX,MAPY,3
EQUB MAPX+48,MAPY+49,MAPX+80,MAPY+49,2
EQUB MAPX+80,MAPY+49,MAPX+80,MAPY+79,2
EQUB MAPX+80,MAPY+79,MAPX+48,MAPY+79,2
EQUB MAPX+48,MAPY+79,MAPX+48,MAPY+49,2
.map_lines_end
.map_key_x EQUB 32,72,128,176,232
.map_key_kind EQUB 5,6,7,9,SHRINE
.map_key_lo EQUB LO(map_k_xp),LO(map_k_heal),LO(map_k_mag),LO(map_k_boss),LO(map_k_shrine)
.map_key_hi EQUB HI(map_k_xp),HI(map_k_heal),HI(map_k_mag),HI(map_k_boss),HI(map_k_shrine)
.map_title EQUS "MAP",0
.map_k_xp EQUS "XP",0
.map_k_heal EQUS "HEAL",0
.map_k_mag EQUS "MAG",0
.map_k_boss EQUS "BOSS",0
.map_k_shrine EQUS "SHRINE",0
.map_keys EQUS KEY,"M",KEY," OR ",KEY,"SPACE",KEY,": RETURN",0

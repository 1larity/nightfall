; Text with highlighted keys: KEY bytes in a string toggle between the font's
; white and KEYCOL (EGFX TEXT request version2, offset20 foreground colour).
; A = x, X = y, (&F8) = string. Keys are written "P RESUME" as KEY,"P",KEY,...
; The string may be EGBUF+22 itself: segments are compacted forwards.
; &F8 is kept here across the OSWORDs: a Tube client's OSWORD leaves its own
; block pointer in &F8/&F9 (each segment then read the request: "G").
KEY=1: KEYCOL=1 ; red
.htext
    STA ht_x: LDA #0: STA ht_x+1: STA ht_colour: STA ht_index
    STX ht_y
    LDA &F8: STA ht_ptr: LDA &F9: STA ht_ptr+1
.ht_segment
    LDA ht_ptr: STA &F8: LDA ht_ptr+1: STA &F9
    LDY ht_index: LDX #0
.ht_copy
    LDA (&F8),Y: BEQ ht_draw
    CMP #KEY: BEQ ht_draw
    STA EGBUF+22,X: INX: INY: BNE ht_copy
.ht_draw
    STY ht_index: PHA
    LDA #0: STA EGBUF+22,X
    TXA: BEQ ht_next
    PHA
    LDA #64: STA EGBUF: LDA #8: STA EGBUF+1
    LDA #'E': STA EGBUF+2: LDA #'G': STA EGBUF+3
    LDA #2: STA EGBUF+4: LDA #7: STA EGBUF+5: LDA #0: STA EGBUF+7
    LDA ht_x: STA EGBUF+8: LDA ht_x+1: STA EGBUF+9
    LDA ht_y: STA EGBUF+10: LDA #0: STA EGBUF+11: STA EGBUF+21
    LDA ht_colour: STA EGBUF+20
    LDA #&C0: LDX #LO(EGBUF): LDY #HI(EGBUF): JSR &FFF1
    PLA: ASL A: ASL A: ASL A ; segment width (8-pixel cells)
    CLC: ADC ht_x: STA ht_x: BCC ht_next
    INC ht_x+1
.ht_next
    PLA: BEQ ht_done ; end of string
    INC ht_index ; skip the KEY byte and swap colour
    LDA ht_colour: EOR #KEYCOL: STA ht_colour
    JMP ht_segment
.ht_done
    RTS
.ht_x EQUW 0
.ht_y EQUB 0
.ht_colour EQUB 0
.ht_index EQUB 0
.ht_ptr EQUW 0

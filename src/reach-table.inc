; A = sword reach R (pixels, <=40) -> REACHLIM[y] = floor(sqrt(R*R-y*y)) for
; y = 0..40 (0 beyond R). Walks the circle edge with E = R*R-x*x-y*y, so no
; multiply: x steps in while E < 0 (E += 2x-1), then the next row costs
; E -= 2y+1. Clobbers A/X/Y and TMP0..TMP2.
.reach_table
    STA TMP2 ; x
    LDA #0: STA TMP0: STA TMP1 ; E (16-bit, signed)
    TAY
.reach_edge
    LDA TMP1: BPL reach_store
    LDA TMP2: BEQ reach_store ; beyond R: no reach on this row
    ASL A: SEC: SBC #1: CLC: ADC TMP0: STA TMP0 ; E += 2x-1
    BCC reach_step
    INC TMP1
.reach_step
    DEC TMP2: JMP reach_edge
.reach_store
    LDA TMP2: STA REACHLIM,Y
    TYA: ASL A: EOR #255: CLC: ADC TMP0: STA TMP0 ; E -= 2y+1
    LDA TMP1: ADC #&FF: STA TMP1
    INY: CPY #41: BNE reach_edge
    RTS

; EAUDIO requests from overlays through the generic EAUD OSWORD (unclaimed,
; and harmless, without EAUDIO). Menu effects use the noise channel, so they
; never disturb menu music on the three tone channels.
.eaud_sfx ; X = effect ID
    STX eaud_block+8: LDX #3
.eaud_cmd ; X = command, payload at eaud_block+8..; keeps A and its flags
    PHA
    STX eaud_block+6 ; each reply overwrites the command byte
    LDA #&C0: LDX #LO(eaud_block): LDY #HI(eaud_block): JSR &FFF1
    PLA: RTS
.menu_click ; key feedback (SFX8); keeps A (and its flags) and X
    PHA: TXA: PHA
    LDX #8: JSR eaud_sfx
    PLA: TAX: PLA
    RTS
; Menu music needs no ticking: EAUDIO refills it from the vsync event.
.key_flush ; drop keys typed during play (held WASD auto-repeat): a window
    ; opened mid-run must not read them as presses, nor tick for them
    LDA #21: LDX #0: JMP &FFF4
.eaud_block EQUB 64,16: EQUS "EAUD": EQUB 0,0,0,0,0,0,0,0

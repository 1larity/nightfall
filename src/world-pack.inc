; Open EGFX world mode and stage every kind frame from the companion ROM's
; sprite packs (WORLD_INIT, then one WORLD_FRAME_ROM44 per frame): at boot,
; and again when a shrine's arena hands the screen back to the world.
; Needs gfxcall (EG function A on EGBUF). EGFX trusts the table: the build
; lints it (tools/sprite_lint.py), so nothing is checked here.
INCLUDE "build/world-frames.inc"
.world_pack
    LDX #15
.pack_dimensions
    LDA world_dims,X: STA EGBUF+16,X
    DEX: BPL pack_dimensions
    LDA #20: STA EGBUF+12: LDA #255: STA EGBUF+13 ; 20 scenery dots
    LDA #0: STA EGBUF+14: LDA #3: STA EGBUF+15 ; no patterns; pixel mode, ring
    LDA #WORLD_BANK: STA EGBUF+8: LDA #&A7: STA EGBUF+9
    LDA #3: STA EGBUF+10: LDA #16: STA EGBUF+11
    LDA #11: JSR gfxcall
    LDA #0: STA LOADCOUNT
.pack_frame
    ; Record: kind, frame, page, source frame, first row; the bank is ours.
    LDX LOADCOUNT
    LDA world_frames,X: STA EGBUF+8
    LDA world_frames+1,X: STA EGBUF+9
    LDA #SWORD_BANK: STA EGBUF+10
    LDA world_frames+2,X: STA EGBUF+11
    LDA world_frames+3,X: STA EGBUF+12
    LDA world_frames+4,X: STA EGBUF+13
    LDA #44: JSR gfxcall
    LDA LOADCOUNT: CLC: ADC #5: STA LOADCOUNT
    CMP #WORLD_FRAMES*5: BNE pack_frame
    RTS

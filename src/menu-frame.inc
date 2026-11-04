; SPDX-License-Identifier: CC-BY-NC-SA-4.0
; MENU_GFX is this overlay's existing EGFX transport: it stamps the EG/v1
; header and BEGIN selects the hidden page. Reuse NFASSETS &C1/16 there.
; No new RAM, artwork, ROM service or gameplay-render dependency.
.menu_background
    LDA #1: JSR MENU_GFX
.draw_pause_art
    LDA #16: STA EGBUF+5
    LDA #0: STA EGBUF+7
    LDA #&C1: LDX #LO(EGBUF): LDY #HI(EGBUF): JMP &FFF1

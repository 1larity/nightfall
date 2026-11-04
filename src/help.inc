; Paged help entry14 shares the cold combat overlay, never the engine window.
INCLUDE "build/sword-bank.inc"
INCLUDE "build/sprite-map.inc"
HELPPAGE=&2C66: HELPITEM=&2C67: HELPY=&2C68
.help_screen
    LDA #0: STA HELPPAGE
.help_redraw
    JSR menu_background
    LDA #LO(help_title): STA &F8: LDA #HI(help_title): STA &F9
    LDA #64: LDX #48: JSR help_text
    LDA #0: STA HELPITEM
.help_item
    LDA HELPITEM: CLC: ADC HELPPAGE: TAX
    LDA help_ids,X: CMP #252: BNE help_have_item
    JMP help_next
.help_have_item
    STA EGBUF+12
    LDA #40: STA EGBUF+8
    LDA HELPITEM: ASL A: CLC: ADC HELPITEM: ASL A: ASL A: ASL A
    CLC: ADC #72: STA HELPY: STA EGBUF+10
    LDA #0: STA EGBUF+9: STA EGBUF+11: STA EGBUF+14
    LDA #3: STA EGBUF+13
    LDA #SWORD_BANK: STA EGBUF+16
    LDA #NFSPR16_PAGE: STA EGBUF+17
    LDA EGBUF+12: CMP #250: BCC help_sprite
    CMP #252: BCS help_large_id
    SEC: SBC #250: STA EGBUF+12 ; SPR_HEAL, SPR_MAGNET
    LDA #NFSPR4_PAGE: STA EGBUF+17
    LDA #37: JSR help_gfx
    JMP help_label
.help_large_id
    CMP #253: BCC help_sprite
    BEQ help_sword
    CMP #254: BNE help_point
    LDA #2: STA EGBUF+13
.help_point
    LDA #9: JSR help_gfx
    JMP help_label
.help_sword
    LDA #SPR_SWORD: STA EGBUF+12
.help_sprite
    LDA #35: JSR help_gfx
.help_label
    LDA HELPITEM: CLC: ADC HELPPAGE: TAX
    LDA help_lo,X: STA &F8: LDA help_hi,X: STA &F9
    LDA #80: LDX HELPY: JSR help_text
.help_next
    INC HELPITEM: LDA HELPITEM: CMP #6: BCS help_footer
    JMP help_item
.help_footer
    LDA #LO(help_keys): STA &F8: LDA #HI(help_keys): STA &F9
    LDA #96: LDX #208: JSR help_text ; 16 cells centred on x=160
    LDA #6: JSR help_gfx
    JSR key_flush
.help_poll
    LDA #19: JSR &FFF4
    LDA #129: LDX #0: LDY #0: JSR &FFF4: BCS help_poll
    JSR menu_click
    CPX #97: BCC help_upper
    CPX #123: BCS help_upper
    TXA: AND #&DF: TAX
.help_upper
    CPX #77: BEQ help_return
    CPX #32: BEQ help_return
    CPX #78: BEQ help_page
    CPX #80: BNE help_poll
.help_page
    LDA HELPPAGE: EOR #6: STA HELPPAGE
    JMP help_redraw
.help_return
    RTS
.help_text ; A = x, X = y, (&F8) string with KEY-highlighted keys
    JMP htext
.help_gfx
    STA EGBUF+5
    LDA #64: STA EGBUF: LDA #8: STA EGBUF+1
    LDA #'E': STA EGBUF+2: LDA #'G': STA EGBUF+3
    LDA #1: STA EGBUF+4
    LDA #&C0: LDX #LO(EGBUF): LDY #HI(EGBUF): JMP &FFF1
.help_ids EQUB SPR_KNIGHT+2,SPR_BAT,SPR_GHOUL,SPR_BOSS,250,251,254,255,SPR_GRAVE,SPR_ROCK,253,252
.help_lo EQUB LO(h_player),LO(h_bat),LO(h_ghoul),LO(h_boss),LO(h_heal),LO(h_mag),LO(h_xp),LO(h_big),LO(h_grave),LO(h_rock),LO(h_sword),0
.help_hi EQUB HI(h_player),HI(h_bat),HI(h_ghoul),HI(h_boss),HI(h_heal),HI(h_mag),HI(h_xp),HI(h_big),HI(h_grave),HI(h_rock),HI(h_sword),0
.help_title EQUS "NIGHTFALL FIELD GUIDE",0
.help_keys EQUS KEY,"N",KEY,"EXT  ",KEY,"P",KEY,"REV  ",KEY,"M",KEY,"ENU",0
.h_player EQUS "WARRIOR: YOU - WASD MOVE",0
.h_bat EQUS "BAT: ENEMY",0
.h_ghoul EQUS "GHOUL: ENEMY",0
.h_boss EQUS "WHITE KNIGHT: BOSS",0
.h_heal EQUS "CROSS: RESTORE 16 HP",0
.h_mag EQUS "MAGNET: COLLECT ALL XP",0
.h_xp EQUS "XP ORB: LEVEL UP",0
.h_big EQUS "WHITE XP: MERGED VALUE",0
.h_grave EQUS "GRAVESTONE: SCENERY",0
.h_rock EQUS "ROCK: SCENERY",0
.h_sword EQUS "SWORD: ATTACK FLASH",0

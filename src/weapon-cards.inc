; Rebinding/load tables rely on byte-wide, adjacent ids/stats and common code page.
ASSERT WEAPON_COUNT=4 ; slot*4 catalogue indexing below
ASSERT SLOTBID=SLOTAID+1
ASSERT SLOT_B_STATS=SLOT_A_STATS+4
ASSERT HI(SLOT_A_STATS)=&2E AND HI(SLOT_B_STATS+3)=&2E
ASSERT HI(SLOT_A_CODE)=HI(SLOT_B_CODE)
; NCARDS-only cold slot ownership, stat binding and weapon disk loads.
; A=weapon id, preserve X. C set/Y=owned slot, otherwise C clear/Y=first
; empty slot. Missing with both full returns Y=1; loader/offer gate reject it.
.weapon_slot
    CMP SLOTAID: BEQ weapon_slot_a
    CMP SLOTBID: BEQ weapon_slot_b
    LDY #0: LDA SLOTAID: BEQ weapon_slot_missing
    INY
.weapon_slot_missing
    CLC: RTS
.weapon_slot_a
    LDY #0: SEC: RTS
.weapon_slot_b
    LDY #1: SEC: RTS
.sync_weapon_cards
    LDX #RESTORE-1
.bind_card
    LDA card_owner,X: BEQ bind_next
    JSR weapon_slot
    LDA slot_statlo,Y: STA TMP0
    LDY card_owner,X
    TXA: SEC: SBC weapon_first-1,Y: CLC: ADC TMP0: STA statlo,X
.bind_next
    DEX: CPX #6: BNE bind_card
    RTS
.weapon_first EQUB 7,10,14,16
.load_weapon ; A=id1..4 (the offer only unlocks into a free slot)
    STA load_id: JSR weapon_slot
    BCC load_new
    RTS
.load_new
    STY load_slot
    TYA: ASL A: ASL A: CLC: ADC load_id: SEC: SBC #1: TAX
    LDA #0: LDY #17
.load_clear
    STA FILEBLOCK,Y: DEY: BPL load_clear
    LDA weapon_file_lo,X: STA FILEBLOCK
    LDA weapon_file_hi,X: STA FILEBLOCK+1
    ; Load at the fixed slot address (exec-low 0: the supplied address).
    LDY load_slot
    LDA slot_codelo,Y: STA FILEBLOCK+2
    LDA #HI(SLOT_A_CODE): STA FILEBLOCK+3
    LDA #&FF: LDX #LO(FILEBLOCK): LDY #HI(FILEBLOCK): JSR &FFDD
    LDA load_id: SEC: SBC #1: ASL A: ASL A: TAX
    LDY load_slot: LDA slot_statlo,Y: TAY
    LDA #4: STA load_left
.load_defaults
    LDA weapon_defaults,X: STA &2E00,Y
    INX: INY: DEC load_left: BNE load_defaults
    LDX load_slot: LDA load_id: STA SLOTAID,X
    RTS
.load_id EQUB 0
.load_slot EQUB 0
.load_left EQUB 0
.slot_statlo EQUB LO(SLOT_A_STATS),LO(SLOT_B_STATS)
.slot_codelo EQUB LO(SLOT_A_CODE),LO(SLOT_B_CODE)
.weapon_defaults EQUB 0,24,1,0, 0,1,20,6, 0,4,3,3, 0,2,8,4
.weapon_file_lo EQUB LO(weapon_orba),LO(weapon_seeda),LO(weapon_storma),LO(weapon_warda),LO(weapon_orbb),LO(weapon_seedb),LO(weapon_stormb),LO(weapon_wardb)
.weapon_file_hi EQUB HI(weapon_orba),HI(weapon_seeda),HI(weapon_storma),HI(weapon_warda),HI(weapon_orbb),HI(weapon_seedb),HI(weapon_stormb),HI(weapon_wardb)
.weapon_orba EQUS "ORBA",13
.weapon_seeda EQUS "SEEDA",13
.weapon_storma EQUS "STORMA",13
.weapon_warda EQUS "WARDA",13
.weapon_orbb EQUS "ORBB",13
.weapon_seedb EQUS "SEEDB",13
.weapon_stormb EQUS "STORMB",13
.weapon_wardb EQUS "WARDB",13

ASSERT weapon_file_lo-weapon_defaults=4*WEAPON_COUNT
ASSERT weapon_file_hi-weapon_file_lo=2*WEAPON_COUNT
ASSERT weapon_orba-weapon_file_hi=2*WEAPON_COUNT

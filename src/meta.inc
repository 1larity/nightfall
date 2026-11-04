.crc_byte                ; CRC16-CCITT step, same as resident
    EOR CRC+1: STA CRC+1
    LDX #8
.crc_bit
    ASL CRC: ROL CRC+1: BCC crc_next
    LDA CRC: EOR #&21: STA CRC
    LDA CRC+1: EOR #&10: STA CRC+1
.crc_next
    DEX: BNE crc_bit
    RTS
.meta_consider
    ; META (magic, CRC) valid and the newest seen since p_best_clear: keep it
    ; in p_best. Saves alternate two slots; the higher generation wins.
    LDX #3
.mc_magic
    LDA META,X: CMP p_meta_magic,X: BNE mc_done
    DEX: BPL mc_magic
    JSR meta_crc
    LDA CRC: CMP META+14: BNE mc_done
    LDA CRC+1: CMP META+15: BNE mc_done
    LDA p_valid: BEQ mc_accept
    LDA META+4: SEC: SBC p_best+4: BMI mc_done
.mc_accept
    LDA #1: STA p_valid
    LDX #15
.mc_keep
    LDA META,X: STA p_best,X: DEX: BPL mc_keep
.mc_done
    RTS
.meta_crc ; CRC16 of META+0..13
    LDA #255: STA CRC: STA CRC+1
    LDX #0
.meta_crc_loop
    STX CRCINDEX: LDA META,X: JSR crc_byte: LDX CRCINDEX
    INX: CPX #14: BNE meta_crc_loop
    RTS
.p_best_clear
    LDA #0: STA p_valid: LDX #15
.pbc_loop
    STA p_best,X: DEX: BPL pbc_loop
    RTS
.legacy_load
    JSR p_best_clear
    LDX #1
.lm_loop
    STX TMP2
    JSR meta_block
    ; Catalogue first: missing slots are normal, never LOAD a missing file.
    LDA #5: LDX #LO(FILEBLOCK): LDY #HI(FILEBLOCK): JSR &FFDD
    CMP #1: BNE lm_next
    LDX TMP2: JSR meta_block
    LDA #&FF: LDX #LO(FILEBLOCK): LDY #HI(FILEBLOCK): JSR &FFDD
    JSR meta_consider
.lm_next
    LDX TMP2
    DEX
    BPL lm_loop
.best_to_globals ; p_best (zeros when nothing was valid) -> the player's stats
    LDA p_best+4: STA GEN
    LDA p_best+5: STA GOLDL: LDA p_best+6: STA GOLDH
    LDX #5
.btg_upg
    LDA p_best+7,X: STA UPGV,X: DEX: BPL btg_upg
    LDA p_best+13: STA VOLDOWN
    RTS
.make_meta
    LDX #3
.mm_magic
    LDA p_meta_magic,X: STA META,X: DEX: BPL mm_magic
    INC GEN: LDA GEN: STA META+4
    LDA GOLDL: STA META+5: LDA GOLDH: STA META+6
    LDA UPGV: STA META+7: LDA UPGS: STA META+8: LDA UPGP: STA META+9
    LDA UPGF: STA META+10: LDA UPGM: STA META+11: LDA UPGG: STA META+12
    LDA VOLDOWN: STA META+13 ; volume below full (older saves: 0 = full)
    JSR meta_crc
    LDA CRC: STA META+14: LDA CRC+1: STA META+15
    RTS
.meta_block
    ; MOS OSFILE: 18-byte block, 32-bit addresses, caller CPU memory.
    TXA: PHA
    LDA #0: LDX #17
.mb_clear
    STA FILEBLOCK,X: DEX: BPL mb_clear
    PLA: TAX
    LDA mfn_lo,X: STA FILEBLOCK: LDA mfn_hi,X: STA FILEBLOCK+1
    LDA #LO(META): STA FILEBLOCK+2: STA FILEBLOCK+10
    LDA #HI(META): STA FILEBLOCK+3: STA FILEBLOCK+11
    LDA #LO(META+16): STA FILEBLOCK+14
    LDA #HI(META+16): STA FILEBLOCK+15
    RTS
IF PLAYERS=0 ; NPLAYER never saves an unnamed player or banks a run
.legacy_save
    JSR make_meta
    LDA GEN: AND #1: TAX
    JSR meta_block
    LDA #0: LDX #LO(FILEBLOCK): LDY #HI(FILEBLOCK): JSR &FFDD
    RTS
.bank_and_save
    ; Death: bank the run's gold, enter the death summary state, then
    ; persist the profile.
    LDA RUNGOLD: CLC: ADC GOLDL: STA GOLDL
    LDA GOLDH: ADC RUNGOLDH: STA GOLDH
    LDA QUITRUN: BEQ bank_death
    ; Quit from pause: straight back to the main menu.
    LDA #0: STA QUITRUN: STA STATE: STA DRAWN
    JMP save_meta
.bank_death
    LDA #3: STA STATE
    LDA #0: STA DRAWN
    LDA #90: STA DEADWAIT
    JMP save_meta
ENDIF
.mfn_lo EQUB LO(mfn0),LO(mfn1)
.mfn_hi EQUB HI(mfn0),HI(mfn1)
.mfn0 EQUS "NFSV0",13
.mfn1 EQUS "NFSV1",13

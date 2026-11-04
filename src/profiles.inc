; Single file, fixed64-byte records,32-bit disk offsets. No resident profile array.
; Header NFP1 + selected record offset. Deleted records are tombstones.
PSELECT=&2B64
.load_meta
    JSR legacy_load
    LDA #0: STA PLAYERNAME: LDX #3
.pl_zero
    STA PSELECT,X: DEX: BPL pl_zero
    JSR p_open_read: BEQ pl_done
    JSR p_header
    LDX #3
.pl_select
    LDA p_record+4,X: STA PSELECT,X: STA p_cursor,X: DEX: BPL pl_select
    LDA PSELECT: ORA PSELECT+1: ORA PSELECT+2: ORA PSELECT+3: BEQ pl_close
    JSR p_read_record
    LDA p_record: BNE pl_valid
    LDA #0: LDX #3
.pl_clear
    STA PSELECT,X: DEX: BPL pl_clear
    JMP pl_close
.pl_valid
    JSR p_load_stats
.pl_close
    JSR p_close
.pl_done
.send_volume ; the selected profile's volume to EAUDIO (also defines envelopes)
    LDA #3: SEC: SBC VOLDOWN: STA eaud_block+8
    LDX #7: JMP eaud_cmd
IF PLAYERS=0
.save_meta
    LDA PSELECT: ORA PSELECT+1: ORA PSELECT+2: ORA PSELECT+3: BNE ps_named
    JSR p_create
    BCS ps_legacy
    RTS
.ps_legacy
    JMP legacy_save
ENDIF
.ps_named
    JSR p_open_update: BEQ ps_done
    LDX #3
.ps_cursor
    LDA PSELECT,X: STA p_cursor,X: DEX: BPL ps_cursor
    JSR make_meta
    ; Alternate16-byte CRC slots; preserve the prior slot and profile name.
    LDA GEN: AND #1: ASL A: ASL A: ASL A: ASL A
    CLC: ADC #32: STA p_extra
    JSR p_seek_extra
    LDX #0
.ps_write
    STX p_index: LDA META,X: LDY p_handle: JSR &FFD4
    LDX p_index: INX: CPX #16: BNE ps_write
    JMP p_close
.ps_done
    RTS
.p_open_read
    LDA #&40: BNE p_open
.p_open_update
    LDA #&C0
.p_open
    LDX #LO(p_filename): LDY #HI(p_filename): JSR &FFCE
    STA p_handle: CMP #0: RTS
.p_close
    LDA #0: LDY p_handle: JMP &FFCE
.p_header
    LDA #0: STA p_extra
    LDX #3
.ph_zero
    STA p_cursor,X: DEX: BPL ph_zero
    JSR p_seek
    LDX #0
.ph_read
    STX p_index: LDY p_handle: JSR &FFD7
    LDX p_index: STA p_record,X: INX: CPX #8: BNE ph_read
    RTS
.p_seek_extra
    CLC: LDA p_cursor: ADC p_extra: STA &70
    LDA p_cursor+1: ADC #0: STA &71
    LDA p_cursor+2: ADC #0: STA &72
    LDA p_cursor+3: ADC #0: STA &73
    JMP p_seek_args
.p_seek
    LDX #3
.p_seek_copy
    LDA p_cursor,X: STA &70,X: DEX: BPL p_seek_copy
.p_seek_args
    LDA #1: LDX #&70: LDY p_handle: JMP &FFDA
.p_extent
    LDA #2: LDX #&70: LDY p_handle: JSR &FFDA
    LDX #3
.pe_copy
    LDA &70,X: STA p_end,X: DEX: BPL pe_copy
    RTS
.p_read_record
    JSR p_seek
    LDX #0
.pr_loop
    STX p_index: LDY p_handle: JSR &FFD7
    LDX p_index: STA p_record,X: INX: CPX #64: BNE pr_loop
    RTS
.p_write_record
    JSR p_seek
    LDX #0
.pw_loop
    STX p_index: LDA p_record,X: LDY p_handle: JSR &FFD4
    LDX p_index: INX: CPX #64: BNE pw_loop
    RTS
.p_set_selected
    LDX #3
.pss_copy
    LDA p_cursor,X: STA PSELECT,X: DEX: BPL pss_copy
    ; Header active pointer starts at byte4, not at current record.
    LDA #4: STA &70: LDA #0: STA &71: STA &72: STA &73
    JSR p_seek_args
    LDX #0
.pss_write
    STX p_index: LDA PSELECT,X: LDY p_handle: JSR &FFD4
    LDX p_index: INX: CPX #4: BNE pss_write
    RTS
.p_load_stats
    LDX #23
.pls_name
    LDA p_record+1,X: STA PLAYERNAME,X: DEX: BPL pls_name
    JSR p_newest_slot
    JMP best_to_globals
; The newest valid 16-byte meta slot of p_record (offsets 32/48) into p_best,
; or zeros. Display uses it without touching the selected player's stats.
.p_newest_slot
    JSR p_best_clear
    LDA #32
.pls_loop
    STA p_slot: TAX: LDY #0
.pls_copy
    LDA p_record,X: STA META,Y: INX: INY: CPY #16: BNE pls_copy
    JSR meta_consider
    LDA p_slot: EOR #32 EOR 48: CMP #48: BEQ pls_loop
    RTS
.p_create
    LDA #0: LDX #63
.pc_clear
    STA p_record,X: DEX: BPL pc_clear
    JSR p_name: BCC pc_name_valid
    RTS
.pc_name_valid
    JSR p_open_update: BNE pc_existing
    LDA #&80: JSR p_open
    BEQ pc_cancel
    LDX #0
.pc_header
    STX p_index: LDA p_magic,X: LDY p_handle: JSR &FFD4
    LDX p_index: INX: CPX #8: BNE pc_header
    JSR p_close: JSR p_open_update
.pc_existing
    LDX #7
.pc_name_backup
    LDA p_record,X: STA META,X: DEX: BPL pc_name_backup
    JSR p_header
    LDX #7
.pc_name_restore
    LDA META,X: STA p_record,X: DEX: BPL pc_name_restore
    JSR p_extent
    LDX #3
.pc_cursor
    LDA p_end,X: STA p_cursor,X: DEX: BPL pc_cursor
    LDA #1: STA p_record
    JSR make_meta
    LDX #15
.pc_meta
    LDA META,X: STA p_record+32,X: STA p_record+48,X: DEX: BPL pc_meta
    JSR p_write_record: JSR p_set_selected: JSR p_close
    JSR p_load_stats
    CLC: RTS
.pc_cancel
    SEC: RTS
.p_name
    LDX #0 ; the stored name is always terminated
.pn_length
    LDA p_record+1,X: BEQ pn_ready
    INX: BNE pn_length
.pn_ready
    STX p_length
    JSR pn_page
    ; Typing redraws only the name, and after PRESENT the CPU writes main RAM,
    ; so the prompt must be showing main. ACCCON bit 0 set means shadow is
    ; showing: one more page flips to main. (A BBC B just redraws.)
    LDA #&96: LDX #&34: JSR &FFF4: TYA: LSR A: BCC pn_key
    JSR pn_page
    JMP pn_key
.pn_page
    JSR menu_background
    LDA #LO(p_name_title): LDX #HI(p_name_title): LDY #48: JSR p_text
    LDA #LO(p_name_keys): LDX #HI(p_name_keys): LDY #200: JSR p_text
    LDA #LO(p_record+1): LDX #HI(p_record+1): LDY #104: JSR p_text
    LDA #6: JMP p_gfx
.pn_draw
    ; Opaque cells (one trailing space) erase a deleted character.
    LDX p_length: LDA #32: STA p_record+1,X
    LDA #0: STA p_record+2,X
    LDA #LO(p_record+1): LDX #HI(p_record+1): LDY #104: JSR p_text
    LDX p_length: LDA #0: STA p_record+1,X
.pn_key
    JSR p_key
    CMP #27: BEQ pn_cancel
    CMP #13: BEQ pn_enter
    CMP #127: BEQ pn_back
    CMP #32: BCC pn_key
    CMP #96: BCS pn_key
    LDX p_length: CPX #23: BCS pn_key
    STA p_record+1,X: INC p_length: JMP pn_draw
.pn_back
    LDA p_length: BEQ pn_key
    DEC p_length
    JMP pn_draw
.pn_enter
    LDA p_length: BEQ pn_key
    CLC: RTS
.pn_cancel
    SEC: RTS
IF PLAYERS ; the Players screen: NPLAYER only
.profiles_screen
    JSR p_open_read: BEQ pm_empty
    JSR p_header
    JSR p_extent
    LDX #3
.pm_cur
    LDA PSELECT,X: STA p_cursor,X: DEX: BPL pm_cur
    LDA p_cursor: ORA p_cursor+1: ORA p_cursor+2: ORA p_cursor+3: BNE pm_read
    LDA #8: STA p_cursor
.pm_read
    JSR p_read_record: JSR p_close
    JMP pm_draw
.pm_empty
    LDA #0: STA p_record
.pm_draw
    JSR p_draw_profiles
.pm_present
    LDA #6: JSR p_gfx
.pm_poll
    JSR p_key
    CMP #66: BEQ pm_return
    CMP #27: BEQ pm_return
    CMP #78: BEQ pm_new
    CMP #86: BEQ pm_volume
    CMP #138: BEQ pm_next_jump
    CMP #83: BEQ pm_next_jump
    CMP #139: BEQ pm_prev_jump
    CMP #87: BEQ pm_prev_jump
    LDX p_record: BEQ pm_poll
    CMP #13: BEQ pm_select
    CMP #32: BEQ pm_select
    CMP #82: BEQ pm_rename
    CMP #68: BEQ pm_delete
    JMP pm_poll
.pm_next_jump
    JMP pm_next
.pm_prev_jump
    JMP pm_prev
.pm_return
    JSR load_meta
    RTS
.pm_volume
    ; V: next volume step for the selected profile, heard and saved at once.
    LDA VOLDOWN: CLC: ADC #1: AND #3: STA VOLDOWN
    JSR send_volume
    LDA PSELECT: ORA PSELECT+1: ORA PSELECT+2: ORA PSELECT+3: BEQ pm_volume_shown
    JSR ps_named
.pm_volume_shown
    JMP pm_draw
.pm_new
    LDA #0: STA GOLDL: STA GOLDH: STA GEN
    LDX #5
.pmn_zero
    STA UPGV,X: DEX: BPL pmn_zero
    STA VOLDOWN ; new players start at full volume
    JSR p_create: BCC pm_new_done
    JSR load_meta
.pm_new_done
    JMP profiles_screen
.pm_select
    JSR p_open_update: JSR p_set_selected
    JSR p_close: JSR p_load_stats
    RTS
.pm_rename
    JSR p_name: BCC pm_rename_save
    JSR p_open_read: JSR p_read_record: JSR p_close
    JMP pm_draw
.pm_rename_save
    JSR p_open_update: JSR p_write_record: JSR p_close
.pm_redraw
    JMP pm_draw
.pm_delete
    JSR p_draw_profiles
    LDA #LO(p_confirm): LDX #HI(p_confirm): LDY #136: JSR p_text
    LDA #6: JSR p_gfx: JSR p_key
    CMP #89: BNE pm_redraw
    JSR p_open_update
    LDA #0: STA p_record: JSR p_write_record
    ; Deleting selected profile clears selection; never switches silently.
    LDX #3
.pmd_cmp
    LDA PSELECT,X: CMP p_cursor,X: BNE pmd_keep
    DEX: BPL pmd_cmp
    LDA #0: LDX #3
.pmd_clear
    STA p_cursor,X: DEX: BPL pmd_clear
    STA PLAYERNAME
    JSR p_set_selected: JSR legacy_load
.pmd_keep
    JSR p_close: JMP profiles_screen
.pm_next
    LDA #64: BNE pm_step
.pm_prev
    LDA #192
.pm_step
    STA p_step
    JSR p_open_read: BNE pms_open
    RTS
.pms_open
    JSR p_extent
    LDX #3
.pms_start
    LDA p_cursor,X: STA p_start,X: DEX: BPL pms_start
.pms_loop
    LDY #0: LDA p_step: BPL pms_add
    DEY ; 192 = -64: high bytes add &FF
.pms_add
    CLC: ADC p_cursor: STA p_cursor
    LDX #253 ; p_cursor+1..3; INX keeps the carry
.pms_carry
    TYA: ADC p_cursor-252,X: STA p_cursor-252,X: INX: BNE pms_carry
.pms_bounds
    LDA p_cursor+3: BMI pms_wrap_last
    LDA p_cursor+1: ORA p_cursor+2: ORA p_cursor+3: BNE pms_upper
    LDA p_cursor: CMP #8: BCC pms_wrap_last
.pms_upper
    LDX #3
.pms_cmp
    LDA p_cursor,X: CMP p_end,X: BCC pms_read
    BNE pms_first
    DEX: BPL pms_cmp
.pms_first
    LDA #0: STA p_cursor+1: STA p_cursor+2: STA p_cursor+3
    LDA #8: STA p_cursor: BNE pms_read
.pms_wrap_last
    SEC: LDA p_end: SBC #64: STA p_cursor
    LDA p_end+1: SBC #0: STA p_cursor+1
    LDA p_end+2: SBC #0: STA p_cursor+2
    LDA p_end+3: SBC #0: STA p_cursor+3
.pms_read
    JSR p_read_record
    LDA p_record: BNE pms_found
    LDX #3
.pms_stop
    LDA p_cursor,X: CMP p_start,X: BEQ pms_same
    JMP pms_loop
.pms_same
    DEX: BPL pms_stop
.pms_found
    JSR p_close: JMP pm_draw
.p_draw_profiles
    JSR menu_background
    LDA #LO(p_title): LDX #HI(p_title): LDY #48: JSR p_text
    LDA #LO(p_keys): LDX #HI(p_keys): LDY #176: JSR p_text
    LDA #LO(p_keys2): LDX #HI(p_keys2): LDY #200: JSR p_text
    LDA #3: SEC: SBC VOLDOWN: ORA #48: STA p_volume_digit
    LDA #LO(p_volume): LDX #HI(p_volume): LDY #160: JSR p_text
    LDA p_record: BEQ pm_none
    LDA #LO(p_record+1): LDX #HI(p_record+1): LDY #88: JSR p_text
    JSR p_draw_stats
    JMP p_draw_profiles_done
.pm_none
    LDA #LO(p_empty): LDX #HI(p_empty): LDY #88: JSR p_text
.p_draw_profiles_done
    ; Cyan rules framing the four profile lines (name, gold, two stat rows).
    LDX #0
.pd_rule
    LDA p_rules,X: STA EGBUF+8
    LDA #0: STA EGBUF+9
    LDA p_rules+1,X: STA EGBUF+10
    LDA #0: STA EGBUF+11
    LDA p_rules+2,X: STA EGBUF+16
    LDA p_rules+3,X: STA EGBUF+17
    LDA p_rules+4,X: STA EGBUF+20
    LDA #0: STA EGBUF+21
    LDA p_rules+5,X: STA EGBUF+13
    LDA #0: STA EGBUF+7
    TXA: PHA: LDA #10: JSR p_gfx ; (p_gfx's OSWORD loads X)
    PLA: CLC: ADC #6: TAX
    CPX #p_rules_end-p_rules: BCC pd_rule
    RTS
.p_draw_stats
    ; Progress of the browsed player: banked gold and every upgrade level.
    JSR p_newest_slot
    LDA p_best+5: STA NUM: LDA p_best+6: STA NUM+1
    LDX #0
.pds_gold
    LDY #47
.pds_digit
    INY
    LDA NUM: SEC: SBC pds_ten_lo,X: STA TMP0
    LDA NUM+1: SBC pds_ten_hi,X: BCC pds_place
    STA NUM+1: LDA TMP0: STA NUM: JMP pds_digit
.pds_place
    TYA: STA p_stat_gold,X
    INX: CPX #5: BNE pds_gold
    ; No leading zeros: slide the digits (and terminator) left.
    LDX #0
.pds_trim
    LDA p_stat_gold,X: CMP #48: BNE pds_shift
    INX: CPX #4: BNE pds_trim
.pds_shift
    LDY #0
.pds_move
    LDA p_stat_gold,X: STA p_stat_gold,Y
    INY: INX: CPX #6: BNE pds_move
    LDX #5
.pds_level
    LDA p_best+7,X: LDY #47: SEC
.pds_tens
    INY: SBC #10: BCS pds_tens
    ADC #58: PHA
    LDA pds_at_lo,X: STA &F8: LDA pds_at_hi,X: STA &F9
    TYA: CMP #48: BNE pds_tens_shown
    LDA #32 ; a space, not a leading zero
.pds_tens_shown
    LDY #0: STA (&F8),Y
    PLA: INY: STA (&F8),Y
    DEX: BPL pds_level
    LDA #LO(p_stat1): LDX #HI(p_stat1): LDY #104: JSR p_text
    LDA #LO(p_stat2): LDX #HI(p_stat2): LDY #120: JSR p_text
    LDA #LO(p_stat3): LDX #HI(p_stat3): LDY #136: JMP p_text
.pds_ten_lo EQUB LO(10000),LO(1000),LO(100),LO(10),LO(1) ; most significant first
.pds_ten_hi EQUB HI(10000),HI(1000),HI(100),HI(10),HI(1)
.pds_at_lo EQUB LO(p_stat2+7),LO(p_stat2+16),LO(p_stat2+25),LO(p_stat3+7),LO(p_stat3+17),LO(p_stat3+26)
.pds_at_hi EQUB HI(p_stat2+7),HI(p_stat2+16),HI(p_stat2+25),HI(p_stat3+7),HI(p_stat3+17),HI(p_stat3+26)
ENDIF
.p_key
    LDA #19: JSR &FFF4
    LDA #129: LDX #0: LDY #0: JSR &FFF4: BCS p_key
    TXA: CMP #97: BCC pk_done
    CMP #123: BCS pk_done
    AND #&DF
.pk_done
    JMP menu_click
.p_text ; A/X = string, Y = y, at x=48; KEY-highlighted keys
    STA &F8: STX &F9: TYA: TAX: LDA #48: JMP htext
.p_gfx
    STA EGBUF+5
    LDA #64: STA EGBUF: LDA #8: STA EGBUF+1
    LDA #'E': STA EGBUF+2: LDA #'G': STA EGBUF+3
    LDA #1: STA EGBUF+4
    LDA #&C0: LDX #LO(EGBUF): LDY #HI(EGBUF): JMP &FFF1
.p_magic EQUS "NFP1": EQUD 0
.p_meta_magic EQUS "NFM1"
.p_filename EQUS "NFPROFS",13
IF PLAYERS
.p_keys EQUS KEY,"W/S",KEY," BROWSE  ",KEY,"ENTER",KEY," SELECT",0
.p_keys2 EQUS KEY,"N",KEY,"EW  ",KEY,"R",KEY,"ENAME  ",KEY,"D",KEY,"ELETE  ",KEY,"B",KEY,"ACK",0
.p_rules EQUB 48,84,16,1,84,2,48,154,16,1,154,2 ; x,y,x2lo,x2hi,y2,cyan
.p_rules_end
.p_stat1 EQUS "GOLD "
.p_stat_gold EQUS "00000",0
.p_stat2 EQUS "VIGOUR 00  LUCK 00  WARD 00",0 ; digit cells, filled in
.p_stat3 EQUS "REROLL 00 MAGNET 00 GREED 00",0
.p_volume EQUS KEY,"V",KEY,"OLUME "
.p_volume_digit EQUS "3",0
ENDIF
.p_name_title EQUS "NAME YOUR PLAYER",0
.p_name_keys EQUS KEY,"ENTER",KEY," SAVE  ",KEY,"ESC",KEY," CANCEL",0
IF PLAYERS
.p_confirm EQUS "DELETE: ",KEY,"Y",KEY,"ES / OTHER KEY NO",0
ENDIF
.p_cursor SKIP 4
.p_end SKIP 4
.p_start SKIP 4
.p_record SKIP 64
.p_handle EQUB 0
.p_index EQUB 0
.p_extra EQUB 0
.p_valid EQUB 0
.p_best SKIP 16
.p_slot EQUB 0
.p_length EQUB 0
.p_step EQUB 0

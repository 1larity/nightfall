; EAUDIO effect data. MOS pitch: 4 per semitone, 53 = middle C (C4);
; duration in 1/20s. Noise channel "pitch": 0..3 periodic, 4..7 white
; (high, middle, low, tone-linked).
; Note channel byte: 0 noise / 1 any tone channel, +128 another note
; follows, +64 queued on the previous note's channel.
MORE=128: QUEUED=64
.notes
EQUB 69,4,1+MORE,&F4 ; 0 death dirge: E4 ...
EQUB 53,4,1+MORE+QUEUED,&F4 ;   C4 ...
EQUB 41,10,1+QUEUED,&F4 ;   A3, held
EQUB 44,2,1,&F4 ; 3 hurt: a short low tone
EQUB 101,2,1+MORE,1 ; 4 level-up fanfare: C5 E5 G5 C6, ringing
EQUB 117,2,1+MORE+QUEUED,1
EQUB 129,2,1+MORE+QUEUED,1
EQUB 149,6,1+QUEUED,1
EQUB 169,8,1+MORE,1 ; 8 XP bell: F6 ...
EQUB 197,8,1,1 ;   with C7 on another channel
EQUB 5,1,0+MORE,2 ; 10 kill crunch: a mid white-noise snap ...
EQUB 6,2,0+QUEUED,2 ;   then a low white-noise crunch (noise only: kills never take a tone channel)
EQUB 4,1,0,&F7 ; 12 menu key: a high white-noise tick
EQUB 129,1,1+MORE,1 ; 13 chest sparkle: G5 B5 D6 G6
EQUB 145,1,1+MORE+QUEUED,1
EQUB 157,1,1+MORE+QUEUED,1
EQUB 177,4,1+QUEUED,1
EQUB 6,4,0+MORE,3 ; 17 boss fanfare: a drum boom ...
EQUB 41,3,1+MORE,2 ;   "da-da-DAAA": A3 and A3, short stings (decay articulates) ...
EQUB 41,3,1+MORE+QUEUED,2
EQUB 61,16,1+MORE+QUEUED,&F2 ;   then D4 held ...
EQUB 0,6,1+MORE,0 ;   and, on a second voice after the same 6cs,
EQUB 73,16,1+QUEUED,&F3 ;   F4 above it: a D minor call (at most 3 notes per queue)
EQUB 4,1,0+MORE,&F5 ; 23 purchase: white "ch" ...
EQUB 0,3,0+QUEUED,&F5 ;   then a metallic periodic "ing"
EQUB 13,2,1+MORE,3 ; 25 heartbeat: lub ...
EQUB 0,1,1+MORE+QUEUED,0 ;   (gap)
EQUB 9,2,1+QUEUED,3 ;   dub
.notes_end
ASSERT notes_end-notes<=256

; First note of each effect ID. IDs 1, 5 and 7 are never sent (holes).
.sfx_start
EQUB 0,0,3,4,8,0,10,0,12,13 ; 0 death, 2 hurt, 3 level up, 4 XP, 6 kill,
EQUB 17,23,25 ; 8 menu key, 9 chest, 10 boss, 11 purchase, 12 heartbeat
SFX_COUNT=P%-sfx_start
; Priority (bits0..1, higher wins a tone channel) and pitch variation
; (bits4..5) per effect ID.
.sfx_flags
EQUB 3,0,2,2,0+16,0,1+32,0,0,2
EQUB 3,0,1
ASSERT P%-sfx_flags=SFX_COUNT

; ENVELOPE N,T,PI1,PI2,PI3,PN1,PN2,PN3,AA,AD,AS,AR,ALA,ALD.
.envelopes
EQUB 1,1,0,0,0,0,0,0,126,LO(-3),0,LO(-8),126,0 ; bell: instant attack, ~0.4s ring
EQUB 2,1,0,0,0,0,0,0,126,LO(-16),0,LO(-16),126,0 ; sting: ~0.08s burst
EQUB 3,&81,LO(-3),0,0,8,0,0,126,LO(-10),0,LO(-10),126,0 ; thud: -6 semitones, once
EQUB 4,1,0,0,0,0,0,0,40,LO(-2),0,LO(-6),100,80 ; music: soft attack, held
.envelopes_end

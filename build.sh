#!/usr/bin/env bash
# Builds nightfall.ssd, NFASSETS.rom and EAUDIO.rom into build/.
# Needs BeebAsm (BEEBASM, default beebasm) and Python 3.
set -euo pipefail
cd "$(dirname "$0")"
BEEBASM="${BEEBASM:-beebasm}"

# NIGHT first: the overlays are assembled against its label addresses.
"$BEEBASM" -i src/bootstrap.6502
"$BEEBASM" -D ARENA=0 -i src/engine.6502 -d -labels build/labels.txt >/dev/null
python3 - <<'PY'
import ast, re
from pathlib import Path
labels = ast.literal_eval(re.sub(r'(\d+)L', r'\1', Path('build/labels.txt').read_text()))[0]
Path('build/engine-entry.inc').write_text('weapon_hit=%d\n' % labels['weapon_hit'])
names = {'NIGHT_WEAPON_HIT': 'weapon_hit', 'NIGHT_MAIN': 'main', 'NIGHT_ROM_SPRITE': 'rom_sprite',
         'NIGHT_RANDOM': 'random', 'NIGHT_EG_CALL': 'eg_call'}
Path('build/night-entry.inc').write_text(''.join('%s=%d\n' % (k, labels[v]) for k, v in names.items()))
PY
"$BEEBASM" -i src/weapons.6502
for slot in 0 1; do
  for kind in 1 2 3 4; do
    "$BEEBASM" -D SLOT="$slot" -D WEAPON_KIND="$kind" -i src/weapon-slot.6502
  done
done

# NARENA is the same engine with ARENA=1; the boss pages use its labels.
"$BEEBASM" -D ARENA=1 -i src/engine.6502 -d -labels build/labels.txt >/dev/null
python3 - <<'PY'
import ast, re
from pathlib import Path
labels = ast.literal_eval(re.sub(r'(\d+)L', r'\1', Path('build/labels.txt').read_text()))[0]
names = {'ARENA_EG_CALL': 'eg_call', 'ARENA_SFX': 'a_sfx', 'ARENA_HURT': 'a_hurt_by', 'ARENA_PHASE': 'a_phase', 'ARENA_WON': 'a_won'}
Path('build/arena-entry.inc').write_text(''.join('%s=%d\n' % (k, labels[v]) for k, v in names.items()))
PY

"$BEEBASM" -D CARD=0 -i src/ui.6502
"$BEEBASM" -D CARD=1 -i src/ui.6502
for program in reset players combat-overlay elite-page wizard-page boss-bell boss-wyrm boss-choir eaudio nf-companion; do
  "$BEEBASM" -i "src/$program.6502"
done

# NFLOAD asserts that every overlay fits its window.
python3 - <<'PY'
from pathlib import Path
files = {'UI': 'NIGHTUI', 'ENGINE': 'NIGHT', 'CARD': 'NCARDS', 'RESET': 'NRESET', 'COMBAT': 'NFCOMB', 'PLAYER': 'NPLAYER', 'ARENA': 'NARENA'}
Path('build/overlay-sizes.inc').write_text(''.join('%s_SIZE=%d\n' % (k, Path('build', v).stat().st_size) for k, v in files.items()))
PY
"$BEEBASM" -i src/overlay-loader.6502
"$BEEBASM" -i src/disc.6502 -do build/nightfall.new.ssd -opt 3 -title NIGHTFALL
mv -f build/nightfall.new.ssd build/nightfall.ssd
echo "Built build/nightfall.ssd, build/NFASSETS.rom and build/EAUDIO.rom"

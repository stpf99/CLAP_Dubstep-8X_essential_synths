# DS Dubstep Kit — 51 wtyczek CLAP (Bitwig, bez GUI)

Jeden silnik (`src/ds_engine.h`, mowa: `src/ds_speech.h`), opakowanie CLAP w `src/ds_plugin.cpp`. Bitwig rysuje generyczne suwaki z parametrów.
`make` buduje **51 osobnych** plików `build/DS-*.clap` oraz paczkę `build/DubstepKit.clap` (wszystkie typy w jednym pliku).

Wspólne cechy: 6 kształtów fali (Sine, Triangle, Saw, Square, Noise, Metal) z płynnym morphem, ADSR (czasy logarytmiczne), glide, tune/fine, unison, wygładzanie parametrów (5 ms), zapis stanu w projekcie Bitwiga. Wszystkie wtyczki są jednogłosowe (mono; basy legato, perkusja retrigger/choke). Perkusja ma Key Track = 0 domyślnie. EQ / kompresja / reverb / delay — zewnętrznymi wtyczkami; filtr w silniku jest charakterem brzmienia (wobble, growl, reese…).

Historia rozbudowy i detale nowszych funkcji: **[CHANGES.md](CHANGES.md)**.

## Lista wtyczek (51)

| # | Wtyczka | ID | Rola / cecha |
|---|---------|-----|--------------|
| 0 | DS Sub | `sub` | sub bas: sinus/tri, mono glide, soft drive |
| 1 | DS Wobble | `wobble` | wobble bas: saw unison + LFO filtra sync do tempa |
| 2 | DS Reese | `reese` | reese: rozstrojone piły, wolny ruch filtra |
| 3 | DS Growl | `growl` | growl/yoi: FM + formanty A-E-I-O-U |
| 4 | DS Screech | `screech` | lead: FM + unison + scoop pitch + env filtra |
| 5 | DS Kick | `kick` | kick: sinus + pitch env, klik szumowy, drive |
| 6 | DS Snare | `snare` | snare/clap: korpus + szum HP, bursty |
| 7 | DS Hat | `hat` | hat: metal cluster 808 + szum, HP, choke |
| 8 | DS Clap | `clap` | clap: szum z burstami HP+LP, bez tonu |
| 9 | DS Rim | `rim` | rim/click: krótki tri + tick szumu |
| 10 | DS Tom | `tom` | tom: sinus + pitch env, keytrack (fille) |
| 11 | DS Crash | `crash` | crash: metal + długi szum |
| 12 | DS Shaker | `shaker` | shaker: filtrowane bursty szumu |
| 13 | DS FM Bass | `fmbass` | FM bass: carrier + ratio, LFO na indeksie FM |
| 14 | DS Neuro | `neuro` | neuro: piły + formanty + FM, szybkie LFO |
| 15 | DS Hollow | `hollow` | hollow: pulse-width square, filter wobble |
| 16 | DS Supersaw | `supersaw` | supersaw: 7 pił, szeroki, filter env |
| 17 | DS Pluck | `pluck` | pluck: krótki filtered saw/square, retrigger |
| 18 | DS Pad | `pad` | pad: wolny atak, detune, dryfujący filtr |
| 19 | DS Stab | `stab` | stab: detuned saw/FM hit, filter env, retrigger |
| 20 | DS Riser | `riser` | riser: noise + saw, rampa pitch/cutoff/level |
| 21 | DS Boom | `boom` | boom/impact: długi zjazd sinusa + rumble |
| 22 | DS Zap | `zap` | zap/laser: szybki pitch dive + FM |
| 23 | DS Vox | `vox` | vox chop: saw przez formanty, retrigger |
| 24 | DS Speak | `speak` | synteza mowy (EN): tekst → fonemy → formanty |
| 25 | DS Theremin | `theremin` | theremin: near-sine, długi glide, vibrato |
| 26 | DS Pan Flute | `panflute` | flet: ton + breath noise śledzący wysokość |
| 27 | DS Handpan | `handpan` | handpan: modal 1:2:3, długi ring |
| 28 | DS Tongue Drum | `tongue` | tongue drum: modal, krótszy ring |
| 29 | DS Piano | `piano` | piano-like: soft hammer, mild detune, retrigger |
| 30 | DS Acid | `acid` | acid 303: resonant filter env, square/saw, glide |
| 31 | DS Strings | `strings` | strings: slow attack, long release, wide detune |
| 32 | DS Organ | `organ` | organ: pulse + sub-oct, vibrato, drive |
| 33 | DS E-Piano | `epiano` | e-piano: sine + soft FM tine, mild chorus |
| 34 | DS Harpsichord | `harpsichord` | harpsichord: bright pluck, quick decay |
| 35 | DS Flute | `flute` | flute: near-sine, breath, gentle vibrato |
| 36 | DS Brass | `brass` | brass: saw unison, filter-env swell |
| 37 | DS Choir | `choir` | choir: wide saw + formanty, slow vowel drift |
| 38 | DS Cello | `cello` | cello: warm saw, bowed attack, dark filter |
| 39 | DS Vibes | `vibes` | vibraphone: modal 1:4:10, long ring |
| 40 | DS Marimba | `marimba` | marimba: wooden bars, short warm ring |
| 41 | DS Bell | `bell` | bell/celesta: inharmonic partials, very long ring |
| 42 | DS Hoover | `hoover` | hoover/rave: 6 detuned saws, pitch dive, drive |
| 43 | DS 808 Bass | `bass808` | 808: long sine tail, pitch drop, glide |
| 44 | DS Upright | `upright` | upright bass: plucked triangle, finger tick |
| 45 | DS Drone | `drone` | drone: huge slow attack, detuned saws, slow filter |
| 46 | DS Open Hat | `openhat` | open hat: metal + noise, long decay |
| 47 | DS Ride | `ride` | ride: metal cluster, ping + long wash |
| 48 | DS Cowbell | `cowbell` | cowbell: inharmonic square pair, band-pass |
| 49 | DS Conga | `conga` | conga: tuned membrane, short pitch drop |
| 50 | DS Glitch | `glitch` | glitch: noisy FM blip, resonant BP, hard drive |

**Grupy w Studio** (`core.GROUPS` / `INST_GROUPS`):

- **Perkusja:** kick, snare, hat, clap, rim, tom, crash, shaker, openhat, ride, cowbell, conga
- **Bas:** sub, wobble, reese, growl, neuro, fmbass, hollow, acid, bass808, upright
- **Synth / lead:** screech, supersaw, pluck, pad, stab, vox, piano, strings, hoover, epiano, organ, harpsichord, brass, choir, cello, drone
- **FX:** riser, boom, zap, glitch
- **Mowa:** speak
- **Akustyczne:** theremin, panflute, handpan, tongue, flute, vibes, marimba, bell

Część typów jest **opcjonalna** (`OPTIONAL` w `core.py`): brak pliku `.clap` nie blokuje startu aplikacji (piano/acid/strings i nowsze z v6).

## Budowa (Linux)

```
make            # build/DS-*.clap (51) + build/DubstepKit.clap + build/dsrender + build/dstypes
make test       # mini-host: ładuje, gra nuty, sprawdza NaN/peak, zapisuje demo/*.wav
make install    # kopiuje DS-*.clap do ~/.clap/
```

Bitwig: Settings → Locations → Plug-in locations → dodaj `~/.clap` (jeśli nie ma), przeskanuj.
**Nie instaluj naraz** `DubstepKit.clap` i `DS-*.clap` (duplikaty ID).

## Dodanie nowego brzmienia

1. Dopisz `TypeDef` w `types()` w `src/ds_engine.h` (`ex` = parametry widoczne + defaulty, `hid` = stałe ukryte).
2. Dopisz nazwę na końcu `NAMES` w `Makefile` (kolejność = indeks typu).
3. Dopisz typ do `TYPES` / `GROUPS` / tabel generatora w `dubstep_studio/core.py` (i w razie potrzeby `generator.py`, style).
4. `make` — kompiliuje nową wtyczkę i `DubstepKit.clap`.

Albo użyj zakładki **Modelowanie CLAP** w GUI (projekt → odsłuch → zatwierdź do biblioteki; generuje `user_types.mk` i przebudowuje tylko nowy typ).

---

# DS Dubstep Studio (generator + host + biblioteka + auto-remix + Live)

Aplikacja w Pythonie (PyQt6), która układa utwór na wielu ścieżkach i renderuje go **przez wtyczki CLAP**
(host: `build/dsrender` — offline-host C++ z automatyzacją parametrów, dokładnymi nutami i tempem hosta dla LFO sync).

## Start

```
make                      # wtyczki + host
pip install PyQt6 numpy   # + ffmpeg (MP3) i odtwarzacz: pw-play/paplay/aplay/ffplay/mpv
./ds-studio               # GUI
```

CLI: `./ds-studio gen --style Riddim --bars 96 --seed 7 --save`, `./ds-studio list`,
`./ds-studio remix ID_MASTER ID_B ID_C --bpm 142 --key 3`.
Biblioteka: `~/DubstepStudio/library/` (`DS_LIBRARY`).
Instrumenty: `--instruments all|core8|style|kick,sub,pad`.
Wtyczki szukane w `build/`, `~/.clap`, `$DS_PLUGIN_DIR`.

## Style

**Dubstep / EDM (generator):** Brostep, Riddim, Tearout, Melodic, Deep, Dubstep, Drumstep, Trap, EDM.

**Rozszerzone (`styles.py`):** Techno, Acid Techno, Hard Techno, Techno Trax, Tunnel Trance, Trance, Psytrance, House, Electro, DnB, Liquid DnB, Neurofunk, Space Electronica, Berlin School, Baroque, Romantic, Minimal Classical, Jazz, Jazz Rock, Experimental, Raw Synthesis.

**Hybrydy:** nazwa w formie `Techno 60% + DnB 40%` — konfiguracja składana deterministycznie ze składników (działa w projektach, remiksie, Live).

## Zakładki (skrót)

* **Generator** — drzewo instrumentów (51 typów; domyślnie paleta stylu = core + extras; „Wszystkie”), styl / hybryda, BPM, tonacja + skala, długość, energia, gęstość, swing, seed, smart-instruments, aranżer, reguły użytkownika. Generuje strukturę (intro/build/drop/break/outro), progresję, wzory ścieżek i brzmienia. Podgląd z cache per ścieżka. „Zapisz” → `project.dsproj`, `track.mid`, `track.mp3` (lub wav), opcjonalnie `stems/`.
* **Ścieżki / parametry** — dodaj/usuń instancje (także wiele kopii tego samego typu), mute/solo/gain, suwaki parametrów, losuj brzmienie / wzór nut.
* **Linki** — mapowanie parametrów między ścieżkami (źródła: kick/snare/hat, energy, build, bar_phase, bass_note…).
* **Auto-remix** — miks ścieżek z kilku projektów biblioteki.
* **Live / Endless** — nieskończony strumień (patrz niżej).
* **Modelowanie CLAP** — własne typy na silniku DS bez wychodzenia z GUI (odsłuch tymczasowy → zatwierdź → `make` tylko nowego typu).
* **Normalizacja / Dynamics** — profile limitera / normalizera 3-pasmowego (domyślny / per grupa / per typ).
* **Reguły użytkownika** — poziom mowy (0–3), własne grupy wtyczek, tryb bezwzględny.
* **Edytor WYSIWYG** — geometria efektów aranżera.

Szczegóły nowszych kart i opcji: **[CHANGES.md](CHANGES.md)**.

## Dobór instrumentów

* `default_instruments(style)` = `core` + `extras` stylu.
* `--smart-instruments` / checkbox: wagowy dobór z puli stylu (perkusja, bas, lead, FX, akustyczne) z bilansem pasm i klarownością.
* `--instruments all|core8|style|lista` — jawna lista.
* `speak` nie wchodzi do domyślnych palet; dokłada się przez tekst/motyw, „Dodaj instancję” lub include.

## DS Speak (synteza mowy)

Opcjonalna wtyczka `#24`. W generatorze panel „Mowa” po zaznaczeniu `speak`.
*Auto* — frazy z motywu (Dubstep/Hype/Dark/Countdown); *Ręczny* — własny tekst.
CLI: `--speak-text "…"`, `--speak-theme Hype`, `--instruments …,speak`.
Tekst w projekcie (`text`, `text_mode`); zmiana unieważnia cache ścieżki.
Ograniczenia: tylko angielski; G2P regułowy (`LEX` w `ds_speech.h`); głos formantowy, nie TTS neuronowy.

**Jak to działa (skrót):** analiza słowa (leksykon + G2P) → skala liter → morphing formantów między segmentami → Hold Vowel na dłuższych nutach. Parametry grupy Speech: Text Advance, Text Bank, Speech Speed, Morph Time, Dialect, Letter Pitch Range, Intonation, Pitch Scale, Formant Shift, Hold Vowel, Breath.

**Skąd tekst:** stan projektu → `$DS_SPEAK_TEXT_FILE` / `~/.config/ds-speak.txt` / `~/DubstepStudio/speak.txt` → domyślnie „drop the bass”. Text Bank przełącza wbudowane teksty (Dubstep / Hype / Countdown).

## Routing i edycja nut (v4+)

* **pre — „Nuty z”**: nuty z innej ścieżki (+ transpozycja).
* **post — „Wejście audio z”** + Input Mix + ukryj źródło: instrument jako filtr (port „In” w CLAP).
* **Edycja grupy nut**: zaznaczenie okna taktów → transpozycja, skala, humanizacja, shuffle, stretch itd. (`noteops.py`), z cofaniem.

## Live / Endless

Producent generuje i renderuje kolejną aranżację z wyprzedzeniem; odtwarzacz pcha PCM do `pw-cat` / `paplay` / `aplay` / `ffplay` / `mpv`. Render ~9× realtime.
Magazyn: `~/DubstepStudio/live/<data>/` (`arr_NNNN.wav` + `.dsproj`); limit magazynu usuwa najstarsze nieprzypięte.

**Tryb auto:** styl z Generatora *albo* kaskada pora dnia → styl → wariacja → BPM → energia/gęstość…; „Na sztywno” blokuje wybrane pola.
**Losuj nowe hybrydy:** świeże 2–3 style na każdą aranżację (`[live] new_hybrids = yes`).

### Icecast2 + RTMP + wideo

* Icecast2 — jak wcześniej (`[icecast]` w `config.txt`).
* **RTMP** — pełnoprawne wyjście obok Icecast (oba naraz możliwe): zakładka „RTMP + wideo” lub `[rtmp]` / `[video]`.
* Lekkie wideo reagujące na dźwięk (`vid/livefx.py`): sceny morph / ring / tunnel / bars / scope, AutoTune jakości (CPU/RAM/klatki), x264 ultrafast lub kodek sprzętowy.
* CLI: `./ds-studio live --rtmp [--rtmp-url URL --rtmp-key KEY] [--no-local]`.

Pełny opis: **[CHANGES.md](CHANGES.md)** (sekcja RTMP).

### config.txt

Sekcje m.in. `[generator]`, `[live]`, `[icecast]`, `[rtmp]`, `[video]`, `[paths]`.
Przykłady i komentarze w dołączonym `config.txt`.

### SBC / mało RAM

Render oszczędny, `render_workers`, `max_store`, `lookahead = 1`, `keep_sessions`, `clean_on_exit`.
Na 512 MB: mało instrumentów, krótkie aranżacje, 1–2 workery. Magazyn sesji i `mallopt` ograniczają narastanie pamięci przy długim nadawaniu.

### Tryb ECO: preroll + air

```
python3 -m dubstep_studio preroll --fill 20 --keep 40 [--preset NAZWA] [--format mp3|flac|wav]
python3 -m dubstep_studio air [--shuffle] [--consume] [--no-local]
```

* **preroll** — offline render do plików (te same reguły Live), niski priorytet.
* **air** — nadawanie z katalogu do Icecast / lokalnie, bez generatora i wtyczek (~35 MB RAM).

## Testy

```
python3 -m unittest discover -s tests -v
# m.in. test_arranger, test_fixes, test_user_rules, test_live_*, test_style_variants, …
```

---

Historia wersji, poprawki i szczegóły implementacji → **[CHANGES.md](CHANGES.md)**.

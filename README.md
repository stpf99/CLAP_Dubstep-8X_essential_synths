https://github.com/stpf99/CLAP_Dubstep-8X_essential_synths/blob/5fe307bc62167668da7cae9bb96e5ce442736005/DS-Dubstep-Studio-v8-final.zip

# DS Dubstep Kit — 29 wtyczek CLAP (Bitwig, bez GUI)

Jeden silnik (`src/ds_engine.h`, mowa: `src/ds_speech.h`), wtyczki w `src/ds_plugin.cpp`. Bitwig rysuje generyczne suwaki z parametrów.

| # | Wtyczka | Rola | Cecha charakterystyczna |
|---|---------|------|-------------------------|
| 1 | DS Sub     | sub bas (sinus, mono)        | glide, warstwa sub -1 oct, miękki drive |
| 2 | DS Wobble  | wobble bas                   | LFO filtra zsynchronizowane do tempa (2/1 … 1/32, trioly, kropkowane), 5 kształtów LFO |
| 3 | DS Reese   | reese / bas rozstrojony      | unison do 7 pił, detune do 100 ct, wolny ruch filtra |
| 4 | DS Growl   | growl / yoi                  | FM + filtr formantowy A-E-I-O-U modulowany LFO |
| 5 | DS Screech | lead / screech               | FM + unison + scoop pitch + obwiednia filtra |
| 6 | DS Kick    | stopa                        | pitch envelope, klik szumowy, drive |
| 7 | DS Snare   | werbel / clap                | korpus + szum HP, 1–5 burstów (clap) |
| 8 | DS Hat     | hat / perkusjonalia          | 6 oscylatorów "metal" (808) + szum, HP, choke (mono) |

**v2: 16 dodatkowych instrumentów (łącznie 24, ten sam silnik):**
Perkusja: DS Clap (szum z burstami HP+LP), DS Rim, DS Tom (keytrack, do fillów), DS Crash (metal + długi szum), DS Shaker.
Bas: DS Neuro (rozstrojone piły + formanty + FM, szybkie LFO), DS FM Bass (LFO na indeksie FM), DS Hollow (pulse-width + wobble).
Synth/lead: DS Supersaw (7 pił, szeroki), DS Pluck, DS Pad, DS Stab, DS Vox (formanty samogłosek, „chop” bez sampli).
FX: DS Riser (rampa pitch/cutoff/level), DS Boom (impact), DS Zap (laserowy zjazd).
Nowe parametry silnika (dopisane na końcu, ID starych bez zmian): Tone Level, Noise Lowpass, Ramp Time/Cutoff/Pitch/Level, Pulse Width.
Pluck/Stab/Vox zawsze retriggerują obwiednię (bez legato).

Wspólne: 6 kształtów fali (Sine, Triangle, Saw, Square, Noise, Metal) z płynnym morphem do kolejnego kształtu,
ADSR (czasy logarytmiczne), glide, tune/fine, unison, wygładzanie wszystkich parametrów (5 ms, bez „zipper noise"),
zapis stanu w projekcie Bitwiga. Wszystkie wtyczki są jednogłosowe (mono, legato dla basów; perkusja zawsze retrigger/choke).
Perkusja ma Key Track = 0 (nuta nie zmienia wysokości; podnieś Key Track, żeby grać nią melodycznie).
Poza zakresem (jak chciałeś): EQ, kompresja, reverb, delay — zewnętrznymi wtyczkami. Filtr w silniku jest tylko
„charakterystyczny" (wobble/growl/reese/screech/hat).

## Budowa (Linux)
    make            # build/DS-*.clap (8 osobnych) + build/DubstepKit.clap (wszystkie 8 w jednym pliku)
    make test       # mini-host: ładuje, gra nuty, sprawdza NaN/peak, zapisuje demo/*.wav
    make install    # kopiuje DS-*.clap do ~/.clap/
Bitwig: Settings → Locations → Plug-in locations → dodaj ~/.clap (jeśli nie ma), przeskanuj.
Nie instaluj naraz DubstepKit.clap i DS-*.clap (duplikaty ID).

## Dodanie nowego brzmienia
Dopisz wpis w `types()` w `ds_engine.h` (lista `ex` = parametry widoczne + defaulty, `hid` = stałe ukryte)
i podnieś `COUNT` w `ds_plugin.cpp`.

---

# DS Dubstep Studio (generator + host + biblioteka + auto-remix)

Aplikacja w Pythonie (PyQt6), która sama układa dubstep na 8 ścieżkach i renderuje go **przez te 8 wtyczek CLAP**
(hostowanie robi `build/dsrender` — mały offline-host w C++ z automatyzacją parametrów, próbkowo dokładnymi nutami i tempem hosta dla LFO sync).

## Start
    make                      # wtyczki + host (build/dsrender)
    pip install PyQt6 numpy   # + ffmpeg w systemie (MP3) i dowolny odtwarzacz: pw-play/paplay/aplay/ffplay/mpv
    ./ds-studio               # GUI
CLI: `./ds-studio gen --style Riddim --bars 96 --seed 7 --save`, `./ds-studio list`, `./ds-studio remix ID_MASTER ID_B ID_C --bpm 142 --key 3`.
Biblioteka: `~/DubstepStudio/library/` (zmień przez `DS_LIBRARY`); CLI: `gen --instruments all|core8|style|kick,sub,pad`. Wtyczki szukane w `build/`, `~/.clap`, `$DS_PLUGIN_DIR`.

## Zakładki
* **Generator** — wybór instrumentów (drzewo 24 wtyczek; domyślnie paleta stylu: 8 podstawowych + 6–10 dodatkowych; „Wszystkie 24”), styl (Brostep 140 / Riddim 140 / Tearout 150 / Melodic 140 / Deep 140), BPM, tonacja + skala (minor/phrygian/harmonic minor),
  długość w taktach, energia, gęstość, swing, seed. Generuje strukturę (intro/build/drop/break/outro), progresję akordów, wzory 8 ścieżek
  i losowe brzmienia. Podgląd renderuje ścieżki równolegle (cache: zmiana jednej ścieżki = render tylko jej). „Zapisz” → folder w bibliotece:
  `project.dsproj` (JSON, wszystko edytowalne), `track.mid` (8 ścieżek/kanałów), `track.mp3` (lub wav bez ffmpeg), opcjonalnie `stems/`.
* **Ścieżki / parametry** — „＋ Dodaj instancję” (dowolna z 24 wtyczek, także kilka kopii tej samej, np. wobble2) i „－ Usuń ścieżkę”; mute/solo/gain, suwaki wszystkich parametrów danej wtyczki, „losuj brzmienie / wzór nut” per ścieżka.
* **Linki** — przekazywanie parametrów między ścieżkami: źródło (`kick`/`snare`/`hat` = akcent z obwiednią release, `energy`, `build`, `bar_phase`,
  `bass_note`) → dowolny parametr dowolnej ścieżki z ilością −1…1. Domyślnie m.in. kick→Level basów (sidechain), build→Cutoff screecha,
  snare→FM screecha, bass_note→Vowel growla, energy→Cutoff reese. Dodatkowo zmiany LFO Division (stepped) są automatyzowane per sekcja.
* **Biblioteka** — lista, odtwarzanie, wczytanie do edytora, usuwanie, „→ Do auto-remixu”.
* **Auto-remix** — wiersze = wszystkie instrumenty/instancje z zaznaczonych utworów; dla każdego wskazujesz źródło albo „wył.” (albo „Auto-remix” losuje). Master daje strukturę,
  progresję i linki; nuty/brzmienie biorę ze źródeł **sekcja po sekcji** (drop z dropem, build z buildem; zapętlane/przycinane do długości mastera),
  a ścieżki melodyczne są przesuwane takt po takcie do akordów mastera (opcja). Można nadpisać BPM i tonację. Wynik trafia do biblioteki z listą rodziców.

## Ograniczenia
* Harmonia w remiksie: przesunięcie jest po prymie akordu, więc przy innej jakości akordu (moll↔dur) możliwy półton kolizji.
* Podgląd/render offline (nie live); 144 takty renderują się w kilkanaście sekund.
* GUI testowane w trybie offscreen; odtwarzanie przez zewnętrzny odtwarzacz.


---

# DS Speak — angielska synteza mowy (wtyczka #25, opcjonalna)

Litery → fonemy → formanty F1–F3 na oscylatorach (te same przebiegi/unison/filtr co w reszcie kitu). **Nie jest w żadnej palecie domyślnej** —
zaznaczasz ją ręcznie (drzewo instrumentów, „Wszystkie”, `--instruments ...,speak` albo „Dodaj instancję”).

**Jak to działa**
1. *Analiza słowa* (`ds_speech.h`): leksykon wyjątków (słownictwo dubstepowe) + reguły G2P (dwuznaki, ciche e, długie/krótkie samogłoski, -ed, -tion…),
   akcent sylaby, redukcja samogłosek niepodkreślonych, kontur intonacji słowa (`!`/WIELKIE = okrzyk, `?` = pytanie). Wynik to lista segmentów z już „dopasowanymi” formantami, czasem i wysokością.
2. *Skala liter*: każda litera/fonem ma pozycję −1…1 na skali wysokości (naturalna wysokość samogłoski: i/u wyżej, a/o niżej). **Letter Pitch Range** rozkłada je w zakresie
   półtonów wokół wysokości nuty, **Pitch Scale** kwantyzuje do skali (moll/dur/frygijska/pentatonika), **Intonation** dodaje kontur słowa.
3. *Morphing*: między segmentami formanty, głos, szum i wysokość płynnie przechodzą (**Morph Time** = koartykulacja). Zwarcia/aspiracja przejmują formanty następnej samogłoski.
4. **Hold Vowel**: przy dłuższej nucie samogłoska akcentowana jest trzymana do note-off, potem słowo dośpiewuje końcówkę.

**Parametry (grupa Speech):** Text Advance (Sequence = nuta czyta kolejne słowo · Key Select = klawisz wybiera słowo · Letters = nuta = kolejny fonem, „litery jako oscylator”), Text Bank, Speech Speed,
Morph Time, Dialect (Neutral / Bright / Dark / Tonal / Robot — profil formantów, zakresu liter, intonacji, tempa i skali), Letter Pitch Range, Intonation, Pitch Scale, Formant Shift, Hold Vowel, Breath (szept).

**Skąd tekst:** stan projektu (zapisywany razem z wtyczką) → plik `$DS_SPEAK_TEXT_FILE`, `~/.config/ds-speak.txt` lub `~/DubstepStudio/speak.txt` (Bitwig nie ma GUI wtyczki; plik wczytywany przy aktywacji) → domyślnie „drop the bass”.
Parametr **Text Bank** przełącza też wbudowane teksty (Dubstep / Hype / Countdown) bez pliku.

**W generatorze:** panel „Mowa (DS Speak)” aktywny tylko po zaznaczeniu `speak`. *Auto* — generator układa frazy z motywu (Dubstep/Hype/Dark/Countdown, domyślnie wg stylu) na tyle słów, ile nut;
*Ręczny* — własny tekst czytany cyklicznie. Nuty: pickupy w buildach, okrzyk na początku dropa, melodyczne frazy w breakach. W zakładce „Ścieżki” ścieżka speak ma edytor tekstu (Zastosuj / Auto-tekst).
CLI: `./ds-studio gen --style Riddim --speak-text "Drop the bass! Hands up?"` (podanie tekstu/motywu dodaje speak) lub `--instruments kick,sub,speak --speak-theme Hype`.
Tekst jest częścią projektu (`text`, `text_mode`) i wędruje do remiksów; zmiana tekstu unieważnia cache renderu tej ścieżki.

**Ograniczenia:** tylko angielski; G2P regułowy (nieregularne słowa dopisz do `LEX` w `ds_speech.h`); głos syntetyczny/„robotyczny” — to formantowy syntezator, nie TTS neuronowy.


---

# v4: instrumenty akustyczne, routing, edycja grup nut, nowe style

**Nowe wtyczki (opcjonalne, nigdy w paletach domyślnych):** DS Theremin (sinus/trójkąt, długi glide, vibrato, formant „oo”), DS Pan Flute (ton + szum oddechu śledzący wysokość: *Noise Key Track*),
DS Handpan (synteza modalna, tony własne 1 : 2 : 3, *Ring Time*, *Strike Brightness*), DS Tongue Drum (tony 1 : 2,76 : 5,4 : 8,9, krótszy ring). Handpan i tongue to perkusjonalia (ignorują note-off, brzmią do końca).

**Instrument jako filtr (wejście audio):** każda wtyczka ma opcjonalny port audio „In” i parametr **Input Mix**. Gdy > 0, zewnętrzny dźwięk zastępuje/miesza źródło i przechodzi przez
filtr, formanty i obwiednię nut tego instrumentu (np. DS Speak „mówi” basem wobble; DS Theremin/Pan Flute formują wejście swoim filtrem). Nuty bramkują dźwięk. Uwaga: w DAW port wejściowy jest nieobowiązkowy.

**Routing w Studio (zakładka Ścieżki):**
* **pre — „Nuty z”**: ścieżka gra nuty innej ścieżki (+ transpozycja), np. speak czyta melodię theremina, więc inaczej brzmi niż z własnym wzorem. Łańcuchy są dozwolone.
* **post — „Wejście audio z”** + mix + „ukryj źródło w miksie”: render idzie poziomami (źródła przed odbiorcami, równolegle w poziomie), cykle są rozcinane. Cache uwzględnia cały łańcuch.
* Routing zapisuje się w projekcie (`notes_from`, `notes_transpose`, `input_from`, `input_mix`, `input_hide_source`) i wędruje do remiksów, jeśli źródło jest w remiksie. MIDI eksportuje nuty po routingu.

**Edycja grupy nut:** przeciągnij myszą po przeglądzie aranżacji (zakładka Generator), by zaznaczyć okno taktów, albo wpisz takty „od–do” w zakładce Ścieżki. Operacje (`noteops.py`):
transpozycja, oktawa góra/dół, w górę/dół po stopniach skali, przyciągnij do skali (projektu, moll, dur, frygijska, dorycka, pentatoniki, blues, chromatyczna), losowe wysokości w skali,
odwrócenie w czasie (backward), lustro wysokości, wysokości rosnąco/malejąco (up/down), przetasowanie wysokości / rytmu / wszystkiego, rotacja, rozciągnięcie, humanizacja, przerzedzenie, powtórzenie okna. Jest „Cofnij”.

**Nowe style:** Dubstep (140, klasyczny halftime), Drumstep (174, werbel na 2 i 4, szybkie frazy), Trap (140, hi-haty z rollami 1/32, 808 podążający za stopą z glide), EDM (128, kick four-on-the-floor, clap na 2 i 4, mocny pumping sidechain).
Generatory partii dla nowych instrumentów: theremin (długie frazy w breakach/buildach), flet (pentatoniczne frazy), handpan (arpeggia 1/8), tongue drum (rzadkie pentatoniczne uderzenia).


---

# v5: Live / Endless (zakładka „Live / Endless")

Nieskończony strumień **bez wstępnego renderowania całości**: producent generuje i renderuje (przez wtyczki CLAP, `dsrender`) kolejną aranżację z wyprzedzeniem
(domyślnie 1 aranżacja), a odtwarzacz wysyła PCM prosto do `pw-cat` / `paplay` / `aplay` / `ffplay` / `mpv` przez rurę. Render jest ok. 9× szybszy niż czas rzeczywisty,
pierwszy dźwięk po kilku sekundach. Ogon poprzedniej aranżacji nakłada się na początek następnej. `DS_LIVE_PLAYER=<nazwa>` wymusza odtwarzacz.

**Magazyn** (`~/DubstepStudio/live/<data>/`): każda aranżacja = `arr_NNNN.wav` + `arr_NNNN.dsproj`. Limit „Magazyn (max aranżacji)" usuwa najstarsze **nieprzypięte** (blok na osi robi się szary);
📌 przypięte nie są nigdy usuwane.

**Oś czasu**: blok = aranżacja (kolor stylu, pasek sekcji intro/build/drop/break/outro, BPM, tonacja, pora). Klik = zaznacz (Ctrl/Shift = wiele), **dwuklik albo „Odsłuchaj od startu" = odsłuch od punktu startowego**
aranżacji; po jej końcu strumień wraca na żywo (opcja „Po odsłuchu z osi wróć na żywo", albo gra dalej po kolei). „Wróć na żywo", „Następna teraz", pauza. Dalej: wczytaj do edytora, zapisz do biblioteki (bez ponownego renderu).

**Tryb auto** (dotyczy kolejnych, jeszcze niewygenerowanych aranżacji — zmiany działają od następnej):
* *Styl aktualnie ustawiony* — styl i ustawienia (BPM, tonacja, skala, energia, gęstość, swing, instrumenty, tekst Speak) z zakładki Generator; zmienia się tylko seed (i tonacja przy „auto").
* *Losowanie kompletne, kaskadowo*: **pora dnia → styl** (ważony: wolne pory częściej Deep/Melodic, szybkie Tearout/Drumstep/EDM) **→ wariacja** (podzbiór palety stylu, czasem instrument akustyczny, skala, tonacja)
  **→ BPM** (BPM stylu × rozrzut × mnożnik pory dnia, widełki 0,80–1,08) **→ energia/gęstość/swing/długość**. Mnożniki pór (Rano 88 %, Popołudnie 100 %, Wieczór 106 %, Noc 90 %) edytujesz w panelu; pora = zegar systemowy albo wymuszona.
  **„Na sztywno z Generatora"**: zaznaczone pola (styl, BPM, tonacja, skala, energia, gęstość, swing, długość, instrumenty) nigdy nie są zmieniane przez losowanie.

**Zrzut MP3**: zaznaczone aranżacje / cały magazyn / ostatnie N minut / **auto: losowe N aranżacji z magazynu** (w kolejności chronologicznej); sklejane z nałożonymi ogonami.

Ograniczenia: min. długość aranżacji = 4 takty × liczba sekcji szablonu stylu (np. Tearout 32); przy zmianie BPM między aranżacjami przejście jest cięciem z nałożonym ogonem (bez beat-matchingu);
głośność normalizowana osobno dla każdej aranżacji; pozycja playheada ma błąd ok. ±0,3 s (bufor odtwarzacza).


---

# v6: config.txt + nadawanie do Icecast2

## config.txt
Plik INI (`[sekcje]`, `klucz = wartość`, komentarze `#` w osobnej linii — hasło może zawierać `#`). Szukany: `$DS_CONFIG` → `./config.txt` (obok `ds-studio`) → `~/DubstepStudio/config.txt`;
jeśli go nie ma, powstaje domyślny z opisem każdego klucza (chmod 600, bo są w nim hasła). Sekcje:
* `[icecast]` — `enabled`, `host`, `port`, `mount`, `source_user`, `password`, `tls`, `legacy`, `format` (mp3 | ogg | opus), `bitrate`, `name`, `description`, `genre`, `url`, `public`,
  `metadata` + `admin_user`/`admin_password` (tytuł utworu), `local_playback` (graj też lokalnie), `reconnect_sec`.
* `[live]` — `mode` (current | random), `arr_bars`, `lookahead`, `max_store`, `return_live`, `tod_on`, `tod_force`, `tod_rano/popoludnie/wieczor/noc`, `lock` (lista pól „na sztywno"), `auto_start`.
* `[generator]` — baza dla trybu bez GUI (`style`, `bpm`, `key`, `scale`, `energy`, `density`, `swing`, `instruments`, `speak_text`). W GUI baza to zakładka Generator; po starcie jest do niej wczytywana.
* `[paths]` — `library`, `plugin_dir` (odpowiedniki `DS_LIBRARY`, `DS_PLUGIN_DIR`; zmienna środowiskowa ma pierwszeństwo).

W GUI (Live → zakładka **Icecast2 / config.txt**): „Zapisz config.txt" (podmienia tylko wartości, komentarze zostają; zapisuje też bieżące ustawienia zakładki Generator do `[generator]`),
„Przeładuj", „Otwórz". Ustawienia połączenia zmienione w trakcie nadawania są stosowane po ok. 1 s bez edycji (nadawanie łączy się ponownie, odsłuch lokalny nie przerywa).

## Icecast2
PCM z silnika Live → `ffmpeg` (libmp3lame / libvorbis / libopus) → `icecast://źródło:hasło@host:port/mount`. Wymaga `ffmpeg` i działającego serwera Icecast2 (hasło źródła = `<source-password>` z `icecast.xml`).
* Własny wątek i kolejka: zerwane łącze lub wolny serwer nie blokują odsłuchu lokalnego; po błędzie (np. 401 złe hasło, restart serwera) ponawia co `reconnect_sec`, a status w GUI pokazuje powód.
* W pauzie i przy oczekiwaniu na render wysyłana jest cisza, żeby serwer nie zerwał źródła (`source-timeout`). Przy `local_playback = no` strumień taktuje zegar ścienny.
* Tytuł utworu („Styl BPM tonacja [DS Live #n]") przez `/admin/metadata` — tylko MP3 i tylko gdy podasz `admin_password`; Ogg/Opus niosą metadane w strumieniu.
* Skok na osi czasu opróżnia bufory: słuchacze Icecast słyszą wybraną aranżację bez kolejki zaległego dźwięku.

## Tryb bez GUI (serwer)
    ./ds-studio live [--config plik] [--random] [--no-local]
Czyta config.txt, generuje i nadaje do Icecast2 bez PyQt6 i bez ekranu. `SIGHUP` przeładowuje config.txt bez przerywania strumienia, `Ctrl-C`/`SIGTERM` kończy. Przykład usługi użytkownika:

    # ~/.config/systemd/user/ds-live.service
    [Service]
    ExecStart=%h/dskit/ds-studio live --no-local
    ExecReload=/bin/kill -HUP $MAINPID
    Restart=on-failure
    [Install]
    WantedBy=default.target

Uwaga: przy `local_playback = no` i wyłączonym Icecast nie ma dokąd grać — wtedy silnik gra lokalnie.

## Style, pole szachowe (hybrydy) i presety

**30 stylów** (`styles.py`): dubstepowe (Brostep, Riddim, Tearout, Melodic, Deep, Dubstep, Drumstep, Trap, EDM), techno (Techno, Acid Techno, Hard Techno, Techno Trax), trance (Tunnel Trance, Trance, Psytrance), House, Electro, DnB (DnB, Liquid, Neurofunk), Space Electronica, Berlin School, klasyka (Baroque, Romantic, Minimal Classical), Jazz, Jazz Rock, Experimental, Raw Synthesis. Style mają własne rytmy (4/4, ride, swing8, ghost snare), tryby suba (walk, funk, offbeat…), arpeggia, gate, comping oraz **wielogłosowość** (kilka instancji pad/stab/supersaw = akord).

**Pole stylów (hybrydy)** — zakładka obok Generatora: lewa oś = *baza* (rytm, struktura, tempo), górna = *mutacja*. Klik w komórkę: siła 1→2→3→wył., prawy klik czyści, klik w nazwę = czysty styl. Suwak ustawia siłę mutacji. Wiele komórek = styl multihybrydowy (np. `Techno 67% + DnB 17% + Jazz 8%`). „Zastosuj do Generatora” ustawia go jako styl; w Live: *Użyj pola szachowego* oraz *Mutacje stylu (losowanie)* — szansa, że wylosowany styl zostanie zmutowany.

**Presety** (globalne, domyślnie puste) — pasek na górze Generatora: Wczytaj / Zapisz jako… / Usuń / Eksportuj… / Importuj…. Preset zawiera ustawienia Generatora, pole szachowe i tryb auto Live; **nie** zawiera Icecast (hasła zostają w `config.txt`). Plik: `$DS_PRESETS`, `[paths] presets` albo `~/DubstepStudio/presets.json` (powstaje po zapisaniu pierwszego presetu).

Bez GUI:
```
python3 -m dubstep_studio presets list|show NAZWA|delete NAZWA|export NAZWA PLIK|import PLIK
python3 -m dubstep_studio gen --preset NAZWA --save
python3 -m dubstep_studio gen --style "Techno 60% + DnB 40%"
python3 -m dubstep_studio live --preset NAZWA
```

## SBC z małą ilością RAM (np. La Frite 512 MB) i wybór urządzenia audio

- **Render oszczędny**: Live składa miks na bieżąco (`core.render_mix`) — w pamięci jest jeden miks + stemy aktualnie renderowane, a nie wszystkie naraz (u mnie ~130 MB zamiast ~460 MB szczytu dla 32 taktów; dźwięk identyczny). `[live] render_workers` (albo `DS_WORKERS`) ogranicza liczbę równoległych procesów `dsrender`; `0` = auto z wolnego RAM (na 512 MB wychodzi 1–2).
- Dalsze oszczędności: tryb bez GUI (`python3 -m dubstep_studio live --preset NAZWA`), `arr_bars = 16..24`, mniej instrumentów (`instruments = core8` w `[generator]`), `lookahead = 1`, ewentualnie zram/swap. Sprawdź, czy render aranżacji jest szybszy niż jej odtwarzanie (`time python3 -m dubstep_studio gen --bars 32`) — jeśli nie, zmniejsz liczbę instrumentów.
- **Urządzenie wyjściowe**: `python3 -m dubstep_studio devices` wypisuje, co widzi system; wybrane id wpisz w `[live] audio_device` (aplay: `plughw:1,0` — samo konwertuje format; paplay/pw-cat: nazwa sinka; mpv: `alsa/plughw:1,0`). W GUI: zakładka Live → Odtwarzacz / Urządzenie (zmiana działa od razu). `[live] player = aplay` wymusza ALSA bez PipeWire/PulseAudio; w trybie `auto` pw-cat/paplay są pomijane, gdy serwer dźwięku nie działa. Ustawienie dotyczy też odsłuchu w Generatorze.

### Miejsce na karcie SD
- **Render** używa katalogu tymczasowego `dsr<pid>_*`; stemy są kasowane od razu po wczytaniu, a porzucone katalogi po zabitym procesie (OOM, zanik zasilania) są sprzątane przy starcie Live. `[paths] tmp_dir` (albo `DS_TMP`) przenosi je gdzie indziej.
- **Magazyn sesji Live**: `[live] max_store` (min. 2) ogranicza liczbę przechowywanych wav (ok. 10 MB na 32 takty), `keep_sessions = 1` kasuje foldery starszych sesji przy starcie, `clean_on_exit = yes` kasuje niezapięte wav po zatrzymaniu (zostają małe projekty). `[paths] live_dir` (albo `DS_LIVE_DIR`) przenosi magazyn np. na dysk USB. Utwory zapisane do biblioteki (`--save`, zrzut MP3) nie są ruszane.
- Na SBC polecane: `max_store = 3`, `lookahead = 1`, `keep_sessions = 1`, `clean_on_exit = yes`.

### Długie nadawanie: narastanie pamięci
Przy wielogodzinnym Live pamięć mogła rosnąć z dwóch powodów, oba poprawione: (1) wykluczone z magazynu aranżacje zostawały w RAM-ie jako pełne projekty (~130 kB każdy) — teraz zostaje tylko to, co potrzebne osi czasu; (2) duże bufory audio (~20 MB) fragmentowały stertę glibc — teraz są zawsze mapowane i zwracane systemowi (`mallopt` w `__init__.py`; `DS_NO_MALLOPT=1` wyłącza). W logu trybu bez GUI każda linia `[play]` pokazuje `RAM procesu`; zdrowy przebieg ustala się na stałym poziomie.

## Tryb ECO: preroll + air (bez generowania na żywo)
Dwa osobne, lekkie kroki zamiast generowania i nadawania naraz — dla płytek z małym RAM-em/CPU:

```
python3 -m dubstep_studio preroll --fill 20 --keep 40 [--preset NAZWA] [--format mp3|flac|wav]
python3 -m dubstep_studio air [--shuffle] [--consume] [--no-local]
```

- **preroll** renderuje aranżacje offline do plików (planer Live: te same style, hybrydy, mutacje, pory dnia), po jednej, z niskim priorytetem (`nice 19`). mp3 192 kbps = ok. 1,4 MB na minutę zamiast 10,8 MB wav. `--fill N` dopełnia katalog do N plików (nadaje się do crona/timera), `--count N` renderuje N nowych, `--minutes M` zbiera tyle minut, `--keep N` ogranicza katalog (najstarsze są kasowane). Pliki: `NNNNNN-<styl>-<bpm>bpm.mp3`, katalog: `[paths] preroll_dir` / `--dir` (domyślnie `~/DubstepStudio/preroll`).
- **air** nadaje pliki do Icecast2 (`[icecast]` z config.txt) i/lub odtwarzacza (`[live] player`, `audio_device`) — bez generatora, wtyczek i renderu, w ok. 35 MB RAM. Katalog jest skanowany przy każdym przebiegu, więc nowe pliki z `preroll` wchodzą na antenę; `--consume` kasuje plik po odegraniu (katalog jako kolejka), przy pustym katalogu Icecast dostaje ciszę i `air` czeka na pliki. Tytuł w Icecast = nazwa pliku.
- Typowo: `preroll --fill 20 --keep 20` z crona co godzinę + `air --consume` jako usługa systemd. Render i nadawanie nie muszą już działać jednocześnie w jednym procesie.

# DS Dubstep Kit — 8 wtyczek CLAP (Bitwig, bez GUI)

Jeden silnik (`src/ds_engine.h`), 8 wtyczek (`src/ds_plugin.cpp`). Bitwig rysuje generyczne suwaki z parametrów.

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

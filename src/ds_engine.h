// ds_engine.h — wspólny silnik "DS Dubstep Kit" (CLAP, bez GUI)
// Jeden silnik, 8 typów wtyczek. Każdy typ eksponuje tylko swój podzbiór parametrów
// (reszta = stałe ukryte), a host (Bitwig) rysuje generyczne suwaki.
#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace ds {

// ---------------------------------------------------------------- parametry
enum Param {
  P_WAVE, P_MORPH, P_ATK, P_DEC, P_SUS, P_REL, P_GLIDE, P_TUNE, P_FINE, P_KEYTRK,
  P_UNISON, P_DETUNE, P_WIDTH, P_SUB, P_NOISE, P_NDEC, P_NCUT, P_NBURST,
  P_FM, P_FMRATIO, P_FORM, P_VOWEL,
  P_FTYPE, P_CUTOFF, P_RESO, P_FENV, P_FDEC,
  P_LFO_SYNC, P_LFO_RATE, P_LFO_DIV, P_LFO_SHAPE, P_LFO_CUT, P_LFO_VOW, P_LFO_FM, P_LFO_PIT,
  P_PENV, P_PDEC, P_DRIVE, P_LEVEL,
  NP
};

enum Kind { LIN, LOG, INT };  // LOG: wartość CLAP 0..1, silnik lo*(hi/lo)^v

struct ParamInfo {
  const char* name; const char* group; Kind kind;
  float lo, hi, def; const char* unit; const char* const* names;
};

inline const char* const N_WAVE[]  = {"Sine", "Triangle", "Saw", "Square", "Noise", "Metal"};
inline const char* const N_FTYPE[] = {"Lowpass", "Bandpass", "Highpass"};
inline const char* const N_SYNC[]  = {"Free", "Tempo"};
inline const char* const N_DIV[]   = {"2/1", "1/1", "1/2.", "1/2", "1/4.", "1/4", "1/8.", "1/4T",
                                      "1/8", "1/8T", "1/16", "1/16T", "1/32"};
inline const float DIV_BEATS[13]   = {8, 4, 3, 2, 1.5f, 1, 0.75f, 2.f / 3, 0.5f, 1.f / 3, 0.25f, 1.f / 6, 0.125f};
inline const char* const N_SHAPE[] = {"Sine", "Triangle", "Saw Down", "Square", "Sample&Hold"};

inline const ParamInfo PI[NP] = {
  /*WAVE*/    {"Waveform",        "Oscillator", INT, 0, 5, 2, "", N_WAVE},
  /*MORPH*/   {"Wave Morph",      "Oscillator", LIN, 0, 1, 0, "", nullptr},
  /*ATK*/     {"Attack",          "Amp Env",    LOG, 0.5f, 4000, 3, "ms", nullptr},
  /*DEC*/     {"Decay",           "Amp Env",    LOG, 5, 8000, 300, "ms", nullptr},
  /*SUS*/     {"Sustain",         "Amp Env",    LIN, 0, 1, 0.8f, "", nullptr},
  /*REL*/     {"Release",         "Amp Env",    LOG, 5, 8000, 120, "ms", nullptr},
  /*GLIDE*/   {"Glide",           "Pitch",      LIN, 0, 1000, 0, "ms", nullptr},
  /*TUNE*/    {"Tune",            "Pitch",      LIN, -36, 36, 0, "st", nullptr},
  /*FINE*/    {"Fine",            "Pitch",      LIN, -100, 100, 0, "ct", nullptr},
  /*KEYTRK*/  {"Key Track",       "Pitch",      LIN, 0, 1, 1, "", nullptr},
  /*UNISON*/  {"Unison Voices",   "Oscillator", INT, 1, 7, 1, "", nullptr},
  /*DETUNE*/  {"Unison Detune",   "Oscillator", LIN, 0, 100, 15, "ct", nullptr},
  /*WIDTH*/   {"Stereo Width",    "Oscillator", LIN, 0, 1, 0.5f, "", nullptr},
  /*SUB*/     {"Sub Osc",         "Oscillator", LIN, 0, 1, 0, "", nullptr},
  /*NOISE*/   {"Noise",           "Noise",      LIN, 0, 1, 0, "", nullptr},
  /*NDEC*/    {"Noise Decay",     "Noise",      LOG, 2, 2000, 100, "ms", nullptr},
  /*NCUT*/    {"Noise Highpass",  "Noise",      LOG, 100, 16000, 100, "Hz", nullptr},
  /*NBURST*/  {"Noise Bursts",    "Noise",      INT, 1, 5, 1, "", nullptr},
  /*FM*/      {"FM Amount",       "FM",         LIN, 0, 1, 0, "", nullptr},
  /*FMRATIO*/ {"FM Ratio",        "FM",         LIN, 0.25f, 12, 2, "x", nullptr},
  /*FORM*/    {"Formant Mix",     "Formant",    LIN, 0, 1, 0, "", nullptr},
  /*VOWEL*/   {"Vowel A-E-I-O-U", "Formant",    LIN, 0, 4, 0, "", nullptr},
  /*FTYPE*/   {"Filter Type",     "Filter",     INT, 0, 2, 0, "", N_FTYPE},
  /*CUTOFF*/  {"Cutoff",          "Filter",     LOG, 20, 20000, 20000, "Hz", nullptr},
  /*RESO*/    {"Resonance",       "Filter",     LIN, 0, 1, 0.1f, "", nullptr},
  /*FENV*/    {"Filter Env Amt",  "Filter",     LIN, -6, 6, 0, "oct", nullptr},
  /*FDEC*/    {"Filter Env Decay","Filter",     LOG, 5, 4000, 300, "ms", nullptr},
  /*LFO_SYNC*/{"LFO Sync",        "LFO",        INT, 0, 1, 1, "", N_SYNC},
  /*LFO_RATE*/{"LFO Rate",        "LFO",        LOG, 0.05f, 40, 2, "Hz", nullptr},
  /*LFO_DIV*/ {"LFO Division",    "LFO",        INT, 0, 12, 8, "", N_DIV},
  /*LFO_SHAPE*/{"LFO Shape",      "LFO",        INT, 0, 4, 0, "", N_SHAPE},
  /*LFO_CUT*/ {"LFO > Cutoff",    "LFO",        LIN, -6, 6, 0, "oct", nullptr},
  /*LFO_VOW*/ {"LFO > Vowel",     "LFO",        LIN, -4, 4, 0, "", nullptr},
  /*LFO_FM*/  {"LFO > FM",        "LFO",        LIN, -1, 1, 0, "", nullptr},
  /*LFO_PIT*/ {"LFO > Pitch",     "LFO",        LIN, -12, 12, 0, "st", nullptr},
  /*PENV*/    {"Pitch Env Amt",   "Pitch",      LIN, -24, 60, 0, "st", nullptr},
  /*PDEC*/    {"Pitch Env Decay", "Pitch",      LOG, 3, 800, 40, "ms", nullptr},
  /*DRIVE*/   {"Drive",           "Output",     LIN, 0, 1, 0, "", nullptr},
  /*LEVEL*/   {"Level",           "Output",     LIN, 0, 1, 0.8f, "", nullptr},
};

inline float toEng(int p, double cv) {
  const ParamInfo& pi = PI[p];
  if (pi.kind == LOG) {
    cv = std::min(1.0, std::max(0.0, cv));
    return pi.lo * std::pow(pi.hi / pi.lo, (float)cv);
  }
  float v = (float)std::min((double)pi.hi, std::max((double)pi.lo, cv));
  return pi.kind == INT ? std::round(v) : v;
}
inline double toClap(int p, float ev) {
  const ParamInfo& pi = PI[p];
  if (pi.kind == LOG) {
    ev = std::min(pi.hi, std::max(pi.lo, ev));
    return std::log(ev / pi.lo) / std::log(pi.hi / pi.lo);
  }
  return std::min(pi.hi, std::max(pi.lo, ev));
}

// ---------------------------------------------------------------- definicje wtyczek
struct Init { int p; float v; };
struct TypeDef {
  const char* id; const char* name; const char* desc;
  bool oneshot;      // perkusja: ignoruje note-off, zawsze retrigger (choke)
  float ref;         // częstotliwość odniesienia (Hz) dla nuty 60 przy KEYTRK=1
  std::vector<Init> ex;   // parametry eksponowane (kolejność = kolejność w hoście) + defaulty
  std::vector<Init> hid;  // ukryte stałe
  void defaults(float* out) const {
    for (int i = 0; i < NP; i++) out[i] = PI[i].def;
    for (auto& h : hid) out[h.p] = h.v;
    for (auto& e : ex) out[e.p] = e.v;
  }
};

inline const std::vector<TypeDef>& types() {
  static const std::vector<TypeDef> T = {
    // ---- 0: SUB — czysty podkład sinusoidalny, mono, glide
    {"stpf99.dubstep.sub", "DS Sub", "Sub bass: sine/tri, mono glide, soft drive", false, 261.6256f,
     {{P_WAVE,0},{P_MORPH,0},{P_ATK,3},{P_DEC,200},{P_SUS,1},{P_REL,90},{P_GLIDE,40},{P_TUNE,0},{P_FINE,0},
      {P_SUB,0},{P_CUTOFF,900},{P_DRIVE,0.15f},{P_LEVEL,0.85f}},
     {{P_KEYTRK,1},{P_UNISON,1},{P_RESO,0},{P_FTYPE,0}}},
    // ---- 1: WOBBLE — saw + filtr LP z LFO zsynchronizowanym do tempa
    {"stpf99.dubstep.wobble", "DS Wobble", "Wobble bass: saw unison + tempo-synced LFO filter", false, 261.6256f,
     {{P_WAVE,2},{P_MORPH,0},{P_ATK,2},{P_DEC,400},{P_SUS,1},{P_REL,120},{P_GLIDE,30},{P_TUNE,0},{P_FINE,0},
      {P_UNISON,2},{P_DETUNE,14},{P_WIDTH,0.4f},{P_SUB,0.3f},{P_CUTOFF,450},{P_RESO,0.5f},
      {P_LFO_SYNC,1},{P_LFO_RATE,2},{P_LFO_DIV,8},{P_LFO_SHAPE,0},{P_LFO_CUT,3.5f},{P_DRIVE,0.4f},{P_LEVEL,0.75f}},
     {{P_KEYTRK,1},{P_FTYPE,0},{P_FENV,0}}},
    // ---- 2: REESE — rozstrojone piły, wolny ruch filtra
    {"stpf99.dubstep.reese", "DS Reese", "Reese bass: detuned saws, slow phasing filter", false, 261.6256f,
     {{P_WAVE,2},{P_MORPH,0},{P_ATK,4},{P_DEC,300},{P_SUS,1},{P_REL,150},{P_GLIDE,60},{P_TUNE,0},{P_FINE,0},
      {P_UNISON,3},{P_DETUNE,22},{P_WIDTH,0.45f},{P_SUB,0.35f},{P_CUTOFF,1800},{P_RESO,0.15f},
      {P_LFO_SYNC,0},{P_LFO_RATE,0.25f},{P_LFO_DIV,3},{P_LFO_SHAPE,1},{P_LFO_CUT,1.5f},{P_DRIVE,0.5f},{P_LEVEL,0.7f}},
     {{P_KEYTRK,1},{P_FTYPE,0}}},
    // ---- 3: GROWL — FM + filtr formantowy z LFO (samogłoski)
    {"stpf99.dubstep.growl", "DS Growl", "Growl/yoi: FM + vowel formant filter morphing", false, 261.6256f,
     {{P_WAVE,2},{P_MORPH,0.3f},{P_ATK,2},{P_DEC,300},{P_SUS,1},{P_REL,100},{P_GLIDE,25},{P_TUNE,0},{P_FINE,0},
      {P_SUB,0.25f},{P_FM,0.45f},{P_FMRATIO,2},{P_FORM,0.85f},{P_VOWEL,1.5f},{P_CUTOFF,6000},{P_RESO,0.3f},
      {P_LFO_SYNC,1},{P_LFO_RATE,3},{P_LFO_DIV,8},{P_LFO_SHAPE,1},{P_LFO_CUT,1.0f},{P_LFO_VOW,1.5f},{P_LFO_FM,0.3f},
      {P_DRIVE,0.7f},{P_LEVEL,0.65f}},
     {{P_KEYTRK,1},{P_UNISON,1},{P_FTYPE,0}}},
    // ---- 4: SCREECH — metaliczny lead z FM, scoop pitch, unison
    {"stpf99.dubstep.screech", "DS Screech", "Screech lead: FM saw unison, pitch scoop, filter env", false, 261.6256f,
     {{P_WAVE,2},{P_MORPH,0.2f},{P_ATK,2},{P_DEC,400},{P_SUS,0.7f},{P_REL,150},{P_GLIDE,80},{P_TUNE,0},{P_FINE,0},
      {P_UNISON,5},{P_DETUNE,25},{P_WIDTH,0.8f},{P_FM,0.55f},{P_FMRATIO,3.01f},{P_CUTOFF,7000},{P_RESO,0.3f},
      {P_FENV,2},{P_FDEC,300},{P_LFO_SYNC,0},{P_LFO_RATE,5.5f},{P_LFO_SHAPE,0},{P_LFO_FM,0.2f},{P_LFO_PIT,0.2f},
      {P_PENV,5},{P_PDEC,80},{P_DRIVE,0.45f},{P_LEVEL,0.6f}},
     {{P_KEYTRK,1},{P_FTYPE,0}}},
    // ---- 5: KICK — sinus + pitch envelope + klik szumowy
    {"stpf99.dubstep.kick", "DS Kick", "Kick: sine with pitch envelope, noise click, drive", true, 50.f,
     {{P_WAVE,0},{P_MORPH,0},{P_ATK,0.5f},{P_DEC,280},{P_TUNE,0},{P_FINE,0},{P_KEYTRK,0},{P_PENV,36},{P_PDEC,28},
      {P_NOISE,0.15f},{P_NDEC,4},{P_NCUT,2000},{P_CUTOFF,20000},{P_DRIVE,0.3f},{P_LEVEL,0.9f}},
     {{P_UNISON,1},{P_FTYPE,0},{P_RESO,0},{P_SUB,0}}},
    // ---- 6: SNARE — korpus + szum z burstami (clap)
    {"stpf99.dubstep.snare", "DS Snare", "Snare/clap: body tone + highpassed noise bursts", true, 190.f,
     {{P_WAVE,1},{P_MORPH,0},{P_ATK,0.5f},{P_DEC,140},{P_TUNE,0},{P_FINE,0},{P_KEYTRK,0},{P_PENV,10},{P_PDEC,30},
      {P_NOISE,0.8f},{P_NDEC,180},{P_NCUT,1200},{P_NBURST,1},{P_CUTOFF,14000},{P_DRIVE,0.35f},{P_LEVEL,0.8f}},
     {{P_UNISON,1},{P_FTYPE,0},{P_RESO,0},{P_SUB,0}}},
    // ---- 7: HAT — 6 oscylatorów "metal" (808) + szum, HP
    {"stpf99.dubstep.hat", "DS Hat", "Hat/perc: metallic square cluster + noise, highpass, choke", true, 205.3f,
     {{P_WAVE,5},{P_ATK,0.5f},{P_DEC,55},{P_TUNE,0},{P_KEYTRK,0},{P_NOISE,0.5f},{P_NDEC,45},{P_NCUT,7000},
      {P_FTYPE,2},{P_CUTOFF,6500},{P_RESO,0.25f},{P_DRIVE,0.1f},{P_LEVEL,0.7f}},
     {{P_UNISON,1},{P_MORPH,0},{P_SUB,0},{P_FINE,0}}},
  };
  return T;
}

// ---------------------------------------------------------------- DSP
namespace dsp {
inline float blep(float t, float dt) {
  if (t < dt) { t /= dt; return t + t - t * t - 1.f; }
  if (t > 1.f - dt) { t = (t - 1.f) / dt; return t * t + t + t + 1.f; }
  return 0.f;
}
struct SVFc { float a1, a2, a3, k; };
inline SVFc svfCoef(float fc, float fs, float k) {
  float g = std::tan(3.14159265f * fc / fs);
  float a1 = 1.f / (1.f + g * (g + k));
  return {a1, g * a1, g * g * a1, k};
}
struct SVF {
  float ic1 = 0, ic2 = 0;
  inline void run(float x, const SVFc& c, float& lp, float& bp, float& hp) {
    float v3 = x - ic2;
    float v1 = c.a1 * ic1 + c.a2 * v3;
    float v2 = ic2 + c.a2 * ic1 + c.a3 * v3;
    ic1 = 2.f * v1 - ic1; ic2 = 2.f * v2 - ic2;
    lp = v2; bp = v1; hp = x - c.k * v1 - v2;
  }
  void reset() { ic1 = ic2 = 0; }
};
}  // namespace dsp

class Engine {
 public:
  void init(const TypeDef* t, double sampleRate) {
    T = t; fs = (float)sampleRate;
    smooth = 1.f - std::exp(-1.f / (0.005f * fs));
    float d[NP]; t->defaults(d);
    for (int i = 0; i < NP; i++) tgt[i] = cur[i] = d[i];
    reset();
  }
  void reset() { v = Voice(); stack.clear(); }
  void snap() { for (int i = 0; i < NP; i++) cur[i] = tgt[i]; }
  void setTempo(double b) { bpm = (float)(b > 20 ? b : 140); }
  void setParam(int p, float e) {
    tgt[p] = e;
    if (PI[p].kind == INT) cur[p] = e;
  }
  void noteOn(int key, float vel) {
    stack.erase(std::remove(stack.begin(), stack.end(), key), stack.end());
    stack.push_back(key);
    bool legato = !T->oneshot && v.active && v.gated;
    v.nTgt = (float)key; v.vel = vel;
    if (!legato) {
      if (!v.active || T->oneshot || cur[P_GLIDE] < 0.5f) v.nCur = (float)key;
      if (!v.active) { v.level = 0; for (auto& f : v.flt) f.reset(); for (auto& row : v.frm) for (auto& f : row) f.reset(); v.lpN[0] = v.lpN[1] = 0; }
      if (T->oneshot) v.level = 0;
      if (!v.active || T->oneshot) {
        for (int i = 0; i < 7; i++) { v.ph[i] = i * 0.173f; v.mph[i] = 0; }
        for (int i = 0; i < 6; i++) v.metPh[i] = 0;
        v.subPh = 0;
      }
      v.stage = ATTACK; v.lfoPh = 0; v.sh = rnd();
      v.nenv = 1; v.burstIdx = 0; v.burstTimer = 0;
    }
    v.penv = 1; v.fenv = 1; v.active = true; v.gated = true;
  }
  void noteOff(int key) {
    stack.erase(std::remove(stack.begin(), stack.end(), key), stack.end());
    if (T->oneshot) return;
    if (!stack.empty()) { v.nTgt = (float)stack.back(); return; }
    v.gated = false; if (v.active) v.stage = RELEASE;
  }
  void allOff() { stack.clear(); if (!T->oneshot) { v.gated = false; if (v.active) v.stage = RELEASE; } }

  void render(float* L, float* R, int n) {
    for (int i = 0; i < n; i++) {
      for (int p = 0; p < NP; p++) if (PI[p].kind != INT) cur[p] += (tgt[p] - cur[p]) * smooth;
      if (!v.active) { L[i] = R[i] = 0.f; continue; }
      float o[2]; tick(o); L[i] = o[0]; R[i] = o[1];
    }
  }

 private:
  enum Stage { ATTACK, DECAY, RELEASE };
  struct Voice {
    bool active = false, gated = false;
    int stage = ATTACK;
    float level = 0, vel = 1, nCur = 60, nTgt = 60;
    float ph[7] = {0}, mph[7] = {0}, metPh[6] = {0}, subPh = 0;
    float lfoPh = 0, sh = 0, penv = 0, fenv = 0, nenv = 0;
    int burstIdx = 0, burstTimer = 0;
    dsp::SVF flt[2], frm[2][2];
    float lpN[2] = {0, 0};
  };
  const TypeDef* T = nullptr;
  float fs = 48000, bpm = 140, smooth = 0.01f;
  float tgt[NP] = {0}, cur[NP] = {0};
  Voice v; std::vector<int> stack; uint32_t rng = 0x1234567u;

  float rnd() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return (rng & 0xFFFFFF) * (2.f / 16777216.f) - 1.f; }
  static float lfoShape(int s, float p, float sh) {
    switch (s) {
      case 0: return std::sin(6.2831853f * p);
      case 1: { float q = p + 0.25f; q -= std::floor(q); return 1.f - 4.f * std::fabs(q - 0.5f); }
      case 2: return 1.f - 2.f * p;
      case 3: return p < 0.5f ? 1.f : -1.f;
      default: return sh;
    }
  }
  float waveAt(int idx, float t, float dt, float metal) {
    switch (idx) {
      case 0: return std::sin(6.2831853f * t);
      case 1: return 1.f - 4.f * std::fabs(t - 0.5f);
      case 2: return 2.f * t - 1.f - dsp::blep(t, dt);
      case 3: { float t2 = t + 0.5f; if (t2 >= 1.f) t2 -= 1.f;
                return (t < 0.5f ? 1.f : -1.f) + dsp::blep(t, dt) - dsp::blep(t2, dt); }
      case 4: return rnd();
      default: return metal;
    }
  }
  static float tc(float ms, float fs) { return std::exp(-4.6f / std::max(1.f, ms * 0.001f * fs)); }

  void tick(float* out) {
    // --- glide
    float gl = cur[P_GLIDE];
    if (gl < 0.5f) v.nCur = v.nTgt;
    else v.nCur += (v.nTgt - v.nCur) * (1.f - std::exp(-4000.f / (gl * fs)));

    // --- LFO
    int div = std::min(12, std::max(0, (int)cur[P_LFO_DIV]));
    float rate = cur[P_LFO_SYNC] > 0.5f ? (bpm / 60.f) / DIV_BEATS[div] : cur[P_LFO_RATE];
    v.lfoPh += rate / fs;
    if (v.lfoPh >= 1.f) { v.lfoPh -= std::floor(v.lfoPh); v.sh = rnd(); }
    float lfo = lfoShape((int)cur[P_LFO_SHAPE], v.lfoPh, v.sh);

    // --- pitch
    float semis = cur[P_TUNE] + cur[P_FINE] * 0.01f + cur[P_KEYTRK] * (v.nCur - 60.f)
                + cur[P_LFO_PIT] * lfo + cur[P_PENV] * v.penv;
    float f = std::min(T->ref * std::exp2(semis / 12.f), 0.45f * fs);
    f = std::max(f, 1.f);

    // --- oscylatory
    int wi = (int)cur[P_WAVE], wj = std::min(wi + 1, 5);
    float m = (wi >= 5) ? 0.f : cur[P_MORPH];
    int nu = std::min(7, std::max(1, (int)cur[P_UNISON]));
    float fmAmt = std::min(1.f, std::max(0.f, cur[P_FM] + cur[P_LFO_FM] * lfo));
    float pm = fmAmt * 1.5915f;  // 10 rad / 2pi
    float metal = 0.f;
    if (wi == 5 || (wj == 5 && m > 0.f)) {
      static const float MR[6] = {1.f, 1.483f, 1.8f, 2.546f, 2.63f, 3.897f};
      float s = 0;
      for (int k = 0; k < 6; k++) {
        float dt = std::min(0.45f, f * MR[k] / fs);
        v.metPh[k] += dt; if (v.metPh[k] >= 1.f) v.metPh[k] -= 1.f;
        s += (v.metPh[k] < 0.5f ? 1.f : -1.f) + dsp::blep(v.metPh[k], dt)
           - dsp::blep(v.metPh[k] + 0.5f >= 1.f ? v.metPh[k] - 0.5f : v.metPh[k] + 0.5f, dt);
      }
      metal = s * 0.35f;
    }
    float sl = 0, sr = 0, norm = 1.f / std::sqrt((float)nu);
    for (int u = 0; u < nu; u++) {
      float off = nu > 1 ? (2.f * u / (nu - 1) - 1.f) : 0.f;
      float fu = f * std::exp2(off * cur[P_DETUNE] / 1200.f);
      float dt = std::min(0.45f, fu / fs);
      v.ph[u] += dt; if (v.ph[u] >= 1.f) v.ph[u] -= 1.f;
      float t = v.ph[u];
      if (pm > 0.f) {
        v.mph[u] += std::min(0.45f, fu * cur[P_FMRATIO] / fs); if (v.mph[u] >= 1.f) v.mph[u] -= 1.f;
        t += pm * std::sin(6.2831853f * v.mph[u]); t -= std::floor(t);
      }
      float a = waveAt(wi, t, dt, metal);
      float s = (m > 0.f) ? a + (waveAt(wj, t, dt, metal) - a) * m : a;
      float pan = off * cur[P_WIDTH];
      sl += s * std::sqrt(1.f - pan) ; sr += s * std::sqrt(1.f + pan);
    }
    sl *= norm; sr *= norm;
    if (cur[P_SUB] > 0.001f) {
      v.subPh += 0.5f * f / fs; if (v.subPh >= 1.f) v.subPh -= 1.f;
      float s = std::sin(6.2831853f * v.subPh) * cur[P_SUB] * 1.4142f;
      sl += s; sr += s;
    }
    sl *= 0.7f; sr *= 0.7f;

    // --- filtr
    int ft = (int)cur[P_FTYPE];
    float fcMod = cur[P_FENV] * v.fenv + cur[P_LFO_CUT] * lfo;
    bool bypass = ft == 0 && cur[P_CUTOFF] > 19000.f && std::fabs(cur[P_FENV]) < 0.01f && std::fabs(cur[P_LFO_CUT]) < 0.01f;
    if (!bypass) {
      float fc = std::min(0.45f * fs, std::max(20.f, cur[P_CUTOFF] * std::exp2(fcMod)));
      dsp::SVFc c = dsp::svfCoef(fc, fs, 2.f - 1.96f * cur[P_RESO]);
      float lp, bp, hp, x[2] = {sl, sr};
      for (int ch = 0; ch < 2; ch++) {
        v.flt[ch].run(x[ch], c, lp, bp, hp);
        x[ch] = ft == 0 ? lp : (ft == 1 ? bp * c.k : hp);
      }
      sl = x[0]; sr = x[1];
    }
    // --- formanty
    if (cur[P_FORM] > 0.001f) {
      static const float F1[5] = {730, 530, 270, 570, 300}, F2[5] = {1090, 1840, 2290, 840, 870};
      float vp = std::min(3.999f, std::max(0.f, cur[P_VOWEL] + cur[P_LFO_VOW] * lfo));
      int i0 = (int)vp; float fr = vp - i0;
      float f1 = F1[i0] + (F1[i0 + 1] - F1[i0]) * fr, f2 = F2[i0] + (F2[i0 + 1] - F2[i0]) * fr;
      dsp::SVFc c1 = dsp::svfCoef(f1, fs, 0.33f), c2 = dsp::svfCoef(f2, fs, 0.33f);
      float x[2] = {sl, sr}, lp, bp, hp;
      for (int ch = 0; ch < 2; ch++) {
        v.frm[ch][0].run(x[ch], c1, lp, bp, hp); float b1 = bp * c1.k;
        v.frm[ch][1].run(x[ch], c2, lp, bp, hp); float b2 = bp * c2.k;
        x[ch] = x[ch] * (1.f - cur[P_FORM]) + (b1 + b2) * 1.6f * cur[P_FORM];
      }
      sl = x[0]; sr = x[1];
    }

    // --- obwiednia amp
    float e = v.level;
    if (v.stage == ATTACK) {
      e += 1.f / std::max(1.f, cur[P_ATK] * 0.001f * fs);
      if (e >= 1.f) { e = 1.f; v.stage = DECAY; }
    } else if (v.stage == DECAY) {
      float cd = tc(cur[P_DEC], fs);
      bool toZero = T->oneshot || cur[P_SUS] < 1e-4f;
      float sus = toZero ? 0.f : cur[P_SUS];
      e = sus + (e - sus) * cd;
      if (toZero && e < 1e-4f) { e = 0; v.active = false; }
    } else {
      e *= tc(cur[P_REL], fs);
      if (e < 1e-4f) { e = 0; v.active = false; }
    }
    v.level = e;
    v.fenv *= tc(cur[P_FDEC], fs);
    v.penv *= tc(cur[P_PDEC], fs);

    // --- szum (burst-y + HP)
    float nl = 0, nr = 0;
    if (cur[P_NOISE] > 0.001f) {
      int nb = (int)cur[P_NBURST];
      if (T->oneshot && v.burstIdx < nb - 1 && ++v.burstTimer >= (int)(0.011f * fs)) {
        v.burstTimer = 0; v.burstIdx++; v.nenv = 1.f;
      }
      bool fast = T->oneshot && v.burstIdx < nb - 1;
      v.nenv *= tc(fast ? 8.f : cur[P_NDEC], fs);
      float a = 1.f - std::exp(-6.2831853f * cur[P_NCUT] / fs);
      float g = cur[P_NOISE] * (T->oneshot ? v.nenv : e);
      float r0 = rnd(), r1 = rnd();
      v.lpN[0] += a * (r0 - v.lpN[0]); v.lpN[1] += a * (r1 - v.lpN[1]);
      nl = (r0 - v.lpN[0]) * g; nr = (r1 - v.lpN[1]) * g;
    }

    // --- miks, drive, level
    float ol = sl * e + nl, orr = sr * e + nr;
    float d = cur[P_DRIVE];
    if (d > 0.001f) {
      float g = 1.f + d * 14.f, inv = 1.f / std::tanh(g), w = std::min(1.f, d * 5.f);
      ol += (std::tanh(ol * g) * inv - ol) * w;
      orr += (std::tanh(orr * g) * inv - orr) * w;
    }
    float gain = cur[P_LEVEL] * (0.2f + 0.8f * v.vel);
    out[0] = std::max(-2.f, std::min(2.f, ol * gain));
    out[1] = std::max(-2.f, std::min(2.f, orr * gain));
  }
};

}  // namespace ds

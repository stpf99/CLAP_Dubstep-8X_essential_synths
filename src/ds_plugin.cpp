// ds_plugin.cpp — opakowanie CLAP. Kompilacja:
//   -DDS_ONLY_TYPE=N  -> pojedyncza wtyczka (typ N)
//   (bez makra)       -> paczka DubstepKit.clap z wszystkimi 8 wtyczkami
#include <clap/clap.h>

#include <atomic>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "ds_engine.h"

using namespace ds;

static const char* const FEATURES[] = {CLAP_PLUGIN_FEATURE_INSTRUMENT, CLAP_PLUGIN_FEATURE_SYNTHESIZER,
                                       CLAP_PLUGIN_FEATURE_STEREO, nullptr};

#ifdef DS_ONLY_TYPE
static const int FIRST = DS_ONLY_TYPE, COUNT = 1;
#else
static const int FIRST = 0, COUNT = 8;
#endif

static std::vector<clap_plugin_descriptor_t>& descs() {
  static std::vector<clap_plugin_descriptor_t> D;
  if (D.empty()) {
    for (int i = 0; i < COUNT; i++) {
      const TypeDef& t = types()[FIRST + i];
      D.push_back({CLAP_VERSION_INIT, t.id, t.name, "stpf99", "https://github.com/stpf99", "", "", "1.0.0",
                   t.desc, FEATURES});
    }
  }
  return D;
}

struct Plug {
  clap_plugin_t plugin;
  const clap_host_t* host;
  const TypeDef* T;
  Engine eng;
  std::vector<int> ids;
  bool exposed[NP] = {false};
  std::atomic<double> val[NP];
  double bpm = 140;
};

static void applyParam(Plug* s, uint32_t id, double v) {
  if (id >= (uint32_t)NP || !s->exposed[id]) return;
  const ParamInfo& pi = PI[id];
  double lo = pi.kind == LOG ? 0.0 : pi.lo, hi = pi.kind == LOG ? 1.0 : pi.hi;
  v = std::min(hi, std::max(lo, v));
  s->val[id].store(v);
  s->eng.setParam((int)id, toEng((int)id, v));
}

static void handleEvent(Plug* s, const clap_event_header_t* h) {
  if (h->space_id != CLAP_CORE_EVENT_SPACE_ID) return;
  switch (h->type) {
    case CLAP_EVENT_NOTE_ON: {
      auto* e = (const clap_event_note_t*)h;
      if (e->key >= 0) s->eng.noteOn(e->key, (float)e->velocity);
    } break;
    case CLAP_EVENT_NOTE_OFF: {
      auto* e = (const clap_event_note_t*)h;
      if (e->key < 0) s->eng.allOff(); else s->eng.noteOff(e->key);
    } break;
    case CLAP_EVENT_PARAM_VALUE: {
      auto* e = (const clap_event_param_value_t*)h;
      applyParam(s, e->param_id, e->value);
    } break;
    case CLAP_EVENT_MIDI: {
      auto* e = (const clap_event_midi_t*)h;
      int st = e->data[0] & 0xF0;
      if (st == 0x90 && e->data[2] > 0) s->eng.noteOn(e->data[1], e->data[2] / 127.f);
      else if (st == 0x80 || st == 0x90) s->eng.noteOff(e->data[1]);
      else if (st == 0xB0 && (e->data[1] == 123 || e->data[1] == 120)) s->eng.allOff();
    } break;
    default: break;
  }
}

// ---------------------------------------------------------------- audio / note ports
static uint32_t ap_count(const clap_plugin_t*, bool is_input) { return is_input ? 0 : 1; }
static bool ap_get(const clap_plugin_t*, uint32_t i, bool is_input, clap_audio_port_info_t* info) {
  if (is_input || i != 0) return false;
  info->id = 0; std::strncpy(info->name, "Out", sizeof(info->name));
  info->flags = CLAP_AUDIO_PORT_IS_MAIN; info->channel_count = 2;
  info->port_type = CLAP_PORT_STEREO; info->in_place_pair = CLAP_INVALID_ID;
  return true;
}
static const clap_plugin_audio_ports_t AUDIO_PORTS = {ap_count, ap_get};

static uint32_t np_count(const clap_plugin_t*, bool is_input) { return is_input ? 1 : 0; }
static bool np_get(const clap_plugin_t*, uint32_t i, bool is_input, clap_note_port_info_t* info) {
  if (!is_input || i != 0) return false;
  info->id = 0; info->supported_dialects = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI;
  info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP; std::strncpy(info->name, "Notes", sizeof(info->name));
  return true;
}
static const clap_plugin_note_ports_t NOTE_PORTS = {np_count, np_get};

// ---------------------------------------------------------------- params
static Plug* P(const clap_plugin_t* p) { return (Plug*)p->plugin_data; }
static uint32_t pr_count(const clap_plugin_t* p) { return (uint32_t)P(p)->ids.size(); }
static bool pr_info(const clap_plugin_t* p, uint32_t i, clap_param_info_t* info) {
  Plug* s = P(p); if (i >= s->ids.size()) return false;
  int id = s->ids[i]; const ParamInfo& pi = PI[id];
  std::memset(info, 0, sizeof(*info));
  info->id = id;
  info->flags = CLAP_PARAM_IS_AUTOMATABLE;
  if (pi.kind == INT) info->flags |= CLAP_PARAM_IS_STEPPED;
  if (pi.names) info->flags |= CLAP_PARAM_IS_ENUM;
  std::snprintf(info->name, sizeof(info->name), "%s", pi.name);
  std::snprintf(info->module, sizeof(info->module), "%s", pi.group);
  float d[NP]; s->T->defaults(d);
  info->min_value = pi.kind == LOG ? 0.0 : pi.lo;
  info->max_value = pi.kind == LOG ? 1.0 : pi.hi;
  info->default_value = toClap(id, d[id]);
  return true;
}
static bool pr_get(const clap_plugin_t* p, clap_id id, double* v) {
  Plug* s = P(p); if (id >= (clap_id)NP || !s->exposed[id]) return false;
  *v = s->val[id].load(); return true;
}
static bool pr_v2t(const clap_plugin_t*, clap_id id, double value, char* out, uint32_t sz) {
  if (id >= (clap_id)NP) return false;
  const ParamInfo& pi = PI[id]; float e = toEng((int)id, value);
  if (pi.names) { int i = std::min((int)std::lround(pi.hi), std::max((int)std::lround(pi.lo), (int)std::lround(e)));
                  std::snprintf(out, sz, "%s", pi.names[i]); }
  else if (pi.kind == INT) std::snprintf(out, sz, "%d", (int)std::lround(e));
  else if (e < 10) std::snprintf(out, sz, "%.2f %s", e, pi.unit);
  else if (e < 1000) std::snprintf(out, sz, "%.1f %s", e, pi.unit);
  else std::snprintf(out, sz, "%.0f %s", e, pi.unit);
  return true;
}
static bool pr_t2v(const clap_plugin_t*, clap_id id, const char* text, double* out) {
  if (id >= (clap_id)NP) return false;
  const ParamInfo& pi = PI[id];
  if (pi.names) {
    int n = (int)pi.hi - (int)pi.lo + 1;
    for (int i = 0; i < n; i++) {
      const char* a = pi.names[i]; const char* b = text; bool ok = true;
      while (*b && *a) { if (std::tolower(*a++) != std::tolower(*b++)) { ok = false; break; } }
      if (ok && !*b) { *out = i; return true; }
    }
  }
  char* end; double x = std::strtod(text, &end);
  if (end == text) return false;
  *out = toClap((int)id, (float)x);
  return true;
}
static void flushEvents(Plug* s, const clap_input_events_t* in) {
  if (!in) return;
  for (uint32_t i = 0, n = in->size(in); i < n; i++) handleEvent(s, in->get(in, i));
}
static void pr_flush(const clap_plugin_t* p, const clap_input_events_t* in, const clap_output_events_t*) {
  flushEvents(P(p), in);
}
static const clap_plugin_params_t PARAMS = {pr_count, pr_info, pr_get, pr_v2t, pr_t2v, pr_flush};

// ---------------------------------------------------------------- state
static bool st_save(const clap_plugin_t* p, const clap_ostream_t* os) {
  Plug* s = P(p);
  std::string buf; uint32_t magic = 0x31534b44, n = (uint32_t)s->ids.size();
  buf.append((char*)&magic, 4); buf.append((char*)&n, 4);
  for (int id : s->ids) { int32_t i = id; double v = s->val[id].load(); buf.append((char*)&i, 4); buf.append((char*)&v, 8); }
  size_t off = 0;
  while (off < buf.size()) {
    int64_t w = os->write(os, buf.data() + off, buf.size() - off);
    if (w <= 0) return false;
    off += (size_t)w;
  }
  return true;
}
static bool st_load(const clap_plugin_t* p, const clap_istream_t* is) {
  Plug* s = P(p);
  std::string buf; char tmp[512]; int64_t r;
  while ((r = is->read(is, tmp, sizeof(tmp))) > 0) buf.append(tmp, (size_t)r);
  if (buf.size() < 8) return false;
  uint32_t magic, n; std::memcpy(&magic, buf.data(), 4); std::memcpy(&n, buf.data() + 4, 4);
  if (magic != 0x31534b44 || buf.size() < 8 + (size_t)n * 12) return false;
  for (uint32_t k = 0; k < n; k++) {
    int32_t id; double v; std::memcpy(&id, buf.data() + 8 + k * 12, 4); std::memcpy(&v, buf.data() + 12 + k * 12, 8);
    applyParam(s, (uint32_t)id, v);
  }
  return true;
}
static const clap_plugin_state_t STATE = {st_save, st_load};

// ---------------------------------------------------------------- plugin
static bool pl_init(const clap_plugin_t*) { return true; }
static void pl_destroy(const clap_plugin_t* p) { delete P(p); }
static bool pl_activate(const clap_plugin_t* p, double sr, uint32_t, uint32_t) {
  Plug* s = P(p);
  s->eng.init(s->T, sr);
  for (int i = 0; i < NP; i++) s->eng.setParam(i, toEng(i, s->val[i].load()));
  s->eng.snap();
  return true;
}
static void pl_deactivate(const clap_plugin_t*) {}
static bool pl_start(const clap_plugin_t*) { return true; }
static void pl_stop(const clap_plugin_t*) {}
static void pl_reset(const clap_plugin_t* p) { P(p)->eng.reset(); }
static clap_process_status pl_process(const clap_plugin_t* p, const clap_process_t* pr) {
  Plug* s = P(p);
  if (pr->transport && (pr->transport->flags & CLAP_TRANSPORT_HAS_TEMPO)) s->bpm = pr->transport->tempo;
  s->eng.setTempo(s->bpm);
  uint32_t n = pr->frames_count;
  if (pr->audio_outputs_count < 1 || !pr->audio_outputs[0].data32) { flushEvents(s, pr->in_events); return CLAP_PROCESS_CONTINUE; }
  float* L = pr->audio_outputs[0].data32[0];
  float* R = pr->audio_outputs[0].data32[1];
  const clap_input_events_t* ev = pr->in_events;
  uint32_t nev = ev ? ev->size(ev) : 0, ei = 0, pos = 0;
  while (pos < n) {
    while (ei < nev) {
      const clap_event_header_t* h = ev->get(ev, ei);
      if (h->time > pos) break;
      handleEvent(s, h); ei++;
    }
    uint32_t next = n;
    if (ei < nev) next = std::min(n, ev->get(ev, ei)->time);
    if (next <= pos) next = pos + 1;
    s->eng.render(L + pos, R + pos, (int)(next - pos));
    pos = next;
  }
  while (ei < nev) handleEvent(s, ev->get(ev, ei++));
  return CLAP_PROCESS_CONTINUE_IF_NOT_QUIET;
}
static const void* pl_ext(const clap_plugin_t*, const char* id) {
  if (!std::strcmp(id, CLAP_EXT_AUDIO_PORTS)) return &AUDIO_PORTS;
  if (!std::strcmp(id, CLAP_EXT_NOTE_PORTS)) return &NOTE_PORTS;
  if (!std::strcmp(id, CLAP_EXT_PARAMS)) return &PARAMS;
  if (!std::strcmp(id, CLAP_EXT_STATE)) return &STATE;
  return nullptr;
}
static void pl_main(const clap_plugin_t*) {}

// ---------------------------------------------------------------- factory
static uint32_t f_count(const clap_plugin_factory_t*) { return COUNT; }
static const clap_plugin_descriptor_t* f_desc(const clap_plugin_factory_t*, uint32_t i) {
  return i < (uint32_t)COUNT ? &descs()[i] : nullptr;
}
static const clap_plugin_t* f_create(const clap_plugin_factory_t*, const clap_host_t* host, const char* id) {
  if (!clap_version_is_compatible(host->clap_version)) return nullptr;
  for (int i = 0; i < COUNT; i++) {
    if (std::strcmp(id, descs()[i].id)) continue;
    Plug* s = new Plug();
    s->host = host; s->T = &types()[FIRST + i];
    float d[NP]; s->T->defaults(d);
    for (int k = 0; k < NP; k++) s->val[k].store(toClap(k, d[k]));
    for (auto& e : s->T->ex) { s->ids.push_back(e.p); s->exposed[e.p] = true; }
    s->plugin = {&descs()[i], s, pl_init, pl_destroy, pl_activate, pl_deactivate, pl_start, pl_stop,
                 pl_reset, pl_process, pl_ext, pl_main};
    return &s->plugin;
  }
  return nullptr;
}
static const clap_plugin_factory_t FACTORY = {f_count, f_desc, f_create};
static bool e_init(const char*) { return true; }
static void e_deinit() {}
static const void* e_get(const char* id) { return std::strcmp(id, CLAP_PLUGIN_FACTORY_ID) ? nullptr : &FACTORY; }

extern "C" CLAP_EXPORT const clap_plugin_entry_t clap_entry = {CLAP_VERSION_INIT, e_init, e_deinit, e_get};

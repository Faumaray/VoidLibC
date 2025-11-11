export module rt.format;

import rt.string;
import rt.mem;

namespace rt::format {
  using sink_fn = bool(*)(void*, const char*, unsigned long);

  inline bool sink_write(sink_fn sink, void* cookie, const char* s, unsigned long n) {
    return sink ? sink(cookie, s, n) : false;
  }

  // Unsigned integer -> string with base and zero-padding support
  inline unsigned long itoa_u_pad(unsigned long v, unsigned base, char* buf,
                                  unsigned long cap, bool upper,
                                  unsigned width, bool zpad) {
    if (cap == 0) return 0;
    const char* digs = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    char tmp[32]; unsigned t = 0;
    do {
      tmp[t++] = digs[v % base];
      v /= base;
    } while (v && t < sizeof(tmp));

    unsigned need = t;
    if (width > need) need = width;
    unsigned long n = (need < cap - 1) ? need : (cap - 1);

    // left pad with '0' if requested and width > digits
    unsigned pad = (n > t) ? (n - t) : 0;
    unsigned long i = 0;
    for (; i < pad; ++i) buf[i] = zpad ? '0' : ' ';
    for (unsigned j = 0; j < t && i < n; ++j) buf[i++] = tmp[t - 1 - j];
    buf[n] = 0;
    return n;
  }

  // Signed with padding (dispatches to unsigned after sign handling)
  inline unsigned long itoa_s_pad(long v, char* buf, unsigned long cap,
                                  unsigned width, bool zpad) {
    if (cap == 0) return 0;
    if (v < 0) {
      if (cap < 2) return 0;
      // Handle sign; width applies to digits only
      unsigned long n = itoa_u_pad((unsigned long)(-v), 10, buf + 1, cap - 1, false, width, zpad);
      // If zero-pad requested, keep '-' before zeros: e.g., "-00012"
      // We already wrote digits with any zeros; just put '-' at front.
      buf[0] = '-';
      return n + 1;
    } else {
      return itoa_u_pad((unsigned long)v, 10, buf, cap, false, width, zpad);
    }
  }

  // Minimal parser state
  struct fmt_state {
    bool zero_pad;
    unsigned width;
    enum class len { none, l, ll, z } length;
  };

  inline const char* parse_flags_width_len(const char* p, fmt_state& st) {
    st.zero_pad = false;
    st.width = 0;
    st.length = fmt_state::len::none;

    // flags
    bool parsing = true;
    while (parsing) {
      switch (*p) {
        case '0': st.zero_pad = true; ++p; break;
        default: parsing = false; break;
      }
    }
    // width
    while (*p >= '0' && *p <= '9') {
      st.width = st.width * 10 + (unsigned)(*p - '0');
      ++p;
    }
    // length
    if (*p == 'l') {
      ++p;
      if (*p == 'l') { st.length = fmt_state::len::ll; ++p; }
      else st.length = fmt_state::len::l;
    } else if (*p == 'z') {
      ++p; st.length = fmt_state::len::z;
    }
    return p;
  }

  export long vprint(sink_fn sink, void* cookie, const char* fmt, __builtin_va_list ap) {
    unsigned long out = 0;
    for (const char* p = fmt; *p; ++p) {
      if (*p != '%') {
        if (!sink_write(sink, cookie, p, 1)) return -1;
        ++out; continue;
      }
      ++p;
      if (*p == '%') { if (!sink_write(sink, cookie, "%", 1)) return -1; ++out; continue; }

      fmt_state st{};
      p = parse_flags_width_len(p, st);
      char buf[64];

      switch (*p) {
        case 's': {
          const char* s = __builtin_va_arg(ap, const char*);
          if (!s) s = "(null)";
          unsigned long n = 0; while (s[n]) ++n;
          if (!sink_write(sink, cookie, s, n)) return -1; out += n; break;
        }
        case 'c': {
          char c = (char)__builtin_va_arg(ap, int);
          if (!sink_write(sink, cookie, &c, 1)) return -1; ++out; break;
        }
        case 'd': { // signed
          long v;
          if (st.length == fmt_state::len::ll)      v = __builtin_va_arg(ap, long long);
          else if (st.length == fmt_state::len::l)  v = __builtin_va_arg(ap, long);
          else if (st.length == fmt_state::len::z)  v = (long)__builtin_va_arg(ap, __SIZE_TYPE__);
          else                                      v = __builtin_va_arg(ap, int);
          unsigned long n = itoa_s_pad(v, buf, sizeof(buf), st.width, st.zero_pad);
          if (!sink_write(sink, cookie, buf, n)) return -1; out += n; break;
        }
        case 'u': { // unsigned
          unsigned long uv;
          if (st.length == fmt_state::len::ll)      uv = __builtin_va_arg(ap, unsigned long long);
          else if (st.length == fmt_state::len::l)  uv = __builtin_va_arg(ap, unsigned long);
          else if (st.length == fmt_state::len::z)  uv = (unsigned long)__builtin_va_arg(ap, __SIZE_TYPE__);
          else                                      uv = __builtin_va_arg(ap, unsigned int);
          unsigned long n = itoa_u_pad(uv, 10, buf, sizeof(buf), false, st.width, st.zero_pad);
          if (!sink_write(sink, cookie, buf, n)) return -1; out += n; break;
        }
        case 'o': { // octal
          unsigned long uv;
          if (st.length == fmt_state::len::ll)      uv = __builtin_va_arg(ap, unsigned long long);
          else if (st.length == fmt_state::len::l)  uv = __builtin_va_arg(ap, unsigned long);
          else if (st.length == fmt_state::len::z)  uv = (unsigned long)__builtin_va_arg(ap, __SIZE_TYPE__);
          else                                      uv = __builtin_va_arg(ap, unsigned int);
          unsigned long n = itoa_u_pad(uv, 8, buf, sizeof(buf), false, st.width, st.zero_pad);
          if (!sink_write(sink, cookie, buf, n)) return -1; out += n; break;
        }
        case 'x': case 'X': { // hex
          unsigned long uv;
          if (st.length == fmt_state::len::ll)      uv = __builtin_va_arg(ap, unsigned long long);
          else if (st.length == fmt_state::len::l)  uv = __builtin_va_arg(ap, unsigned long);
          else if (st.length == fmt_state::len::z)  uv = (unsigned long)__builtin_va_arg(ap, __SIZE_TYPE__);
          else                                      uv = __builtin_va_arg(ap, unsigned int);
          unsigned long n = itoa_u_pad(uv, 16, buf, sizeof(buf), *p=='X', st.width, st.zero_pad);
          if (!sink_write(sink, cookie, buf, n)) return -1; out += n; break;
        }
        case 'p': { // pointer (always 0x + hex, width ignored)
          unsigned long uv = (unsigned long)__builtin_va_arg(ap, void*);
          if (!sink_write(sink, cookie, "0x", 2)) return -1; out += 2;
          unsigned long n = itoa_u_pad(uv, 16, buf, sizeof(buf), false, 1, true);
          if (!sink_write(sink, cookie, buf, n)) return -1; out += n; break;
        }
        default: {
          // unknown: print literally
          if (!sink_write(sink, cookie, "%", 1)) return -1; ++out;
          if (!sink_write(sink, cookie, p, 1)) return -1; ++out;
        }
      }
    }
    return (long)out;
  }

  export long print(sink_fn sink, void* cookie, const char* fmt, ...) {
    __builtin_va_list ap; __builtin_va_start(ap, fmt);
    long r = vprint(sink, cookie, fmt, ap);
    __builtin_va_end(ap);
    return r;
  }
}

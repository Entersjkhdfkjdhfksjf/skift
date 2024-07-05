#pragma once

#include <karm-base/map.h>
#include <karm-math/rand.h>
#include <karm-sys/mmap.h>

#include "font.h"
#include "ttf/spec.h"

namespace Karm::Text {

struct TtfFontface : public Fontface {
    Sys::Mmap _mmap;
    Ttf::Font _ttf;
    Map<Rune, Glyph> _cachedEntries;
    Map<Glyph, f64> _cachedAdvances;
    Map<Cons<Glyph>, f64> _cachedKerns;

    static Res<Strong<TtfFontface>> load(Sys::Mmap &&mmap) {
        auto ttf = try$(Ttf::Font::load(mmap.bytes()));
        return Ok(makeStrong<TtfFontface>(std::move(mmap), ttf));
    }

    TtfFontface(Sys::Mmap &&mmap, Ttf::Font ttf)
        : _mmap(std::move(mmap)),
          _ttf(std::move(ttf)) {
    }

    FontMetrics metrics() const override {
        auto m = _ttf.metrics();
        return {
            .ascend = m.ascend,
            .captop = m.ascend,
            .descend = m.descend,
            .linegap = m.linegap,
            .advance = 0
        };
    }

    Glyph glyph(Rune rune) override {
        auto glyph = _cachedEntries.tryGet(rune);
        if (glyph.has())
            return glyph.unwrap();
        auto g = _ttf.glyph(rune);
        _cachedEntries.put(rune, g);
        return g;
    }

    f64 advance(Glyph glyph) override {
        auto advance = _cachedAdvances.tryGet(glyph);
        if (advance.has())
            return advance.unwrap();
        auto a = _ttf.glyphMetrics(glyph).advance;
        _cachedAdvances.put(glyph, a);
        return a;
    }

    f64 kern(Glyph prev, Glyph curr) override {
        auto kern = _cachedKerns.tryGet({prev, curr});
        if (kern.has())
            return kern.unwrap();

        auto k = _ttf.glyphKern(prev, curr);
        _cachedKerns.put({prev, curr}, k);
        return k;
    }

    void contour(Gfx::Context &g, Glyph glyph) const override {
        _ttf.glyphContour(g, glyph);
    }

    f64 units() const override {
        return _ttf.unitPerEm();
    }
};

} // namespace Karm::Text

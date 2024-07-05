#pragma once

#include <karm-base/array.h>
#include <karm-gfx/context.h>

#include "font.h"

namespace Karm::Text {

struct VgaFontface : public Fontface {
    static constexpr isize WIDTH = 8;
    static constexpr isize HEIGHT = 8;

    static constexpr Array<u8, 1024> const DATA = {
#include "vga.inc"
    };

    FontMetrics metrics() const override {
        return {
            .ascend = 12,
            .captop = 10,
            .descend = 4,
            .linegap = 4,
            .advance = 8,
        };
    }

    Glyph glyph(Rune rune) override {
        One<Ibm437> one;
        encodeOne<Ibm437>(rune, one);
        return Glyph(one);
    }

    f64 advance(Glyph) override {
        return 8;
    }

    f64 kern(Glyph, Glyph) override {
        return 0;
    }

    void contour(Gfx::Context &g, Glyph glyph) const override {
        for (isize y = 0; y < HEIGHT; y++) {
            for (isize x = 0; x < WIDTH; x++) {
                u8 byte = DATA[glyph.value() * HEIGHT + y];
                if (byte & (0x80 >> x)) {
                    g.rect(Math::Recti{x, y - 8, 1, 1}.cast<f64>());
                }
            }
        }
    }

    f64 units() const override {
        return 8;
    }
};

} // namespace Karm::Text

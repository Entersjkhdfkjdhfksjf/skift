#pragma once

#include <karm-base/enum.h>
#include <karm-base/string.h>

namespace Karm::Events {

// MARK: Mouse ---------------------------------------------

enum struct Button : u8 {
    NONE = 0,

    LEFT = 1 << 0,
    MIDDLE = 1 << 1,
    RIGHT = 1 << 2,
    X1 = 1 << 3,
    X2 = 1 << 4,
};

FlagsEnum$(Button);

// MARK: Keyboard ------------------------------------------

enum struct Mod : u8 {
    NONE = 0,

    LSHIFT = 1 << 0,
    RSHIFT = 1 << 1,
    ANY_SHIFT = LSHIFT | RSHIFT,

    LCTRL = 1 << 2,
    RCTRL = 1 << 3,
    ANY_CTR = LCTRL | RCTRL,

    ALT = 1 << 4,
    ALTGR = 1 << 5,

    SUPER = 1 << 6,
};

FlagsEnum$(Mod);

enum struct KeyMotion {
    RELEASED,
    PRESSED,
};

struct Key {
    enum struct Code {
#define KEY(name, code) name = code,
#include "defs/keys.inc"
#undef KEY
    };

    using enum Code;

    Code _code;

    Key(Code code = Code::INVALID)
        : _code(code) {}

    Str name() const {
        switch (_code) {
#define KEY(name, code) \
    case Code::name:    \
        return #name;
#include "defs/keys.inc"
#undef KEY
        }
        return "INVALID";
    }

    Code code() {
        return _code;
    }

    auto operator<=>(Key const &other) const = default;

    auto operator<=>(Code const &other) const {
        return _code <=> other;
    }
};

} // namespace Karm::Events

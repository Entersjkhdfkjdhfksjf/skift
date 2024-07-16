#pragma once

#include <karm-io/emit.h>

#include "length.h"
#include "percent.h"

namespace Vaev {

enum struct BoxSizing {
    CONTENT_BOX,
    BORDER_BOX,
};

struct Size {
    enum struct Type {
        NONE,
        AUTO,
        LENGTH,
        MIN_CONTENT,
        MAX_CONTENT,
        FIT_CONTENT,
    };

    using enum Type;

    Type type;
    PercentOr<Length> value;

    constexpr Size() : type(AUTO), value(Length{}) {
    }

    constexpr Size(Type type) : type(type), value(Length{}) {
    }

    constexpr Size(Type type, PercentOr<Length> value) : type(type), value(value) {
    }

    constexpr Size(Percent value) : type(LENGTH), value(value) {
    }

    constexpr Size(Length value) : type(LENGTH), value(value) {
    }

    bool operator==(Type type) const {
        return this->type == type;
    }

    void repr(Io::Emit &e) const {
        switch (type) {
        case Type::NONE:
            e("none");
            break;
        case Type::AUTO:
            e("auto");
            break;

        case Type::LENGTH:
            e("{}", value);
            break;

        case Type::MIN_CONTENT:
            e("min-content");
            break;

        case Type::MAX_CONTENT:
            e("max-content");
            break;

        case Type::FIT_CONTENT:
            e("fit-content");
            break;

        default:
            e("unknown");
            break;
        }
    }
};

struct Sizing {
    BoxSizing boxSizing;
    Size width, height;
    Size minWidth, minHeight;
    Size maxWidth, maxHeight;
};

} // namespace Vaev

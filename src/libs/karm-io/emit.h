#pragma once

#include <karm-base/string.h>
#include <karm-io/fmt.h>
#include <karm-io/funcs.h>
#include <karm-io/traits.h>

namespace Karm::Io {

struct Emit : public Io::TextWriterBase<> {
    Io::TextWriter &_writer;
    usize _ident = 0;
    usize _total = 0;
    Res<> _error = Ok();
    bool _newline = false;

    Emit(Io::TextWriter &writer)
        : _writer(writer) {
    }

    void _tryWrapper(Res<usize> result) {
        if (result) {
            _total += result.unwrap();
        } else {
            _error = result.none();
        }
    }

    void indent() {
        _ident++;
    }

    void indentNewline() {
        indent();
        newline();
    }

    void indented(auto inner) {
        indent();
        inner();
        deindent();
    }

    void deindent() {
        if (_ident == 0) [[unlikely]]
            panic("deident() underflow");

        _ident--;
    }

    void newline() {
        if (not _error)
            return;

        _newline = true;
    }

    Res<usize> _insertNewline() {
        if (not _error)
            return _error.none();

        usize written = try$(_writer.writeRune('\n'));
        _newline = false;
        for (usize i = 0; i < _ident; i++)
            written += try$(_writer.writeStr("    "s));
        return Ok(written);
    }

    virtual Res<usize> write(Bytes bytes) override {
        return _writer.write(bytes);
    }

    Res<usize> writeRune(Rune r) override {
        if (r == '\n') {
            newline();
            return Ok(0);
        }

        usize written = 0;
        if (_newline)
            written += try$(_insertNewline());
        written += try$(_writer.writeRune(r));
        return Ok(written);
    }

    void operator()(Rune r) {
        if (r == '\n') {
            newline();
            return;
        }
        _tryWrapper(writeRune(r));
    }

    void operator()(Str str) {
        _tryWrapper(writeStr(str));
    }

    template <typename... Ts>
    void operator()(Str format, Ts &&...ts) {
        _tryWrapper(Io::format(*this, format, std::forward<Ts>(ts)...));
    }

    template <typename... Ts>
    void ln(Ts &&...ts) {
        _tryWrapper(Io::format(*this, std::forward<Ts>(ts)...));
        newline();
    }

    usize total() {
        return _total;
    }

    Res<usize> flush() override {
        if (_newline)
            return _writer.writeRune('\n');
        return Ok(0uz);
    }

    Res<> raiseIfError() {
        return _error;
    }
};

template <Reprable T>
struct Formatter<T> {
    Res<usize> format(Io::TextWriter &writer, T const &val) {
        Io::Emit emit{writer};
        val.repr(emit);
        try$(emit.raiseIfError());
        return Ok(emit.total());
    }
};

} // namespace Karm::Io

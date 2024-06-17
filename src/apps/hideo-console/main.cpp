#include <hideo-base/scafold.h>
#include <karm-sys/entry.h>
#include <karm-ui/app.h>

namespace Hideo::Console {

Ui::Child app() {
    return scafold({
        .icon = Mdi::CONSOLE_LINE,
        .title = "Console"s,
        .body = [] {
            return Ui::codeMedium(" $ ls");
        },
    });
}

} // namespace Hideo::Console

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    co_return Ui::runApp(ctx, Hideo::Console::app());
}

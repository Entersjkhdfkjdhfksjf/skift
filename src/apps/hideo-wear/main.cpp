#include <karm-sys/entry.h>
#include <karm-ui/app.h>

namespace Hideo::Wear {

Ui::Child app() {
    auto [date, time] = DateTime{
        .date = {1, Month::APRIL, 2024},
        .time = {56, 34, 12},
    };

    auto dateTime = Io::format(
        // Mon. 28 Jul
        "{}. {} {}",
        Io::toCapitalCase(date.dayOfWeek().abbr()),
        date.dayOfMonth() + 1,
        Io::toCapitalCase(date.month.str())
    );

    auto clock = Ui::vflow(
        16,
        Math::Align::CENTER,
        Ui::displayMedium("{02}:{02}", time.hour, time.minute),
        Ui::titleMedium(dateTime.unwrap())
    );

    return Ui::stack(
               Ui::image(Media::loadImageOrFallback("bundle://skift-wallpapers/images/abstract.qoi"_url).unwrap()) | Ui::cover(),
               clock | Ui::center()
           ) |
           Ui::maxSize(192);
}

} // namespace Hideo::Wear

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    co_return Ui::runApp(
        ctx,
        Hideo::Wear::app()
    );
}

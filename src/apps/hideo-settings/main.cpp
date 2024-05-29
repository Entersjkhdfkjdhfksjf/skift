#include <hideo-base/scafold.h>
#include <hideo-base/sidenav.h>
#include <karm-sys/entry-async.h>
#include <karm-ui/anim.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "app.h"

namespace Hideo::Settings {

Ui::Child sidebar(State const &state) {
    Ui::Children items = {
        Hideo::sidenavItem(state.page() == Page::ACCOUNT, Model::bind<GoTo>(Page::ACCOUNT), Mdi::ACCOUNT, "Accounts"s),
        Hideo::sidenavItem(state.page() == Page::PERSONALIZATION, Model::bind<GoTo>(Page::PERSONALIZATION), Mdi::PALETTE, "Personalization"s),
        Hideo::sidenavItem(state.page() == Page::APPLICATIONS, Model::bind<GoTo>(Page::APPLICATIONS), Mdi::WIDGETS_OUTLINE, "Applications"s),

        Hideo::sidenavItem(state.page() == Page::SYSTEM, Model::bind<GoTo>(Page::SYSTEM), Mdi::LAPTOP, "System"s),
        Hideo::sidenavItem(state.page() == Page::NETWORK, Model::bind<GoTo>(Page::NETWORK), Mdi::WIFI, "Network"s),
        Hideo::sidenavItem(state.page() == Page::SECURITY, Model::bind<GoTo>(Page::SECURITY), Mdi::SECURITY, "Security & Privacy"s),

        Hideo::sidenavItem(state.page() == Page::UPDATES, Model::bind<GoTo>(Page::UPDATES), Mdi::UPDATE, "Updates"s),
        Hideo::sidenavItem(state.page() == Page::ABOUT, Model::bind<GoTo>(Page::ABOUT), Mdi::INFORMATION_OUTLINE, "About"s),
    };

    return Hideo::sidenav(items);
}

// MARK: Pages -----------------------------------------------------------------

Ui::Child pageContent(State const &state) {
    switch (state.page()) {
    case Page::HOME:
        return pageHome(state);

    case Page::ABOUT:
        return pageAbout(state);

    default:
        return Ui::grow(
            Ui::center(
                Ui::text("Content")
            )
        );
    }
}

// MARK: Body ------------------------------------------------------------------

Ui::Child app() {
    return Ui::reducer<Model>({}, [](State const &s) {
        return scafold({
            .icon = Mdi::COG,
            .title = "Settings"s,
            .startTools = slots$(
                Ui::button(Model::bindIf<GoBack>(s.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
                Ui::button(Model::bindIf<GoForward>(s.canGoForward()), Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT),
                Ui::button(Model::bind<GoTo>(Page::HOME), Ui::ButtonStyle::subtle(), Mdi::HOME)
            ),
            .sidebar = slot$(sidebar(s)),
            .body = slot$(pageContent(s) | Ui::grow()),
        });
    });
}

} // namespace Hideo::Settings

Async::Task<> entryPointAsync(Sys::Ctx &ctx) {
    co_return Ui::runApp(ctx, Hideo::Settings::app());
}

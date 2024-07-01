#include <karm-sys/time.h>
#include <vaev-css/mod.h>
#include <vaev-dom/element.h>
#include <vaev-layout/builder.h>
#include <vaev-paint/stack.h>
#include <vaev-style/computer.h>

#include "render.h"

namespace Vaev::View {

static void _collectStyle(Dom::Node const &node, Style::StyleBook &sb) {
    if (auto *el = node.is<Dom::Element>(); el and el->tagName == Html::STYLE) {
        auto text = el->textContent();
        Io::SScan textScan{text};
        auto sheet = Css::parseStylesheet(textScan);
        sb.add(std::move(sheet));
    } else {
        for (auto &child : node.children())
            _collectStyle(*child, sb);
    }
}

RenderResult render(Dom::Document const &dom, Style::Media const &media, Vec2Px viewport) {
    Style::StyleBook stylebook;
    stylebook.add(Css::fetchStylesheet("bundle://vaev-view/user-agent.css"_url).take("user agent stylesheet not available"));
    _collectStyle(dom, stylebook);

    Style::Computer computer{media, stylebook};
    auto layoutRoot = Layout::build(computer, dom);
    layoutRoot->layout(viewport);

    auto paintRoot = makeStrong<Paint::Stack>();
    layoutRoot->paint(*paintRoot);
    paintRoot->prepare();

    return {layoutRoot, paintRoot};
}

} // namespace Vaev::View

#include <hjert-api/api.h>
#include <karm-base/map.h>
#include <karm-logger/logger.h>
#include <karm-sys/entry-async.h>

#include "cmos.h"
#include "io.h"
#include "ps2.h"

namespace Grund::Device {

struct IsaRootBus : public Node {
    Res<> init() override {
        auto i18042port = try$(PortIo::open({0x60, 0x8}));
        try$(attach(makeStrong<Ps2::I8042>(i18042port)));

        auto cmosPort = try$(PortIo::open({0x70, 0x2}));
        try$(attach(makeStrong<Cmos::Cmos>(cmosPort)));

        return Ok();
    }
};

} // namespace Grund::Device
Async::Task<> entryPointAsync(Sys::Ctx &) {
    logInfo("devices: building device tree...");
    auto root = makeStrong<Grund::Device::IsaRootBus>();
    co_try$(root->init());

    logInfo("devices: binding IRQs...");
    auto listener = co_try$(Hj::Listener::create(Hj::ROOT));

    Map<Hj::Cap, usize> cap2irq = {};
    Vec<Hj::Irq> irqs = {};

    for (usize i = 0; i < 16; i++) {
        auto irq = co_try$(Hj::Irq::create(Hj::ROOT, i));
        co_try$(listener.listen(irq, Hj::Sigs::TRIGGERED, Hj::Sigs::NONE));
        cap2irq.put(irq.cap(), i);
        irqs.pushBack(std::move(irq));
    }

    while (true) {
        co_try$(listener.poll(TimeStamp::endOfTime()));

        auto ev = listener.next();
        while (ev) {
            co_try$(Hj::_signal(ev->cap, Hj::Sigs::NONE, Hj::Sigs::TRIGGERED));

            auto irq = cap2irq.get(ev->cap);
            if (irq) {
                auto e = Sys::makeEvent<Grund::Device::IrqEvent>(Sys::Propagation::UP, *irq);
                co_try$(root->event(*e));
            }
            ev = listener.next();
        }
    }

    co_return Ok();
}

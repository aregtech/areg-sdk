/************************************************************************
 * Checks that Application::release() joins and deletes the component
 * threads when the application never calls Application::unload_model().
 *
 * Not part of the CMake build. Build it with release-unload.bat.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/base/Thread.hpp"

#include <atomic>
#include <cstdio>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

namespace
{
    constexpr char const _modelName[]   { "ReleaseUnloadModel" };
    constexpr char const _roleOne[]     { "ReleaseUnloadOne" };
    constexpr char const _roleTwo[]     { "ReleaseUnloadTwo" };
    constexpr char const _threadOne[]   { "ReleaseUnloadThreadOne" };
    constexpr char const _threadTwo[]   { "ReleaseUnloadThreadTwo" };

    std::atomic_int gAlive{ 0 };
}

class ProbeComponent : public areg::Component
{
public:
    // The model registers the component through a static 'destroy', which otherwise hides the
    // inherited virtual areg::RuntimeObject::destroy() and makes the class warn under
    // -Woverloaded-virtual. Bringing the base name into scope keeps both callable.
    using areg::Component::destroy;

    static areg::Component * create(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    {
        return new ProbeComponent(entry, owner);
    }

    static void destroy(areg::Component & item, const areg::ComponentEntry &)
    {
        delete (&item);
    }

    ProbeComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
    {
        gAlive.fetch_add(1);
    }

    virtual ~ProbeComponent()
    {
        gAlive.fetch_sub(1);
    }
};

BEGIN_MODEL(_modelName)
    BEGIN_REGISTER_THREAD(_threadOne)
        BEGIN_REGISTER_COMPONENT(_roleOne, ProbeComponent)
        END_REGISTER_COMPONENT(_roleOne)
    END_REGISTER_THREAD(_threadOne)

    BEGIN_REGISTER_THREAD(_threadTwo)
        BEGIN_REGISTER_COMPONENT(_roleTwo, ProbeComponent)
        END_REGISTER_COMPONENT(_roleTwo)
    END_REGISTER_THREAD(_threadTwo)
END_MODEL(_modelName)

int main()
{
    areg::Application::setup(false, true, false, true, false, nullptr);
    areg::Application::load_model(_modelName);

    areg::Thread::sleep(300);

    const int loaded = gAlive.load();
    printf("components alive after load        : %d\n", loaded);

    // Deliberately no Application::unload_model() here. release() has to finish
    // the job on its own, which is what this test is about.
    areg::Application::release();

    const int left = gAlive.load();
    printf("components alive after release()   : %d\n", left);

    const bool threadOneGone = areg::Thread::find_by_address(areg::ThreadAddress(_threadOne)) == nullptr;
    const bool threadTwoGone = areg::Thread::find_by_address(areg::ThreadAddress(_threadTwo)) == nullptr;
    printf("component threads out of registry  : %s / %s\n"
          , threadOneGone ? "yes" : "no", threadTwoGone ? "yes" : "no");

    const bool ok = (loaded == 2) && (left == 0) && threadOneGone && threadTwoGone;
    printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}

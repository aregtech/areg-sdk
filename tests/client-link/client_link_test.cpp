/************************************************************************
 * Link test of a client of the shared areg library, built without link time optimization.
 *
 * A client compiler that devirtualizes a call to an inline virtual method of an exported class
 * may refer to the library's copy of the method instead of emitting its own, and a library built
 * with link time optimization keeps that copy local: the client fails to link (bug 418). GCC 15.2
 * at -O2 and -O3 did so for this file while Mutex::lock() was inline. Keep the code as it is:
 * other code in the same file changes the inlining and hides the defect.
 * tools/intern/check_inline_virtuals.py guards every class.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncPrimitives.hpp"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <deque>
#include <thread>

//!< A queue of the client, locked with areg::Lock on a member of the type SYNC.
template <typename VALUE, typename SYNC>
class Guarded
{
public:
    //!< Appends the value, returns the size of the queue.
    unsigned push(const VALUE & value) { areg::Lock lock(mSync); mValues.push_back(value); return static_cast<unsigned>(mValues.size()); }
private:
    mutable SYNC        mSync;
    std::deque<VALUE>   mValues;
};

int main()
{
    static Guarded<int, areg::Mutex> values;
    values.push(1);
    static std::atomic<bool> done{ false };
    std::thread([]() { values.push(2); done = true; }).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::printf("client link test: %s\n", done.load() ? "passed" : "FAILED");
    return (done.load() ? 0 : 1);
}

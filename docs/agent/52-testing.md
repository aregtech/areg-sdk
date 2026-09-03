# Testing an application

Three things can be tested, and they need different tools. Pick the row, not the page.

| What you want to know | Use | Where |
|---|---|---|
| does the whole application still work | the scenario harness | `50-running.md`, last section |
| does one component answer correctly | a scripted provider in the model | section 1 below |
| does a plain function or class still work | any C++ test framework | section 3 below |

Nothing here needs a second machine, `mtrouter`, or a network. A test that needs the
router is a scenario, not a unit test.

## 1. Test a consumer against a scripted provider

A consumer's logic is the part worth testing, and it needs a provider to answer it.
Register a provider that answers from a script instead of doing the work. The consumer
cannot tell the difference: it resolves the same role name, over the same generated
base, through the same dispatcher.

**Copy `recipes/12-testing/` and change the script.** It registers one scripted
provider and two consumers, drives a success and a refusal, asserts both, and returns
a non-zero exit code when an assertion fails.

The shape:

```cpp
#include "areg/component/Component.hpp"

class ScriptedProvider final : public    areg::Component
                             , protected HelloServiceProviderBase
{
protected:
    void request_hello_service( const areg::String & client ) final
    {
        response_hello_service( client != "Client_Bad" );   // the script
    }
};
```

The model registers it under the role name the consumer depends on, so nothing in the
consumer changes:

```cpp
#include "areg/component/ComponentLoader.hpp"

BEGIN_REGISTER_COMPONENT("ScriptedProvider", ScriptedProvider)
    REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
END_REGISTER_COMPONENT("ScriptedProvider")
```

**A provider is tested the same way with the sides swapped:** register the real
provider and a consumer that sends the requests the test is about and records what
comes back.

## 2. Reaching the verdict

A component answers on its own thread, so the assertions run when the last answer has
arrived, not when `main` reaches the end.

- Count the answers with a `std::atomic<int>`, and assert once the count is the number
  the test sent.
- Call `areg::Application::signal_quit()` from that point. Without it the process waits
  for ever and the test times out instead of failing.
- Return a non-zero exit code when an assertion failed, so a harness that only reads
  the exit code still fails.

```cpp
// areg-check: ignore
if ( gAnswered.fetch_add( 1 ) + 1 == _expected )
{
    check( "a scripted success reaches the consumer", gGood.load() );
    areg::Application::signal_quit();
}
```

**Never sleep to wait for an answer.** A sleep long enough to be reliable makes the
suite slow, and a sleep short enough to be fast makes it flaky. The count is the
signal. `AGENTS.md` section 6 forbids blocking in a handler, and a test handler is a
handler.

**Give the test a timeout from outside**, so a component that never answers fails
rather than hangs:

```bash
timeout 30 ./build/bin/mytests.elf
```

On Windows there is no `timeout` wrapper of this kind; give the test itself a limit,
or run it from the scenario harness, which times every scenario out:

```
build\bin\mytests.exe
```

## 3. Testing plain logic

Logic that does not need a service should not be inside a component. Move it into an
ordinary class, test it with whatever framework the project already uses, and let the
component call it. The SDK's own suite uses GTest under `tests/`; an application is
free to use anything, because none of this touches areg.

This is the cheapest test to write and the one that stays fastest, so prefer it
whenever a rule, a calculation or a state decision can be lifted out of a handler.

## 4. The whole application, as one verdict

`run_scenarios.py` starts the processes, matches the expected lines and checks the
exit codes. It is documented in `50-running.md`; a `scenarios.json` beside the project
is what an agent runs to prove the application works. Use it for anything that needs
`mtrouter`, two processes, or a real log database.

## 5. Wiring the project's own CI

A test binary and a scenario file are enough:

```yaml
- run: cmake -B build -DCMAKE_BUILD_TYPE=Release
- run: cmake --build build -j
- run: timeout 60 ./build/bin/mytests.elf
- run: python3 <areg-sdk>/tools/agent/run_scenarios.py
- run: python3 <areg-sdk>/tools/agent/check_contract.py . --strict
```

`check_contract.py --strict` is the one that catches the mistakes a compiler does not:
a handler that blocks, a dependency naming no role, a broadcast nothing subscribes to.
Run it on the application, not on the SDK.

## When this is not enough

- the application will not start, or a consumer never connects: `51-debug.md`
- reading what actually happened out of the logs: `35-sqlog.md`
- starting and stopping the router and the collector: `50-running.md`

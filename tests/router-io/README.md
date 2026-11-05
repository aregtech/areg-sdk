# Router I/O and registration contracts

`areg-router-io-test` compiles the router service into its own process. CTest
starts a fresh process for each case. Raw loopback TCP clients exercise the
actual receive, dispatch, registration and send paths, both with one shared
I/O pair (`0`) and a pool of four pairs (`4`). Ports are assigned by the OS.

Pure value-object, registry and wire-contract tests live separately in
`tests/units/RouterServiceRegistryTest.cpp`, with native GoogleTest assertions.
CMake and the Visual Studio unit-test project compile them into `areg-unit-tests`.
They do not start a router or create sockets.

Build both targets:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DAREG_TESTS=ON -DAREG_EXAMPLES=OFF -DAREG_OUTPUT_LAYOUT=OFF
cmake --build build --target areg-unit-tests areg-router-io-test -j 4
```

Visual Studio builds both projects from the `tests` folder in `areg-sdk.sln`:

```bat
msbuild areg-sdk.sln /m /t:tests\areg-unit-tests;tests\areg-router-io-test /p:Configuration=Release /p:Platform=x64
```

Select the integration cases:

```sh
ctest --test-dir build --output-on-failure -R '^areg-router-io-test\.(registration-|duplicate-role\.|held-provider-|router-stop-registered\.)'
```

Select the pure unit suites separately:

```sh
ctest --test-dir build --output-on-failure -R '^Router(ServiceStub|ServiceProxy|ServiceRegistry|ListServiceProxies|RegistrationWire)Test\.'
```

The equivalent GoogleTest filter is
`RouterServiceStubTest.*:RouterServiceProxyTest.*:RouterServiceRegistryTest.*:RouterListServiceProxiesTest.*:RouterRegistrationWireTest.*`.
Disabled tests remain excluded unless explicitly enabled.

The original I/O cases remain selectable with `-R '^areg-router-io-test\.'`.
Each executable invocation takes `<case> <pairs> <bin-dir>`; `<bin-dir>` contains
the configured `config/areg.init`. The network cases remove their generated
configuration on exit.

## Registration cases

All names below have CTest prefix `areg-router-io-test.` and suffix `.0` / `.4`.

| Case | Contract |
|---|---|
| `registration-provider-first-provider-out` | Provider registers first and unregisters first; consumer waits and both reconnect |
| `registration-provider-first-consumer-out` | Provider registers first; consumer unregisters before its provider |
| `registration-consumer-first-provider-out` | Waiting consumer connects when its provider arrives; provider unregisters first |
| `registration-consumer-first-consumer-out` | Consumer registers first and unregisters first |
| `registration-two-proxies` | Two remote sources connect to one provider; removing one preserves the other |
| `registration-two-proxies-one-source` | Two consumer threads on one source each reach the provider; provider availability is sent once to that source |
| `registration-truncated-provider` | EOF one byte before the end of provider registration cannot leave a phantom provider |
| `registration-truncated-consumer` | Truncated consumer registration cannot leave a phantom consumer |
| `registration-provider-disconnect` | Socket loss unregisters a connected provider and a new source reconnects its consumer |
| `registration-consumer-disconnect` | Socket loss unregisters a consumer without removing its provider |
| `registration-same-source` | Local provider/consumer registrations in both orders produce no remote notifications; later remote registration works |
| `duplicate-role` | A living owner retains its role; both sources receive duplicate notices and the challenger is rejected |
| `held-provider-takeover` | Repeated held registration does not duplicate entries; a waiting successor takes over after owner unregistration |
| `held-provider-cancel` | Cancelling a held provider leaves the owner intact and prevents takeover |
| `held-provider-disconnect` | Disconnect removes held registrations without affecting the live owner |
| `router-stop-registered` | Stop joins the router and closes connected and pending clients; restart starts with an empty registry |

Notifications check the action, disconnect reason, router source, recipient,
service identity, endpoint thread and instance number. A connection notification
sent through the dispatcher provides an ordered barrier; an unexpected extra
notification fails rather than being silently skipped. Data forwarding is not
used as a registration barrier because its receive fast path bypasses dispatch.

The duplicate case waits for the real rejection timer with keepalive configured
to the supported minimum of six seconds. Held-provider cases configure the
maximum keepalive so ordinary scheduler delays do not turn takeover into rejection.
There are no elapsed-time assertions or new sleeps. Thirty-second socket waits
are hang guards; CTest allows 300 seconds per network case.

## GoogleTest unit cases

The separate suites distinguish the registry, its small classes and wire helpers:

| Suite | Exact case names |
|---|---|
| `RouterServiceStubTest` | `copy_move_identity_and_connection_state`, `service_and_consumer_lookup_keys`, `invalid_and_unsourced_endpoints_do_not_connect` |
| `RouterServiceProxyTest` | `copy_move_identity_and_target_lifecycle` |
| `RouterServiceRegistryTest` | `missing_endpoints_leave_registry_empty`, `provider_first_lifecycle`, `consumer_first_lifecycle`, `removing_last_waiting_consumer_erases_placeholder`, `disconnect_and_reregister_preserve_sibling`, `cookie_and_source_filters_are_distinct` |
| `RouterListServiceProxiesTest` | `unique_endpoints_selection_copy_move_and_availability` |
| `RouterRegistrationWireTest` | `factories_preserve_routing_identity_version_and_reason`, `local_sources_and_services_cannot_register_remotely` |

These retain the endpoint default/valid/invalid states, copy/move operations,
identity/hash contracts, connection transitions, duplicate registration,
replacement providers, pending/final-entry cleanup, distinct cookie/source
filters, list uniqueness and append/update counts. The wire cases check all eight
register/unregister request/notification factories, exact header fields,
header-only wire size, address round trips and rejection of local-only services.

`RouterServiceRegistryTest.DISABLED_source_wildcard_returns_all_endpoints` is an
additional, disabled GoogleTest case for bug 809, also discovered as disabled by CTest.
It asserts the documented `service_sources(COOKIE_ANY)` contract: one registered
provider and consumer must both be returned with their original source IDs.
The current implementation compares the wildcard as an ordinary source and
returns neither. The case is compiled but not part of an enabled test run.

Both executables compile in Release and Debug on WSL Ubuntu; their native
Visual Studio Release x64 targets also compile. Dispatcher verification includes
20 repetitions of the new registry and socket cases, all new socket cases on
Windows and TSan, and all router I/O cases with ASan and UBSan.
The disabled wildcard case reproduces the documented mismatch in two runs.

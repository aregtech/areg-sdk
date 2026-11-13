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
ctest --test-dir build --output-on-failure -R '^areg-router-io-test\.(registration-|duplicate-role\.|name-collision-|held-provider-|router-stop-registered\.)'
```

With logging enabled, every registration case also runs as `<case>.log`.

Select the pure unit suites separately:

```sh
ctest --test-dir build --output-on-failure -R '^Router(ServiceStub|ServiceProxy|ServiceRegistry|ListServiceProxies|RegistrationWire)Test\.'
```

The equivalent GoogleTest filter is
`RouterServiceStubTest.*:RouterServiceProxyTest.*:RouterServiceRegistryTest.*:RouterListServiceProxiesTest.*:RouterRegistrationWireTest.*`.
Disabled tests remain excluded unless explicitly enabled.

The original I/O cases remain selectable with `-R '^areg-router-io-test\.'`.
Each executable invocation takes `<case> <pairs> <bin-dir>`; `<bin-dir>` contains
the configured `config/areg.init`. A case reads that file, sets its own port, pool
size and logging, and loads the result from memory: it writes no configuration file,
so cases run in parallel without sharing one. The `log` cases write the router log to
`config/router-io-<case>-0.log` and remove it on exit.

## I/O cases

All names below have CTest prefix `areg-router-io-test.` and suffix `.0` / `.4`.

| Case | Contract |
|---|---|
| `stale-cache` | A client that sends its disconnect and half of a message leaves nothing behind; the next client connects and gets its cookie |
| `wrong-client` | A client connected right after another one left never gets the messages of that one |
| `partial-header` | A client that stops after 10 bytes of a header does not stop the router from serving another client |
| `partial-body` | A client that stops inside the body of a message does not stop the router from serving another client |
| `invalid-type` | A message without an event type closes the connection of its sender, not the router |
| `oversize` | A header announcing a 768 MB message closes that client without allocating it, and another client still connects (Linux, no sanitizer) |
| `slow-reader` | A slow reader holds back only its own sender; a fast reader on the same send thread gets its messages at once, and the slow one loses none |
| `stop-while-flooding` | The router restarts while its clients flood each other, and serves new clients after each restart |

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
| `name-collision-provider` | Two role names with one number: the challenger is rejected and each source receives the name check of the other provider |
| `name-collision-consumer-provider-first` | A consumer of another role name with the provider's number does not connect; a consumer of the right role does |
| `name-collision-consumer-consumer-first` | The same, with the consumer registered before the provider |
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
| `RouterServiceRegistryTest` | `missing_endpoints_leave_registry_empty`, `provider_first_lifecycle`, `consumer_first_lifecycle`, `removing_last_waiting_consumer_erases_placeholder`, `disconnect_and_reregister_preserve_sibling`, `cookie_and_source_filters_are_distinct`, `source_wildcard_returns_all_endpoints`, `service_list_filters_by_cookie`, `unregister_of_an_unknown_provider_changes_nothing`, `disconnect_proxy_acts_only_on_a_registered_consumer`, `consumer_of_a_provider_that_is_not_connected_waits` |
| `RouterListServiceProxiesTest` | `unique_endpoints_selection_copy_move_and_availability` |
| `RouterRegistrationWireTest` | `factories_preserve_routing_identity_version_and_reason`, `local_sources_and_services_cannot_register_remotely` |

These retain the endpoint default/valid/invalid states, copy/move operations,
identity/hash contracts, connection transitions, duplicate registration,
replacement providers, pending/final-entry cleanup, distinct cookie/source
filters, list uniqueness and append/update counts. The wire cases check all eight
register/unregister request/notification factories, exact header fields,
header-only wire size, address round trips and rejection of local-only services.

`source_wildcard_returns_all_endpoints` asserts the `service_sources(COOKIE_ANY)` contract: one
registered provider and consumer are both returned with their original source IDs.

# Request Failures and Data Validity

A service call can fail, and the data a consumer holds can stop being valid. Areg never hides either: a failed request
is reported to its caller with a reason, and every attribute value comes with a state. This page lists every reason a
consumer can receive, what causes it, what the provider does to raise it, and what Areg does with a message it cannot
use.

---

## Table of Contents

1. [Where a Failure Arrives](#1-where-a-failure-arrives)
2. [Why a Request Fails](#2-why-a-request-fails)
3. [Who Is Told](#3-who-is-told)
4. [Attribute Data and Its State](#4-attribute-data-and-its-state)
5. [Invalid Responses and Broadcasts](#5-invalid-responses-and-broadcasts)
6. [Raising an Error in the Provider](#6-raising-an-error-in-the-provider)
7. [Messages That Are Dropped](#7-messages-that-are-dropped)
8. [All Result Values](#8-all-result-values)

---

## 1. Where a Failure Arrives

The generated consumer base class (`<Service>ConsumerBase`) has one method for each way a call can end. Override the
ones you need; each default writes a log line and does nothing else.

| Callback | Called when |
|---|---|
| `response_<name>(params)` | the request was answered |
| `request_<name>_failed(areg::ResultType reason)` | the request was not answered; `reason` says why (section 2) |
| `broadcast_<name>(params)` | a broadcast you subscribed to arrived |
| `on_<attribute>_update(value, areg::DataState state)` | an attribute you subscribed to changed, or became invalid (section 4) |
| `invalid_response(<Service>::ResponseIDs)` | a response arrived without its full data, or its data is invalid (section 5) |
| `invalid_broadcast(<Service>::BroadcastIDs)` | a broadcast arrived without its full data, or the provider reported it invalid (section 5) |
| `service_connected(status, proxy)` | the provider became reachable or went away |

`areg::as_string(reason)` gives the name of a result value for a log line.

```cpp
void request_read_file_failed(areg::ResultType reason) final
{
    if (reason == areg::ResultType::RequestBusy)
    {
        // the provider is still working on the previous call: try again later
    }
    else
    {
        LOG_WARN("read_file failed: %s", areg::as_string(reason));
    }
}
```

---

## 2. Why a Request Fails

A request that declares a response fails with exactly one of these reasons:

| Reason | Cause | Raised by | Try again? |
|---|---|---|---|
| `RequestBusy` | the provider has not answered an earlier call of the same response yet, and has not released it with `unblock_current_request()` | provider | yes, later |
| `RequestError` | the provider reported that it cannot complete the call | provider (section 6) | depends on the service |
| `RequestCanceled` | the provider canceled the call; a session released with `unblock_current_request()` is canceled too | provider (section 6) | depends on the service |
| `RequestUnsupported` | the provider has no request with this ID, or the request arrived without all its parameters | provider's generated code | no: it does not become known |
| `ServiceUnavailable` | the request was sent before the service was connected, or the service went away while the call was open | consumer's proxy | after `service_connected` reports `Connected` |
| `MessageUndelivered` | the message could not be passed on: the router connection is not up, a send failed, the provider is not started or already stopped, or the provider's event queue refused it | consumer's proxy, the router client, or the receiving process | yes, the condition may pass |

Notes:

- `RequestBusy` is the normal answer to a second call while the first is open. It is not an error of the service.
  See `docs/agent/30-provider.md` section 3 for how a provider lets the next caller in.
- `RequestUnsupported` means the two sides do not share one definition of the interface, or a foreign peer sent the
  message. Rebuild every process from one definition ([Service Message Identity](./06h-message-identity.md), section 5).
- `ServiceUnavailable` is reported once for every open call when the service disconnects. Nothing is retried for you.

---

## 3. Who Is Told

- **Only the caller.** A failure carries the sequence number of the call it answers and reaches only the consumer that
  made that call. Other consumers of the same service, and subscribers of the same response, are not told.
- **Only once.** After `request_<name>_failed`, the call is closed. No response follows for it.
- **Only requests that declare a response.** A request without a response is fire-and-forget: no caller waits for
  it, so nobody is told when it fails. The proxy logs a warning and drops the failure.
- **The response data is invalid.** After a failure, the response parameters the proxy keeps for that response are
  marked invalid.
- **Two requests, one response.** When two requests share a response, a failure of one calls only that request's
  `request_<name>_failed`.

---

## 4. Attribute Data and Its State

Every attribute value comes with an `areg::DataState`:

| State | Meaning |
|---|---|
| `DataIsOK` | the value is valid and current |
| `DataIsInvalid` | the provider marked the value invalid, or it arrived without its full data |
| `DataIsUnavailable` | no value is known: not subscribed, unsubscribed, or the service is not connected |
| `DataIsUndefined` | not used by an attribute |

`on_<attribute>_update(value, state)` is called with `DataIsOK` or `DataIsInvalid`. Read `state` before `value`: with
`DataIsInvalid`, `value` is the attribute's last value or a default, never data to act on.

| Event | What the subscriber receives |
|---|---|
| subscribe | first the value the proxy already holds (`DataIsInvalid` if it holds none), then the provider's current value: `DataIsOK`, or `DataIsInvalid` if the provider has not set it or invalidated it |
| the provider sets a new value (`set_<attribute>()`) | an update with `DataIsOK`, as the attribute's notification type says |
| the provider calls `invalidate_<attribute>()` | an update with `DataIsInvalid` |
| an update arrives without its full data | an update with `DataIsInvalid` |
| the provider has no attribute with this ID | one update with `DataIsInvalid`; the provider keeps no subscription |
| the service disconnects | **no update**: `service_connected` reports it, and the state becomes `DataIsUnavailable` |

After a reconnect, the proxy subscribes again by itself, and the first update brings the current value.

---

## 5. Invalid Responses and Broadcasts

`response_<name>` and `broadcast_<name>` run only with valid data. Instead, the consumer receives
`invalid_response()` or `invalid_broadcast()` with the entry, and the parameters the proxy keeps for it are marked
invalid, when:

- the provider reports a broadcast invalid: every subscriber of the broadcast receives `invalid_broadcast()`
  (section 6);
- a subscription names a response or a broadcast the provider does not have: the subscriber receives
  `invalid_response()` or `invalid_broadcast()` once, and the provider keeps no subscription;
- the payload is shorter than the parameter list. Such a message comes only from a peer built from another
  definition, or from a malformed message.

A broadcast has no caller, so an error of a broadcast is never a `request_<name>_failed`.

---

## 6. Raising an Error in the Provider

| To | Call | The caller receives |
|---|---|---|
| answer later and let other callers in | `unblock_current_request()`, then `prepare_response(session)` before `response_<name>()` | nothing yet; other calls are no longer `RequestBusy` |
| fail the call the next response would answer | `error_request(areg::CallType::Request, index, false)` | `RequestError` |
| cancel every open call of a request, released sessions included | `error_request(areg::CallType::Request, index, true)` | `RequestCanceled`, and the sessions become invalid: `prepare_response(session)` returns `false` |
| fail the callers waiting for a response | `error_request(areg::CallType::Response, index, false)` | `RequestError`, each with its own sequence number; a session released by `unblock_current_request()` is not failed: it stays valid and is answered by `prepare_response(session)` |
| mark a broadcast invalid | `error_request(areg::CallType::Broadcast, index, false)` | every subscriber: `invalid_broadcast()` |
| mark an attribute invalid | `invalidate_<attribute>()` | every subscriber: an update with `DataIsInvalid` |
| fail everything open | `error_all_requests()` | attributes invalid; every pending caller `RequestError`; released sessions stay valid |
| cancel everything open | `cancel_all_requests()` | every caller, released sessions included, `RequestCanceled` |

`index` is the entry's index in its kind, for example
`static_cast<uint32_t>(HelloWorld::RequestIDs::MsgId_request_hello_world)`. Subscriptions are never removed by an
error: a subscriber of a response does not receive `RequestError`, and keeps receiving the next responses.

An error answers the calls that are pending **now**. A call released with `unblock_current_request()` is a promise the
provider keeps later: only a cancel (`RequestCanceled`) ends it.

A provider that is stopped or unloaded does not answer open calls itself: its consumers see the service disconnect and
receive `ServiceUnavailable` for every open call.

---

## 7. Messages That Are Dropped

Some messages cannot be matched to anything a consumer or a provider is waiting for. They are dropped and a warning is
logged; the process keeps running.

| Message | Where it is dropped |
|---|---|
| a combination of event type and call type that generated code never produces | where the message enters the process |
| a request that has a response but carries no valid sequence number | provider |
| a response, broadcast or attribute update whose ID the consumer's interface does not have | consumer's proxy |
| a response with no waiting caller and no subscriber | provider |
| the failure of a request without a response | consumer's proxy |
| a response, broadcast or attribute update whose result value no provider sends for its kind, or a failure value on a response that names a waiting call | consumer's proxy |

---

## 8. All Result Values

`areg::ResultType` is shared by every service. The values an application meets:

| Value | Kind | Reaches the application as |
|---|---|---|
| `RequestOK` | success | `response_<name>()`, `broadcast_<name>()` |
| `DataOK` | success | `on_<attribute>_update(value, DataIsOK)` |
| `DataInvalid` | data | `on_<attribute>_update(value, DataIsInvalid)`, `invalid_response()`, `invalid_broadcast()` |
| `RequestInvalid` | data | `invalid_response()`, `invalid_broadcast()` |
| `RequestBusy` | failure | `request_<name>_failed()` |
| `RequestError` | failure | `request_<name>_failed()` |
| `RequestCanceled` | failure | `request_<name>_failed()` |
| `RequestUnsupported` | failure | `request_<name>_failed()` |
| `ServiceUnavailable` | failure | `request_<name>_failed()` |
| `MessageUndelivered` | failure | `request_<name>_failed()` |

The other values (`Undefined`, `Error`, `Undelivered`, `NotProcessed`, `Processed`, `ServiceOK`, `Invalid`,
`ServiceRejected`) are internal bit patterns or connection results and are never passed to these callbacks. A rejected
connection is reported by `service_connected` with `ServiceConnectionState::Rejected`.

---

## See Also

- **[Service Message Identity](./06h-message-identity.md)**: how a message finds its entry, and the unknown-ID answers
- **[Code Generator](./06b-code-generator.md)**: the generated provider and consumer classes
- **[Service Interface Design](./06e-lusan-service-interface.md)**: requests, responses, broadcasts and attributes

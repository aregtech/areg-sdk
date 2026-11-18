# Service Message Identity

Every request, response, broadcast and attribute of a service interface has a **message ID**: a 32-bit number that
travels in the header of every message and, together with the kind of the entry, tells the receiver which entry of the
interface the message belongs to. This page explains how the ID is made, how a process finds the entry of an ID, what
stops two entries from sharing one, and the one rule a distributed system built with Areg must follow.

---

## Table of Contents

1. [How an ID Is Made](#1-how-an-id-is-made)
2. [Collisions, and No Reserved Value](#2-collisions-and-no-reserved-value)
3. [What the Header Carries](#3-what-the-header-carries)
4. [An ID the Receiver Does Not Know](#4-an-id-the-receiver-does-not-know)
5. [One SDK, One Definition](#5-one-sdk-one-definition)

---

## 1. How an ID Is Made

The ID is the CRC-32C of the entry's kind and name, written exactly as the `.siml` document spells the name:

| Entry | Hashed string | Example |
|---|---|---|
| Request | `request_<Name>` | `request_identifier` |
| Response | `response_<Name>` | `response_identifier` |
| Broadcast | `broadcast_<Name>` | `broadcast_stateChanged` |
| Attribute | `attribute_<Name>` | `attribute_HelloServiceState` |

The code generator writes the string, and the C++ compiler computes the value. Every kind has an ID space of its own:
the entries of a kind are numbered `0, 1, 2 ...` in the order of the document, and that **index** is what the code inside
a process uses. The ID is what travels:

```cpp
enum class RequestIDs : uint32_t
{
      MsgId_request_identifier      // index 0
    , MsgId_request_hello_ublock    // index 1
};

constexpr uint32_t RequestIdList[]  // the ID of every request, by index
{
      areg::crc32_calculate("request_identifier")
    , areg::crc32_calculate("request_hello_ublock")
};

constexpr uint32_t message_id(RequestIDs msgId) noexcept;   // the ID of one request
```

`ResponseIDs`, `BroadcastIDs` and `AttributeIDs` follow the same pattern.

✅ The ID of an entry depends **only on its kind and its name**: not on its position in the document and not on the
other entries. Reordering the entries of a document does not change any ID; it changes the indexes, which never leave
the process.

✅ The service name is not part of the string. The same method name in two different interfaces has the same ID, which
is harmless: an ID is unique **within its kind and its service**, and the header carries the service identity beside it.

✅ No CRC is computed at run time. Where a message enters a provider or a consumer, the generated code finds the index
of the ID once, in a small table the code generator builds for every kind: one multiplication, one shift and two reads.
From there on the provider and the consumer work with the index: the listeners of an entry are one array element, and
the generated dispatch is a `switch` over consecutive values.

---

## 2. Collisions, and No Reserved Value

Two entries of **one kind** could, in theory, hash to the same value. That is refused three times:

- **Lusan** reports it while the document is edited (rule 61, "two entries of one kind with the same message ID").
- **codegen** refuses to generate the document and names both entries.
- the generated code checks it with one `static_assert` per entry:
  ```
  HelloUnblock: the request 'identifier' has the message ID of another request. Rename one of them.
  ```

The fix is to rename one of the two entries. For a kind of 100 entries the chance of a collision is about one in a
million.

Entries of **two kinds** may share an ID: a request and a broadcast with the same value are told apart by the kind the
header carries, and each is delivered to its own handler.

No ID value is reserved. `0`, `0x80000000`, `0xFFFFFFFF` and every value below `0x10000` are ordinary IDs of an entry:
the system messages of the framework carry a call type of their own and never meet an interface entry.

---

## 3. What the Header Carries

The value of an ID says nothing about its kind. Three header fields work together:

| Field | Says |
|---|---|
| `messageId` | **which** entry: the ID |
| `callType` | **what kind** of entry the ID is: request, response, broadcast, attribute, or a system message |
| `eventType` | **where** the message goes: to a provider or a consumer, local or remote |

A broadcast, an attribute update and the failure of a request all travel on the response channel to the consumer and
differ only by `callType`. The router and the client decide whether a message is a service call by one bit of
`callType`, never by the ID value.

A receiver accepts only the combinations of `eventType` and `callType` that generated code produces; any other message
is dropped before it reaches a provider or a consumer.

---

## 4. An ID the Receiver Does Not Know

A process never asserts on an ID that another process chose. When a message carries an ID its interface does not have:

| Receiver | What happens |
|---|---|
| Provider, request | the caller gets `request_<name>_failed(RequestUnsupported)` once, with its own sequence number |
| Provider, subscription | the subscriber gets one `DataInvalid` notification; no subscription is kept |
| Consumer, response / broadcast / attribute | the message is dropped and a warning is logged |

Such a message comes only from a malformed or foreign peer. The answers keep the receiver alive and leak nothing.

---

## 5. One SDK, One Definition

> ⚠️ **Every process of a system is built from one Areg SDK and from one definition of each service interface.**

This covers applications, `mtrouter`, `logcollector` and log observers alike.

- Processes built from different SDK versions are **not supported** and are **not detected** at run time.
- A provider and a consumer generated from different definitions of the same interface are **not supported**. Change
  the `.siml` document, regenerate, and rebuild every process that uses it.
- Code generated by an older `codegen.jar` does not compile against the current framework. Regenerating is the fix.

---

## See Also

- **[Code Generator](./06b-code-generator.md)**: generating service interface code from `.siml`
- **[Service Interface Design](./06e-lusan-service-interface.md)**: designing an interface in Lusan
- **[Multitarget Router](./03a-mtrouter.md)**: how messages travel between processes

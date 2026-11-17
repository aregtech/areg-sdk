# Service Message Identity

Every request, response, broadcast and attribute of a service interface has a **message ID**: a 32-bit number that
travels in the header of every message and tells the receiver which entry of the interface the message belongs to. This
page explains how the ID is made, what stops two entries from sharing one, and the one rule a distributed system built
with Areg must follow.

---

## Table of Contents

1. [How an ID Is Made](#1-how-an-id-is-made)
2. [Collisions and Reserved Values](#2-collisions-and-reserved-values)
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

The code generator writes the string, and the C++ compiler computes the value:

```cpp
enum class MessageIDs : MSG_ID
{
      MsgId_request_identifier  = areg::crc32_calculate("request_identifier")
    , MsgId_response_identifier = areg::crc32_calculate("response_identifier")
    , MsgId_HelloServiceState   = areg::crc32_calculate("attribute_HelloServiceState")
    ...
};
```

✅ The ID of an entry depends **only on its kind and its name**: not on its position in the document and not on the
other entries. Reordering the entries of a document does not change any ID.

✅ The service name is not part of the string. The same method name in two different interfaces has the same ID, which
is harmless: an ID is unique **within its service**, and the header carries the service identity beside it.

✅ No CRC is computed at run time. The value is a compile-time constant, so there is no cost on any message path and no
table in RAM.

---

## 2. Collisions and Reserved Values

Two entries of one interface could, in theory, hash to the same value, or an entry could hash to a value the system
reserves. Both **stop the build** of the generated code:

- one `static_assert` per entry checks that its ID is not reserved and occurs once in the interface. The message names
  the interface and the entry:
  ```
  HelloUnblock: the ID of request 'identifier' is reserved or equals the ID of another entry. Rename the entry.
  ```
- the generated lookup is a `switch` over every ID, so a duplicate is also a `duplicate case value` error on every
  compiler.

The fix is to rename one of the two entries. For an interface of 100 entries the chance of any collision is about one in
a million.

Reserved values:

| Value | Meaning |
|---|---|
| `0x00000000` | empty: no message, no response |
| `0x00000001` … `0x0000FFFF` | system messages (connection, registration, logging) |
| `0x80000000` | invalid message ID |
| `0xFFFFFFFF` | reserved |

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

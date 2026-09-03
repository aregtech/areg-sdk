# Define a service interface

A service interface is one XML document with the extension `.siml`. It is the
contract between a provider and its consumers. The generator turns it into two base
classes that you inherit.

Minimal document, complete and valid:

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<ServiceInterface FormatVersion="1.1.0">
    <Overview ID="1" Name="HelloService" Version="1.0.0" Category="Public">
        <Description>Greeting service.</Description>
    </Overview>
    <MethodList>
        <Method ID="2" Name="hello_service" MethodType="Request" Response="hello_service">
            <Description>Ask the provider to greet.</Description>
            <ParamList>
                <Parameter ID="51" Name="client" DataType="String">
                    <Description>Name of the caller.</Description>
                </Parameter>
            </ParamList>
        </Method>
        <Method ID="4" Name="hello_service" MethodType="Response">
            <Description>Result of the greeting.</Description>
            <ParamList>
                <Parameter ID="52" Name="success" DataType="bool">
                    <Description>True when the greeting was printed.</Description>
                </Parameter>
            </ParamList>
        </Method>
    </MethodList>
</ServiceInterface>
```

Place it under your project, for example `src/services/HelloService.siml`, and add
one line to the project's `CMakeLists.txt`:

```cmake
addServiceInterface(gen_myproject src/services/HelloService.siml)
```

---

## 1. Rules that make a document valid

- Every element carries an `ID` that is unique in the document. One counter, never
  reused. The value does not matter; the uniqueness does.
- `Category` decides how far the service reaches: `Private` inside one process,
  `Public` across processes on one machine, `Internet` across machines. A `Private`
  service cannot be reached through the router.
- A `Request` names the `Response` that answers it. The response must exist and be
  declared with `MethodType="Response"`. A request with no `Response` attribute is
  fire and forget.
- Two methods may share a name only when they are of different kinds. A request and
  its response usually carry the same name, as above.
- Every `DataType` you reference must be predefined (`bool`, `uint32`, `String`, ...),
  declared in this document's `DataTypeList`, or imported from a `.dtml` document.

The full grammar is `../../tools/schema/siml.xsd`. Its comments list the rules the schema
cannot express. Validation findings are reported with the numbers registered in
`../../tools/schema/rules.xml`.

---

## 2. What a document may contain

| Element | Purpose |
|---|---|
| `Overview` | Name, version, category, description. Mandatory. |
| `DataTypeList` | Enumerations, structures, containers, imported C++ types |
| `AttributeList` | Values the provider publishes and consumers subscribe to |
| `MethodList` | Requests, responses and broadcasts |
| `ConstantList` | Compile time constants shared by both sides |
| `IncludeList` | C++ headers or `.dtml` documents to pull in |

---

## 3. The generated API contract

**This table replaces reading the generated code.** Every name below is derived
mechanically from the document. `<Name>` is `Overview/@Name`.

Provider class inherits `<Name>ProviderBase`. Consumer class inherits `<Name>ConsumerBase`.

| In the `.siml` | The provider | The consumer |
|---|---|---|
| `Request foo(a)` | **must override** `void request_foo(a) final` (pure virtual) | **calls** `request_foo(a)` |
| ... and it fails | - | **may override** `void request_foo_failed(areg::ResultType reason) final` |
| `Response foo(b)` | **calls** `response_foo(b)` to answer | **may override** `void response_foo(b) final` |
| `Broadcast bar(c)` | **calls** `broadcast_bar(c)` | **may override** `void broadcast_bar(c) final`, **subscribes** with `notify_on_broadcast_bar(true)` |
| `Attribute Baz` of type `T` | **calls** `set_baz(value)`; also `is_baz_valid()`, `invalidate_baz()` | **reads** `baz(state)`, **may override** `void on_baz_update(T Baz, areg::DataState state) final`, **subscribes** with `notify_on_baz_update(true)` |
| `Constant Qux` | `<Name>::Qux` | `<Name>::Qux` |
| `DataType Enumeration E` | `<Name>::E`, plus `<Name>::as_string(E)` | same |
| `DataType Structure S` | `<Name>::S` | same |
| `Overview/@Name` | `<Name>::ServiceName`, `<Name>::InterfaceVersion` | same |

### The naming rule

| What | Transform | Example |
|---|---|---|
| Method name | to `snake_case`, with the prefix of its kind | `hello_service` -> `request_hello_service`, `response_hello_service` |
| Broadcast name | to `snake_case`, prefix `broadcast_` | `reached_maximum` -> `broadcast_reached_maximum` |
| Attribute name | to `snake_case` in every generated method | `StringOnChange` -> `set_string_on_change`, `on_string_on_change_update`, `string_on_change(state)` |
| Data type, structure, constant name | kept exactly as written | `sConnectedClient` -> `HelloWorld::sConnectedClient` |
| Parameter name | kept exactly as written | `StringOnChange` stays the parameter name in the handler |

A worked case, from the `PubSub` example. The document declares
`<Attribute Name="StringOnChange" DataType="String" Notify="OnChange"/>`, so:

```cpp
// areg-check: ignore
// provider
void set_string_on_change(const areg::String & newValue);
bool is_string_on_change_valid() const noexcept;
void invalidate_string_on_change();

// consumer
const areg::String & string_on_change(areg::DataState & state) const;
void notify_on_string_on_change_update(bool notify = true);
virtual void on_string_on_change_update(const areg::String & StringOnChange, areg::DataState state);
```

### `Notify` on an attribute

`OnChange` sends an update only when the value differs from the previous one.
`Always` sends one on every `set_`. Absent means `OnChange`.

---

## 4. Changing an existing interface

Regenerating is safe: nothing you wrote is touched, because your code is in separate
files. After a change, rebuild. The compiler reports every place that no longer
matches, because provider requests are pure virtual.

| Change | Effect on your code |
|---|---|
| Add a request | Provider stops compiling until you implement it |
| Remove a request | Provider stops compiling; delete your override |
| Change a parameter | Both sides stop compiling; fix the signatures |
| Add an attribute or broadcast | Nothing breaks; subscribe when you want it |
| Change `Version` | Consumers built against the old version stop connecting |

---

## 5. Before you move on

- [ ] Every `ID` in the document is unique.
- [ ] Every `Request` names an existing `Response`, or is deliberately fire and forget.
- [ ] `Category` matches the deployment: `Public` if the consumer is another process.
- [ ] The build ran the generator without a refusal.
- [ ] You did not open a generated file to learn a name.

Next: `30-provider.md` to implement the provider, `31-consumer.md` for the consumer.

# Data types

The same `DataTypeList` block appears in all three documents -- `.siml`, `.fsml` and
`.dtml` -- with the same elements and the same meaning. Learn it once here. A `.dtml`
carries that block and nothing else, so that several documents can share it.

## Where to declare a type

| Situation | The `design.json` key | Becomes |
|---|---|---|
| Used by one document | that document's own `"types"` | its own `DataTypeList` |
| Used by two or more | the project's `"datatypes"` | a `.dtml`, included by each |
| A C++ type you own | `"kind": "imported"` | `Type="Imported"`, not redeclared |

Sharing costs an include; duplicating a structure in two documents produces two
unrelated C++ types with the same field names, and they do not convert.

## The four kinds

`Type` takes `Structure`, `Enumeration`, `Imported` or `Container`. (`Enumerate` and
`DefinedType` are the 1.0.0 spellings of the last two; they are read, never written.)

```xml
<DataTypeList>
    <DataType ID="2" Name="PatientInfo" Type="Structure">
        <FieldList>
            <Field ID="3" Name="firstName" DataType="String"/>
            <Field ID="4" Name="age" DataType="uint32">
                <Value IsDefault="true">0</Value>
            </Field>
        </FieldList>
    </DataType>

    <DataType ID="5" Name="RunState" Type="Enumeration" Values="default">
        <FieldList>
            <EnumEntry ID="6" Name="Shutdown"><Value>0</Value></EnumEntry>
            <EnumEntry ID="7" Name="Ready"><Value>1</Value></EnumEntry>
        </FieldList>
    </DataType>

    <DataType ID="8" Name="ServiceAddress" Type="Imported">
        <Location>areg/component/ServiceAddress.hpp</Location>
        <Namespace>areg</Namespace>
        <ImportedObject>ServiceAddress</ImportedObject>
    </DataType>
</DataTypeList>
```

An `EnumEntry` with no `Value` continues from the previous entry, exactly as C++ does.
A `Container` carries `Container` plus `BaseTypeValue`, and `BaseTypeKey` as well for
the keyed ones (`HashMap`, `Map`, `Pair`).


## What the generator produces

Everything lands in the document's own namespace, `<Name>` being `Overview/@Name`.

| Declared | In C++ |
|---|---|
| `Structure PatientInfo` | `struct <Name>::PatientInfo` with a default and an all-field constructor, copy and move, `==` and `!=`, stream operators, and a `required_size` specialisation |
| `Enumeration RunState` | `enum class <Name>::RunState` plus `const char * <Name>::as_string(RunState)` |
| `Imported X` | the type you named; the generator only includes your header |
| `Container X` | an alias to the areg container of the declared element types |

### The operators every type carries

**Assume `=`, `==`, `!=`, `>>` and `<<` on every type a document can name.** Write
comparison and handler code with them and do not go looking for a declaration: the
predefined types below all carry them, and the generator writes them for every
`Structure` and `Container` you declare. `>>` and `<<` are `areg::InStream` /
`areg::OutStream` -- serialisation, not text formatting. An `Enumeration` gets a static
`as_string(value)` returning `const char *` in their place -- not an `areg::String`, so
nothing is called on its result. `String` and `WideString` also carry `+` and `+=`.

**This list is what you may assume, not the whole set.** An operator it does not name
is not thereby absent, so write the natural spelling and let the build answer. Code
that already compiles is never rewritten to avoid an operator that went unlisted.

A generated `==` compares field by field and a generated `<<` streams field by field,
so **a field of a `Type="Imported"` type must supply them itself.** It usually does;
when it does not, the compiler says so from inside the generated header. Define the
missing operator beside your own type -- never inside a generated file.

**Field and type names are kept exactly as written.** This is the one place the naming
rule differs from the rest of the document: methods and attributes become
`snake_case`, but `firstName` stays `firstName`. Copy the spelling from the document.

## Predefined type names

These come with the generator and need no declaration. Four of them are **not** named
in C++ the way the document names them, so take the spelling from this table rather
than guessing it. Authoritative source: `../../tools/schema/datatype.xml`.

| In the document | In C++ |
|---|---|
| `bool` `char` `float` `double` | the same name |
| `uint8` `int16` `uint16` `int32` `uint32` `int64` `uint64` | the `<cstdint>` name: `uint8_t` `int16_t` `uint16_t` `int32_t` `uint32_t` `int64_t` `uint64_t` |
| `String` `WideString` `DateTime` | `areg::String`, `areg::WideString`, `areg::DateTime` |
| `BinaryBuffer` | **`areg::SharedBuffer`** |
| `Array` | **`areg::ArrayList`** |
| `LinkedList` `HashMap` | `areg::LinkedList`, `areg::HashMap` |
| `Map` | **`areg::OrderedMap`** |
| `Pair` | **`areg::KeyValuePair`** |

The last row is what a `Container` may name; `HashMap`, `Map` and `Pair` are the keyed
ones and need `BaseTypeKey` as well as `BaseTypeValue`.

These are the **defaults, not the whole vocabulary**. A `DataType` attribute accepts
any name the document declares, and `Space::Type` for a name from an included
document. So a type that is not in the list above is not an error -- it just has to be
declared, usually as `Type="Imported"`. Note there is no predefined `int8`: declare it
if you need one.

Scalars are passed by value; `String`, structures and containers by `const T &`.

## The `.dtml` document

`gen_docs.py` writes it from the spec's `"datatypes"` section -- the root element, the
namespace and the `FormatVersion`, which is not the one a `.siml` carries -- and every
interface and machine of the same spec may then spell those types:

```json
{"datatypes": {"name": "SharedTypes", "declare": [
  {"name": "Quality", "kind": "enum", "values": {"Suspect": 0, "Good": 1}},
  {"name": "Reading", "kind": "struct",
   "fields": [{"name": "value", "type": "uint32"},
              {"name": "quality", "type": "Quality", "default": "Quality::Good"}]},
  {"name": "Firmware", "kind": "imported", "header": "areg/base/Version.hpp",
   "namespace": "areg", "object": "Version"},
  {"name": "History", "kind": "container", "container": "Array", "of": "Reading"}
]}}
```

`Overview/@Name` is the `Space` in `Space::Type`, the generated namespace and the
generated file name. It does not have to match the file name. A `.dtml` has no
attributes, no methods and no constants, and it may not include another `.dtml`.

Generating a document that includes a `.dtml` generates both: name the `.siml` or the
`.fsml`, and the shared types come with it.

## Including a shared document

A document that spells `Space::Type` is given the include automatically, so nothing is
written for it. A C++ header a declared type needs is a name in the document's
`"includes"` list. `IncludeList` carries both kinds. The path is spelled under the
workspace root.

A cycle, a missing file, or one name declared by two included documents is a numbered
rule -- ask `tools/explain_rule.py` what the number means instead of reading a schema.

## Never

- Never declare the same structure in two documents. Include one `.dtml` instead.
- Never re-case a field name in C++. It is generated as written.
- Never give a structure a field of a type nothing declares; an unresolved type is a
  generator error, not a compile error.
- Never wrap an existing C++ type in a new `Structure`. Use `Type="Imported"`.

## More

`../../tools/schema_help.py <name> --document dtml` reads out the grammar, which is
repeated verbatim inside `siml.xsd` and `fsml.xsd`. It answers what an element may
contain and nothing else; a spelling this page does not give is the only reason to ask,
and a refused document is `tools/explain_rule.py`.

A complete project with a shared document, generated, built and run by CI, is
`recipes/09-shared-types/`.

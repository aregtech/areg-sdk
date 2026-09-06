# Data types

The same `DataTypeList` block appears in all three documents -- `.siml`, `.fsml` and
`.dtml` -- with the same elements and the same meaning. Learn it once here. A `.dtml`
carries that block and nothing else, so that several documents can share it.

## Where to declare a type

| Situation | Put it in |
|---|---|
| Used by one document | that document's own `DataTypeList` |
| Used by two or more documents | a `.dtml`, included by each |
| Already a C++ type you own | `Type="Imported"` -- do not redeclare it |

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
| `Enumeration RunState` | `enum class <Name>::RunState` plus `<Name>::as_string(RunState)` |
| `Imported X` | the type you named; the generator only includes your header |
| `Container X` | an alias to the areg container of the declared element types |

### The operators every type carries

**Assume `=`, `==`, `!=`, `>>` and `<<` on every type a document can name.** Write
comparison and handler code with them and do not go looking for a declaration: the
predefined types below all carry them, and the generator writes them for every
`Structure` and `Container` you declare. `>>` and `<<` are `areg::InStream` /
`areg::OutStream` -- serialisation, not text formatting. An `Enumeration` gets
`as_string()` in their place.

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

A shared document is `DataTypeDocument`, its `Overview` names the namespace the types
land in, and **its `FormatVersion` is `1.0.0`** -- not the `1.1.0` a `.siml` and a
`.fsml` carry. A version the generator does not read is refused, not guessed at:
`error[50/RULE_FORMAT_VERSION]`, and nothing is generated.

```xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<DataTypeDocument FormatVersion="1.0.0">
    <Overview ID="1" Name="SharedTypes" Version="1.0.0"/>
    <DataTypeList>
        <!-- exactly the block above -->
    </DataTypeList>
</DataTypeDocument>
```

`Overview/@Name` is the `Space` in `Space::Type`, the generated namespace and the
generated file name. It does not have to match the file name. A `.dtml` has no
attributes, no methods and no constants, and it may not include another `.dtml`.

Generating a document that includes a `.dtml` generates both: name the `.siml` or the
`.fsml`, and the shared types come with it.

## Including a shared document

```xml
<IncludeList>
    <Location ID="9" Name="src/services/CommonTypes.dtml"/>
</IncludeList>
```

`IncludeList` carries both kinds of include: a C++ header a declared type needs, and a
`.dtml` document whose types this one uses. Types coming from an included document are
referred to as `Space::Type`. The path is spelled under the workspace root.

A cycle, a missing file, or one name declared by two included documents is a numbered
rule -- ask `tools/explain_rule.py` what the number means instead of reading a schema.

## Never

- Never declare the same structure in two documents. Include one `.dtml` instead.
- Never re-case a field name in C++. It is generated as written.
- Never give a structure a field of a type nothing declares; an unresolved type is a
  generator error, not a compile error.
- Never wrap an existing C++ type in a new `Structure`. Use `Type="Imported"`.

## More

`../../tools/schema/dtml.xsd` is the grammar, repeated verbatim inside
`../../tools/schema/siml.xsd` and `../../tools/schema/fsml.xsd`. It answers what an
element may contain and nothing else; a spelling this page does not give is the only
reason to open it, and a refused document is `tools/explain_rule.py`.

A complete project with a shared document, generated, built and run by CI, is
`recipes/09-shared-types/`.

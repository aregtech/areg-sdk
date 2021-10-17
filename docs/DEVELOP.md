```
This file is part of AREG IoT SDK
Copyright (c) 2017-2021, Aregtech
Contact: info[at]aregtech.com
Website: https://www.aregtech.com
```
============================================

# Developer guide

This document describes how to develop a service enabled application.

## Service interface prototype

_Service Interface_ is a base for any service. It defines the API of service. The interface contains following sections:
1. The general overview with the type of interface (_Public_ or _Local_).
2. The data types specific for the service or exported from any header file.
3. Attributes, which are the data of services and developers can describe to be notified on value change.
4. Methods, which are _requests_, _responses_ and _broadcast_.
5. Constant values, needed by interface.
6. Any additional file to include.

The service interfaces can be defined in prototype XML document. An example of the service intface prototype is [Sample.siml](./Sample.siml).

> 💡 There is as well a GUI tool to define an interface. The tool is in development and will be available in next release.

### Public and Local services

The following example specifies _Sample_ service as _Public_ (used in IPC).
```xml
<Overview ID="1" Name="Sample" Version="1.0.0" isRemote="true">
    <Description>This is an example of defining service interface.</Description>
</Overview>
```
Remove **isRemote** or set to false ```isRemote="true"``` for _Local_ service.

> 💡 The _Local_ services are not used in IPC, the system will ignore to forward messages and they will not be visible outside of process.


### Data types

There are several ways to specify service specific data types. When new data type is defined in prorotype document, the type can be used to declare any variables. The data types are listed in the section ```<DataTypeList> ... </DataTypeList>```. All data types are automatically become streamable objects.

#### Structures

The following example demostrates how to declare new structure with the fields. The default values are used to set when object is created. The fields of structure must have `assigning operator`, `comparing operator`, `copy constructor` (if an object), must be possible explicitely to convert to `unsigned int` and must be possible to `stream`.

```xml
<DataType ID="2" Name="SomeStruct" Type="Structure">
    <Description>Some structure with data. It will become new type.</Description>
    <FieldList>
        <Field DataType="int16" ID="3" Name="anyFiled1">
            <Value IsDefault="true">0</Value>
            <Description>Some filed 1</Description>
        </Field>
        <Field DataType="int32" ID="4" Name="anyField2">
            <Value IsDefault="true">0</Value>
            <Description>Just another field</Description>
        </Field>
        <Field DataType="String" ID="5" Name="anyField3">
            <Value IsDefault="true"/>
            <Description>More field.</Description>
        </Field>
    </FieldList>
</DataType>
```
In this example, the structure has 3 fields with default values to set when create the object.

#### Enumerations

The following example demonstrates how to declare enumeration. All enumeration objects are supposed to have `int` type.
```xml
<DataType ID="6" Name="SomeEnum" Type="Enumerate" Values="default">
    <FieldList>
        <EnumEntry ID="7" Name="Invalid">
            <Value>-1</Value>
        </EnumEntry>
        <EnumEntry ID="8" Name="Nothing">
            <Value>0</Value>
        </EnumEntry>
        <EnumEntry ID="9" Name="Something">
            <Value/>
        </EnumEntry>
        <EnumEntry ID="10" Name="Anything">
            <Value/>
        </EnumEntry>
    </FieldList>
</DataType>
```
 The enumeration fields may have explicit values. In this example, the `Invalid` fild has value `-1`.

 #### Imported types

 The following example demonstrates how to import an existing type and to use in the service. The specified `NEMemory::uAlign` is already existing in `areg/base/NEMemory.hpp`. It is imported to be used in the service.
 ```xml
<DataType ID="11" Name="uAlign" Type="Imported">
    <Description>Example of how to import existing types from other header files. This example exports NEMemory::uAlign</Description>
    <Namespace>NEMemory</Namespace>
    <Location>areg/base/NEMemory.hpp</Location>
</DataType>
```

#### Defined containers

The following example demonstrates how to define a container, which is a class template. The keys and the values of containers must be possible to copy, assign and compare. In case of hash-map, the key must be possible to convert to `unsigned int`.

**New array**:
```xml
<DataType ID="12" Name="SomeArray" Type="DefinedType">
    <Description>Defines new type of array</Description>
    <Container>Array</Container>
    <BaseTypeValue>uint32</BaseTypeValue>
</DataType>
```

**New linked list**:
```xml
<DataType ID="13" Name="SomeList" Type="DefinedType">
    <Description>New type of linked list.</Description>
    <Container>LinkedList</Container>
    <BaseTypeValue>String</BaseTypeValue>
</DataType>
```

**New hash-map**:
```xml
<DataType ID="14" Name="SomeMap" Type="DefinedType">
    <Description>An example of defining hash-map, where the key is exported value and the value is new defined type.</Description>
    <Container>HashMap</Container>
    <BaseTypeValue>SomeStruct</BaseTypeValue>
    <BaseTypeKey>String</BaseTypeKey>
</DataType>
```

### Attributes

_Attributes_ in service interface are data that other clients can subsribe to be notified when the value is changed or when it is set. The attributes are listed in the section `<AttributeList> ... <\AttributeList>`.

> 💡 Setting avalue to the attribute does not mean that it is changed.

In this example the system notifies connected clients when the value of `SomeAttr1` is changed.
```xml
<Attribute DataType="SomeEnum" ID="15" Name="SomeAttr1" Notify="OnChange">
    <Description>An attribute to notify subscribers only when value is changed.</Description>
</Attribute>
```

In this example, the system notifies clients each time when the value is set via `setter` method.
```xml
<Attribute DataType="SomeStruct" ID="16" Name="SomeAttr2" Notify="Always">
    <Description>Another attribute to notify subscribers any time when value is set (maybe not changed).</Description>
</Attribute>
```
### Requests, responses and broadcasts
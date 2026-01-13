# Creating Service Interface Documents with Lusan

This document explains how to create and maintain **Service Interface documents** using **Lusan**, the graphical design and diagnostics tool of the Areg SDK ecosystem. Service interfaces define the contract between **Service Providers** and **Service Consumers** and are the foundation for code generation, communication, and observability in AREG-based systems.

---

## What Is a Service Interface in Areg SDK?

A **Service Interface** describes how distributed components communicate. It defines:

- Service name and version
- RPC methods and their parameters
- Attributes and events
- Communication direction and access rules

Service interfaces are **implementation-independent**. They are used as input for the Areg code generator, which produces consistent and ready-to-use C++ code for both providers and consumers.

---

## Why Use Lusan for Service Interface Design?

While service interfaces can be authored manually, **Lusan provides a visual, structured, and validated workflow** that significantly reduces errors and accelerates development.

Key benefits:
- Visual modeling instead of manual editing
- Early validation of interface structure
- Guaranteed compatibility with the code generator
- Easier review and maintenance of interfaces over time

---

## Prerequisites

Before creating a service interface with Lusan, ensure that:

- Lusan is built and installed  
  👉 https://github.com/aregtech/areg-sdk-tools
- Areg SDK is available in your development environment
- Java is installed (required later for code generation)
- You have a basic understanding of services, methods, and events

---

## Creating a New Service Interface

### Step 1: Launch Lusan

Start the Lusan application from the build output or installation directory.

<!-- TODO: Insert screenshot of Lusan main window -->

---

### Step 2: Create or Open a Service Interface Document

From the main menu, create a **new service interface document** or open an existing one.  
Lusan manages service interface files in a structured, project-oriented way.

<!-- TODO: Insert screenshot of New Service Interface dialog -->

---

### Step 3: Define Service Metadata

Specify the core service information:
- Service name
- Version
- Optional namespace or grouping

This metadata uniquely identifies the service and ensures compatibility between providers and consumers.

<!-- TODO: Insert screenshot of Service Metadata editor -->

---

### Step 4: Add Service Methods (RPC)

Define **service methods** that represent callable operations:
- Method name
- Input parameters
- Output parameters
- Call semantics (synchronous / asynchronous)

Lusan enforces valid method definitions and prevents common design errors.

<!-- TODO: Insert screenshot of Method definition editor -->

---

### Step 5: Define Attributes and Events

Add **attributes** and **events** to model state and notifications:
- Attributes represent shared or observable state
- Events notify consumers about changes or occurrences

These elements enable reactive and event-driven communication patterns.

<!-- TODO: Insert screenshot of Attributes and Events editor -->

---

### Step 6: Validate and Save the Interface

Before saving, Lusan validates the service interface structure to ensure it is:
- Complete
- Internally consistent
- Compatible with the Areg code generator

Once validated, save the service interface document. It can now be used directly in the build process.

---

## Using the Service Interface in a Project

After saving the service interface document:
- Add it to your Areg-based project
- The **code generator (`codegen.jar`)** is automatically invoked during the build
- Generated C++ code is used to implement **Service Providers** and **Service Consumers**

This guarantees that runtime communication exactly matches the designed interface.

---

## Best Practices

- Design interfaces early, before implementation
- Keep interfaces stable and versioned
- Use attributes and events to avoid excessive polling
- Review interfaces visually in Lusan during code reviews

---

## Summary

Lusan provides a **clear, visual, and validated workflow** for creating service interfaces in Areg SDK. By designing interfaces graphically and generating code automatically, teams reduce manual errors, improve consistency, and accelerate development of distributed systems.

Service interfaces created with Lusan become the **single source of truth** for communication across the entire application.

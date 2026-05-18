"""Extract all class, struct, and enum names from Areg SDK headers.

Usage: python tools/extract_types.py
  Run from the repository root (C:\\projects\\areg-sdk).
  Scans framework/ for headers, writes results to product/.
"""
import re
import os

# Resolve paths relative to the repository root (parent of tools/)
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.dirname(SCRIPT_DIR)
FRAMEWORK_DIR = os.path.join(REPO_ROOT, "framework")
OUTPUT_DIR = os.path.join(REPO_ROOT, "product")

# Names to skip (internal artifacts, std specializations, C system types)
SKIP_NAMES = {
    "AREG_API", "AREG_API_IMPL", "hash", "_EmptyClass", "_name",
    "sockaddr_in", "tm", "timespec", "_EXCEPTION_POINTERS",
}

results = {"classes": [], "structs": [], "enums": [], "namespaces": []}
seen = {"classes": set(), "structs": set(), "enums": set(), "namespaces": set()}


def add_result(category, fpath, full_name, short_name, extra=None):
    if short_name in SKIP_NAMES:
        return
    if full_name in seen[category]:
        return
    seen[category].add(full_name)
    if extra is not None:
        results[category].append((fpath, full_name, short_name, extra))
    else:
        results[category].append((fpath, full_name, short_name))


for root, dirs, files in os.walk(FRAMEWORK_DIR):
    for f in files:
        if not (f.endswith(".hpp") or f.endswith(".h")):
            continue
        fpath_abs = os.path.join(root, f)
        fpath = os.path.relpath(fpath_abs, REPO_ROOT).replace("\\", "/")
        try:
            with open(fpath_abs, "r", encoding="utf-8", errors="ignore") as fh:
                lines = fh.readlines()
        except Exception:
            continue

        ns_stack = []
        ns_brace_depths = []
        std_depth = -1
        brace_depth = 0

        # For tracking typedef struct/enum that span multiple lines
        in_typedef_struct = False
        in_typedef_enum = False
        typedef_is_enum_class = False
        typedef_inner_name = None
        typedef_start_depth = 0

        for line in lines:
            stripped = line.strip()

            # Skip pure comment lines
            if stripped.startswith("//") or stripped.startswith("/*"):
                brace_depth += line.count("{") - line.count("}")
                continue

            # Track namespace opens (raw namespace keyword)
            ns_match = re.match(r"namespace\s+(\w+)", stripped)
            if ns_match:
                ns_name = ns_match.group(1)
                if ns_name == "std":
                    std_depth = brace_depth
                else:
                    ns_stack.append(ns_name)
                    ns_brace_depths.append(brace_depth)
                    # Record the namespace (full qualified path)
                    full_ns = "::".join(ns_stack)
                    if full_ns not in seen["namespaces"]:
                        seen["namespaces"].add(full_ns)
                        results["namespaces"].append((fpath, full_ns, ns_name))

            # Count braces
            brace_depth += line.count("{") - line.count("}")

            # Pop namespaces out of scope
            while ns_brace_depths and brace_depth <= ns_brace_depths[-1]:
                ns_brace_depths.pop()
                if ns_stack:
                    ns_stack.pop()

            # Check if std namespace closed
            if std_depth >= 0 and brace_depth <= std_depth:
                std_depth = -1

            # Skip anything inside std namespace
            if std_depth >= 0:
                continue

            ns_prefix = "::".join(ns_stack) + "::" if ns_stack else ""

            # ---- typedef struct ----
            ts = re.match(
                r"\s*typedef\s+struct\s+(?:AREG_API\s+|AREG_API_IMPL\s+)?(\w+)",
                stripped,
            )
            if ts:
                in_typedef_struct = True
                typedef_inner_name = ts.group(1)
                typedef_start_depth = brace_depth
                # Also record the inner name as a struct
                full = ns_prefix + typedef_inner_name
                add_result("structs", fpath, full, typedef_inner_name)

            # Check for closing "} alias_name;" of typedef struct
            if in_typedef_struct:
                close = re.match(r"\s*\}\s*(\w+)\s*;", stripped)
                if close and brace_depth <= typedef_start_depth:
                    alias = close.group(1)
                    full = ns_prefix + alias
                    add_result("structs", fpath, full, alias)
                    in_typedef_struct = False

            # ---- typedef enum ----
            te = re.match(
                r"\s*typedef\s+enum\s+(?:class\s+)?(\w+)",
                stripped,
            )
            if te:
                in_typedef_enum = True
                typedef_is_enum_class = "enum class" in stripped
                typedef_inner_name = te.group(1)
                typedef_start_depth = brace_depth
                tag = "enum class" if typedef_is_enum_class else "enum"
                full = ns_prefix + typedef_inner_name
                add_result("enums", fpath, full, typedef_inner_name, tag)

            # Check for closing "} alias_name;" of typedef enum
            if in_typedef_enum:
                close = re.match(r"\s*\}\s*(\w+)\s*;", stripped)
                if close and brace_depth <= typedef_start_depth:
                    alias = close.group(1)
                    tag = "enum class" if typedef_is_enum_class else "enum"
                    full = ns_prefix + alias
                    add_result("enums", fpath, full, alias, tag)
                    in_typedef_enum = False

            # Skip further matching if this line is a typedef (already handled above)
            if re.match(r"\s*typedef\s+", stripped):
                continue

            # ---- Regular class ----
            cm = re.match(
                r"\s*class\s+(?:AREG_API\s+|AREG_API_IMPL\s+)?(\w+)",
                stripped,
            )
            if cm and not stripped.rstrip().endswith(";"):
                name = cm.group(1)
                full = ns_prefix + name
                add_result("classes", fpath, full, name)

            # ---- Regular struct (not typedef, not forward decl) ----
            sm = re.match(
                r"\s*struct\s+(?:AREG_API\s+|AREG_API_IMPL\s+)?(\w+)",
                stripped,
            )
            if sm and not stripped.rstrip().endswith(";"):
                name = sm.group(1)
                full = ns_prefix + name
                add_result("structs", fpath, full, name)

            # ---- Regular enum (not typedef) ----
            em = re.match(r"\s*enum\s+(?:class\s+)?(\w+)", stripped)
            if em:
                name = em.group(1)
                if name == "class":
                    em2 = re.match(r"\s*enum\s+class\s+(\w+)", stripped)
                    if em2:
                        name = em2.group(1)
                    else:
                        continue
                is_class = "enum class" in stripped
                tag = "enum class" if is_class else "enum"
                full = ns_prefix + name
                add_result("enums", fpath, full, name, tag)

            # ---- AREG_DECLARE_EVENT macros ----
            # 3-arg: AREG_DECLARE_EVENT(DataClass, EventClass, ConsumerClass)
            #        AREG_DECLARE_EVENT_EX(DataClass, EventClass, ConsumerClass)
            #        AREG_DECLARE_EVENT_STANDARD(DataClass, EventClass, ConsumerClass)
            #        AREG_DECLARE_EVENT_EXTENDED(DataClass, EventClass, ConsumerClass)
            dm3 = re.match(
                r"\s*AREG_DECLARE_EVENT(?:_EX|_STANDARD|_EXTENDED)?"
                r"\s*\(\s*(\w+)\s*,\s*(\w+)\s*,\s*(\w+)\s*\)",
                stripped,
            )
            if dm3:
                evt_name = dm3.group(2)
                con_name = dm3.group(3)
                add_result("classes", fpath, ns_prefix + evt_name, evt_name)
                add_result("classes", fpath, ns_prefix + con_name, con_name)

            # 2-arg: AREG_DECLARE_INTERNAL_EVENT(EventClass, ConsumerClass)
            #        AREG_DECLARE_EXTERNAL_EVENT(EventClass, ConsumerClass)
            #        AREG_DECLARE_INTERNAL_EVENT_EXTENDED(EventClass, ConsumerClass)
            #        AREG_DECLARE_EXTERNAL_EVENT_EXTENDED(EventClass, ConsumerClass)
            dm2 = re.match(
                r"\s*AREG_DECLARE_(?:INTERNAL|EXTERNAL)_EVENT"
                r"(?:_EXTENDED)?"
                r"\s*\(\s*(\w+)\s*,\s*(\w+)\s*\)",
                stripped,
            )
            if dm2:
                evt_name = dm2.group(1)
                con_name = dm2.group(2)
                add_result("classes", fpath, ns_prefix + evt_name, evt_name)
                add_result("classes", fpath, ns_prefix + con_name, con_name)

# Ensure output directory exists
os.makedirs(OUTPUT_DIR, exist_ok=True)

# Write results
classes_file = os.path.join(OUTPUT_DIR, "areg_classes.txt")
with open(classes_file, "w") as f:
    f.write("# Areg SDK Classes\n")
    f.write(f"# Total: {len(results['classes'])}\n")
    f.write("# Format: FullName // file\n\n")
    for path, full, name in sorted(results["classes"], key=lambda x: x[1]):
        f.write(f"{full:<60s} // {path}\n")

structs_file = os.path.join(OUTPUT_DIR, "areg_structs.txt")
with open(structs_file, "w") as f:
    f.write("# Areg SDK Structs\n")
    f.write(f"# Total: {len(results['structs'])}\n")
    f.write("# Format: FullName // file\n\n")
    for path, full, name in sorted(results["structs"], key=lambda x: x[1]):
        f.write(f"{full:<60s} // {path}\n")

enums_file = os.path.join(OUTPUT_DIR, "areg_enums.txt")
with open(enums_file, "w") as f:
    f.write("# Areg SDK Enums\n")
    f.write(f"# Total: {len(results['enums'])}\n")
    f.write("# Format: FullName [type] // file\n\n")
    for path, full, name, tag in sorted(results["enums"], key=lambda x: x[1]):
        f.write(f"{full:<60s} [{tag:<10s}] // {path}\n")

namespaces_file = os.path.join(OUTPUT_DIR, "areg_namespaces.txt")
with open(namespaces_file, "w") as f:
    f.write("# Areg SDK Namespaces\n")
    f.write(f"# Total: {len(results['namespaces'])}\n")
    f.write("# Format: FullNamespace (Name) // first occurrence file\n\n")
    for path, full_ns, name in sorted(results["namespaces"], key=lambda x: x[1]):
        f.write(f"{full_ns:<60s} ({name:<30s}) // {path}\n")

total = len(results['classes']) + len(results['structs']) + len(results['enums'])
print(f"Classes:    {len(results['classes'])}")
print(f"Structs:    {len(results['structs'])}")
print(f"Enums:      {len(results['enums'])}")
print(f"Namespaces: {len(results['namespaces'])}")
print(f"Total:      {total} types + {len(results['namespaces'])} namespaces")
print(f"Written to: {OUTPUT_DIR}/areg_*.txt")

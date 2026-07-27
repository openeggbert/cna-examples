#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Check that the demo screens on disk, the catalog registrations, and the
counts written in README.md / plan.md all agree.

This exists because they silently drifted apart once already: plan.md claimed
12 Audio demo screens when there were 10, and nothing anywhere would have
noticed. With ~290 screens planned, a number in prose is a number nobody
re-derives by hand.

Checks performed:

  1. Every class defined in src/Demos/**/*Screen.hpp is either registered with
     MakeDemo<> in the catalog, or is a base class (something else derives
     from it). A screen that is written but never wired up is unreachable.
  2. Every MakeDemo<Class> registration has a class definition on disk.
  3. No class is registered twice (two menu entries running the same screen is
     almost always a copy-paste slip, not intent).
  4. The total demo count appears in README.md and plan.md.

Usage:
  tools/check_catalog.py            # check, exit non-zero on any disagreement
  tools/check_catalog.py --list     # also print the per-area inventory
"""

from __future__ import annotations

import collections
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEMOS_DIR = ROOT / "src" / "Demos"
CATALOG = ROOT / "src" / "Navigation" / "AreaCatalog.hpp"

CLASS_RE = re.compile(r"^class\s+(\w+)\s*:\s*public\s+(\w+)", re.MULTILINE)
NAMESPACE_RE = re.compile(r"^namespace\s+([\w:]+)\s*\{", re.MULTILINE)
BUILD_FN_RE = re.compile(r"^inline\s+std::vector<DemoEntry>\s+(\w+)\s*\(", re.MULTILINE)
USING_NS_RE = re.compile(r"using\s+namespace\s+([\w:]+)\s*;")
MAKEDEMO_RE = re.compile(r"MakeDemo<(\w+)>")

# Class names alone are ambiguous on purpose: Song and Video both have a
# LoadAndPlayScreen, and four Input categories each have a
# StateEqualityHashScreen. Everything below is keyed by the namespace-qualified
# name so those are distinct rather than colliding.


def collect_classes() -> tuple[dict[str, Path], set[str]]:
    """Return (qualified class name -> defining file, set of base class names)."""
    defined: dict[str, Path] = {}
    bases: set[str] = set()

    for path in sorted(DEMOS_DIR.rglob("*Screen.hpp")):
        text = path.read_text(encoding="utf-8")
        namespaces = NAMESPACE_RE.findall(text)
        namespace = namespaces[0] if namespaces else ""
        for name, base in CLASS_RE.findall(text):
            defined[f"{namespace}::{name}" if namespace else name] = path
            bases.add(base)
    return defined, bases


def collect_registrations() -> list[str]:
    """Qualified class names registered with MakeDemo<>, in catalog order.

    Each Build*Demos() function opens with the `using namespace` that its
    MakeDemo<> arguments resolve against, so the qualification is recovered by
    reading that per function rather than guessing from the class name.
    """
    text = CATALOG.read_text(encoding="utf-8")
    starts = [(m.start(), m.group(1)) for m in BUILD_FN_RE.finditer(text)]

    registrations: list[str] = []
    for index, (start, _name) in enumerate(starts):
        end = starts[index + 1][0] if index + 1 < len(starts) else len(text)
        body = text[start:end]

        using = USING_NS_RE.search(body)
        namespace = using.group(1) if using else ""
        for cls in MAKEDEMO_RE.findall(body):
            registrations.append(f"{namespace}::{cls}" if namespace else cls)
    return registrations


def area_of(path: Path) -> str:
    relative = path.relative_to(DEMOS_DIR)
    return relative.parts[0] if len(relative.parts) > 1 else "(shared)"


def main() -> int:
    if not DEMOS_DIR.is_dir() or not CATALOG.is_file():
        print("run this from a cna-examples checkout", file=sys.stderr)
        return 2

    defined, bases = collect_classes()
    registrations = collect_registrations()
    registered = set(registrations)

    problems: list[str] = []

    # 1. Defined but neither registered nor used as a base. Base classes are
    #    matched on their bare name, since a `: public Foo` reference is written
    #    unqualified when the base is visible via a using-directive.
    for name, path in sorted(defined.items()):
        if name in registered or name.rsplit("::", 1)[-1] in bases:
            continue
        problems.append(
            f"{name} ({path.relative_to(ROOT)}) is defined but never registered "
            f"with MakeDemo<> and nothing derives from it -- it is unreachable"
        )

    # 2. Registered but not defined anywhere.
    for name in sorted(registered - set(defined)):
        problems.append(f"MakeDemo<{name}> is registered but no *Screen.hpp defines it")

    # 3. Registered more than once.
    for name, count in sorted(collections.Counter(registrations).items()):
        if count > 1:
            problems.append(f"MakeDemo<{name}> is registered {count} times")

    total = len(registrations)

    # 4. The total must appear verbatim in the docs.
    for doc in ("README.md", "plan.md"):
        path = ROOT / doc
        if not path.is_file():
            problems.append(f"{doc} is missing")
            continue
        if str(total) not in path.read_text(encoding="utf-8"):
            problems.append(
                f"{doc} does not mention the current demo count ({total}) anywhere"
            )

    if "--list" in sys.argv:
        per_area: collections.Counter[str] = collections.Counter()
        for name in registrations:
            path = defined.get(name)
            if path is not None:
                per_area[area_of(path)] += 1
        print("Demo screens per area:")
        for area, count in sorted(per_area.items()):
            print(f"  {area:<12} {count}")
        print(f"  {'TOTAL':<12} {total}")
        base_names = sorted(n.rsplit("::", 1)[-1] for n in defined
                            if n.rsplit("::", 1)[-1] in bases)
        print(f"Base classes (not demos): {', '.join(base_names) or '(none)'}")
        print()

    if problems:
        print(f"{len(problems)} problem(s):")
        for problem in problems:
            print(f"  - {problem}")
        return 1

    print(f"OK: {total} demo screens, all defined, registered exactly once, and documented.")
    return 0


if __name__ == "__main__":
    sys.exit(main())

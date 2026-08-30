---
name: antelope-dev-style
description: >-
  Installs the Cursor rule "antelope-dev-style" for Antelope / EOSIO CDT C++
  smart contracts (actions, tables, comments, inline sends). NEVER use
  EOSLIB_SERIALIZE. Use when the user asks to install antelope dev style, add
  contract coding conventions to .cursor/rules or .cursorrules, or when Cogo is
  setting up an Antelope / EOSIO / CDT / eosio.cdt contract project.
---

# Antelope Dev Style

Installs (and then follows) the project Cursor rule **antelope-dev-style**.

This is **contract** C++ (CDT, `ACTION`, `TABLE`, WASM). Do not confuse with **antelope-core-dev** (node / spring / leap).

**NEVER use `EOSLIB_SERIALIZE`.** Do not write it on `TABLE`s, structs, nested types, or ABI mirrors you author. CDT serializes members in declaration order. Do not copy the macro from older headers (`takeiteasy.hpp`, `alcorswap_interface.hpp`).

## Install into the current project

Copy the rule file from this skill into the workspace:

1. Create `.cursor/rules/` if missing.
2. Write `.cursor/rules/antelope-dev-style.mdc` with the **exact contents** of [antelope-dev-style.mdc](antelope-dev-style.mdc) (frontmatter included). Overwrite if the user asked to update the rule.
3. If the project uses a root `.cursorrules` (Cogo), append a short pointer only if it does not already mention this rule:

```
Antelope / EOSIO contract C++: follow .cursor/rules/antelope-dev-style.mdc
```

4. Do not add the rule to unrelated (non-contract) apps unless the repo already contains `.cpp`/`.hpp` CDT contracts.

When Cogo is generating a **new** Antelope/EOSIO contract project, install this rule in Gate 3 **before** writing `.cpp`/`.hpp`, and list the path in the Gate 1 skeleton.

## After install (or if the rule already exists)

Read `.cursor/rules/antelope-dev-style.mdc` and apply it when creating or editing contract sources. Canonical examples that defined the style:

- Header + `TABLE` / `ACTION` / wrappers: `takeiteasy.hpp` (ignore any `EOSLIB_SERIALIZE` in that file)
- Action sections, `check` + emoji, `}//END name()`: `invitono.cpp`, `loot.cpp`
- Row suffix `_s` + `*_t` indexes: `loot.hpp`
- Self-contained ABI mirror + inline senders: `alcorswap_interface.hpp` (ignore any `EOSLIB_SERIALIZE` in that file)

## Do not

- **NEVER** use `EOSLIB_SERIALIZE` (or any manual serialize list). CDT serializes members in declaration order.
- Reorder or change types on deployed tables that already hold rows.
- Mix spring/leap node patterns into contract code.
- Invent a second style (Google C++, LLVM, etc.) when this rule is installed.

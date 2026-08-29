---
content_title: Keyd
---

## Introduction

`keyd` is a key manager service daemon for storing private keys and signing digital messages. It provides a secure key storage medium for keys to be encrypted at rest in the associated wallet file. `keyd` also defines a secure enclave for signing transaction created by `chan` or a third part library.

## Installation

`keyd` is distributed as part of the [Channel software suite](https://github.com/TetraGrids/The-Channel). To install `keyd` just visit the [Channel Software Installation](../00_install/index.md) section.

## Operation

When a wallet is unlocked with the corresponding password, `chan` can request `keyd` to sign a transaction with the appropriate private keys. Also, `keyd` provides support for hardware-based wallets such as Secure Encalve and YubiHSM.

[[info | Audience]]
| `keyd` is intended to be used by Antelope developers only.

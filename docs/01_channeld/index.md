---
content_title: Channeld
---

## Introduction

`channeld` is the core service daemon that runs on every Antelope node. It can be configured to process smart contracts, validate transactions, produce blocks containing valid transactions, and confirm blocks to record them on the blockchain.

## Installation

`channeld` is distributed as part of the [Channel software suite](https://github.com/TetraGrids/The-Channel). To install `channeld`, visit the [Channel Software Installation](../00_install/index.md) section.

## Explore

Navigate the sections below to configure and use `channeld`.

* [Usage](02_usage/index.md) - Configuring and using `channeld`, node setups/environments.
* [Plugins](03_plugins/index.md) - Using plugins, plugin options, mandatory vs. optional.
* [Replays](04_replays/index.md) - Replaying the chain from a snapshot or a blocks.log file.
* [RPC APIs](05_rpc_apis/index.md) - Remote Procedure Call API reference for plugin HTTP endpoints.
* [Logging](06_logging/index.md) - Logging config/usage, loggers, appenders, logging levels.
* [Concepts](07_concepts/index.md) - `channeld` concepts, explainers, implementation aspects.
* [Troubleshooting](08_troubleshooting/index.md) - Common `channeld` troubleshooting questions.

[[info | Access Node]]
| A local or remote Antelope access node running `channeld` is required for a client application or smart contract to interact with the blockchain.

# The Channel

The human-first blockchain, built on p2p trust, mining attention, and Pure Liquid Finance.

**The Channel** is a C++ implementation of the Channel protocol by **Tetra Grids**, derived from [Spring](https://github.com/AntelopeIO/spring) 2.0 (the [Antelope](https://github.com/AntelopeIO) protocol) with support for Savanna consensus. It contains blockchain node software, the core system contracts, and supporting tools for developers and node operators.

The Channel is everything Spring 2.0 was meant to be, plus:

- **Signing for many types** — K1, R1, WebAuthn, plus Wire **EM** (EIP-191) and **ED** (ed25519). Link an external key with `ra.authex` and unlock a deposit through `ra.claim`.
- **A human-first resource model** — Tonomy-style app registry and `loginwithapp` (per-app `user@app` keys). Contracts pay user-row RAM with `payer=get_self()`. Users pay CPU/NET, or rent via `ra.resources` / powerup. Block producers produce blocks.
- **The RAM market from [XPR Network](https://github.com/ProtonProtocol)** — fixed, governance-set RAM price (`setramoption`) and subscription CPU/NET rental. REX is disabled.
- **Dangling Spring features** — strong-QC mid-production apply and paginated `get_accounts_by_authorizers` are already in this tree. Larger unmerged Spring work (chaindb slab allocator, xxhash wrapper) is cataloged and not merged yet.

1. [Branches](#branches)
2. [System Token](#system-token)
3. [Core Contracts](#core-contracts)
4. [Supported Operating Systems](#supported-operating-systems)
5. [Binary Installation](#binary-installation)
6. [Build and Install from Source](#build-and-install-from-source)
7. [Bash Autocomplete](#bash-autocomplete)

## Branches
The `main` branch is the development branch; do not use it for production. Refer to the [release page](https://github.com/TetraGrids/The-Channel/releases) for current information on releases, pre-releases, and obsolete releases, as well as the corresponding tags for those releases.

## System Token
The default system token is **RA** (4 decimals), the equivalent of EOS on EOS, WAX on WAX, or TONO on Tonomy. RAM is sold at a governance-set fixed price (default `0.0001 RA` per byte, 10% fee, 8 MiB per-account cap; change with `setramoption`). Anyone may buy RAM; apps are expected to pay user-row RAM. The core symbol can be changed at build time with `-DCORE_SYMBOL_NAME`.

## Core Contracts
All core contracts ship in this repository under [`contracts/`](./contracts), renamed to the `ra.*` namespace:

| Contract | Purpose |
|---|---|
| `ra.bios` | Chain bootstrapping |
| `ra.boot` | Post-boot activation sequencing |
| `ra.system` | Staking, voting, producers, fixed-price RAM, app registry / `loginwithapp`, powerup |
| `ra.token` | Standard token (RA lives here) |
| `ra.msig` | Multisignature proposals |
| `ra.wrap` | Governance transaction wrapper |
| `ra.bpay` / `ra.fees` | Block pay and fee routing |
| `ra.resources` | Subscription CPU/NET rental (XPR-style; REX is off) |
| `ra.authex` | External-key linking for cross-chain identity |
| `ra.claim` | Relayer-credited deposit / user claim unlock |

After `ra.boot` is on `ra`, activate these Spring 2.0 features before deploying `ra.system` (these digests are Spring's, not Leap 4's):

| Feature | Digest |
|---|---|
| `RAM_RESTRICTIONS` | `1812fdb5096fd854a4958eb9d53b43219d114de0e858ce00255bd46569ad2c68` |
| `ONLY_BILL_FIRST_AUTHORIZER` | `2f1f13e291c79da5a2bbad259ed7c1f2d34f697ea460b14b565ac33b063b73e2` |
| `WEBAUTHN_KEY` | `927fdf78c51e77a899f2db938249fb1f8bb38f4e43d9c1f75b190492080cbc34` |
| `EM_ED_KEYS` | `dfc2e8e511691cb1e4c3e0792c48e2e882f74117e08b68b48f987903416bdd86` |

`RAM_RESTRICTIONS` is what makes `payer=get_self()` safe. `EM_ED_KEYS` raises supported key types to 5 so EM/ED work in authorities and in `recover_key`. Do **not** activate `FORWARD_SETCODE` if `ra` should keep intercepting `setcode`.

## Supported Operating Systems
We currently support the following operating systems.
- Ubuntu 22.04 Jammy

Other Unix derivatives such as macOS are tended to on a best-effort basis and may not be full featured. If you aren't using Ubuntu, please visit the "[Build Unsupported OS](./docs/00_install/01_build-from-source/00_build-unsupported-os.md)" page to explore your options.

If you are running an unsupported Ubuntu derivative, such as Linux Mint, you can find the version of Ubuntu your distribution was based on by using this command:
```bash
cat /etc/upstream-release/lsb-release
```
Your best bet is to follow the instructions for your Ubuntu base, but we make no guarantees.

## Binary Installation
This is the fastest way to get started. From the [latest release](https://github.com/TetraGrids/The-Channel/releases/latest) page, download a binary for one of our [supported operating systems](#supported-operating-systems), or visit the [release tags](https://github.com/TetraGrids/The-Channel/releases) page to download a binary for a specific version of The Channel.

Once you have a `*.deb` file downloaded for your version of Ubuntu, you can install it as follows:
```bash
sudo apt-get update
sudo apt-get install -y ~/Downloads/the-channel*.deb
```
Your download path may vary. If you are in an Ubuntu docker container, omit `sudo` because you run as `root` by default.

Finally, verify The Channel was installed correctly:
```bash
channeld --full-version
```
You should see a [semantic version](https://semver.org) string followed by a `git` commit hash with no errors. For example:
```
v1.0.0-dev1-8a7415a70f2b9c0d1e2f3a4b5c6d7e8f9a0b1c2d
```

## Build and Install from Source
You can also build and install The Channel from source.

### Prerequisites
You will need to build on a [supported operating system](#supported-operating-systems).

Requirements to build:
- C++20 compiler and standard library
- CMake 3.21+
- LLVM 7 - 11 - for Linux only
  - newer versions do not work
- libcurl 7.40.0+
- git
- GMP
- Python 3
- python3-numpy
- zlib

To also build the core contracts under `contracts/`, you will need [CDT](https://github.com/AntelopeIO/cdt) 3.x/4.x and to configure with `-DBUILD_CONTRACTS=ON`.

### Step 1 - Clone
If you don't have The Channel repo cloned to your computer yet, open a terminal and navigate to the folder where you want to clone The Channel repository:
```bash
cd ~/Downloads
```
Clone The Channel using either HTTPS...
```bash
git clone --recursive https://github.com/TetraGrids/The-Channel.git
```
...or SSH:
```bash
git clone --recursive git@github.com:TetraGrids/The-Channel.git
```

> ℹ️ **HTTPS vs. SSH Clone** ℹ️
> Both an HTTPS or SSH git clone will yield the same result - a folder named `The-Channel` containing our source code. It doesn't matter which type you use.

Navigate into that folder:
```bash
cd The-Channel
```

### Step 2 - Checkout Release Tag or Branch
Choose which [release](https://github.com/TetraGrids/The-Channel/releases) or [branch](#branches) you would like to build, then check it out. If you are not sure, use the [latest release](https://github.com/TetraGrids/The-Channel/releases/latest). In the example below, replace `v0.0.0` with your selected release tag accordingly:
```bash
git fetch --all --tags
git checkout v0.0.0
```

Once you are on the branch or release tag you want to build, make sure everything is up-to-date:
```bash
git pull
git submodule update --init --recursive
```

### Step 3 - Build
Select build instructions below for a [pinned build](#pinned-reproducible-build) (preferred) or an [unpinned build](#unpinned-build).

> ℹ️ **Pinned vs. Unpinned Build** ℹ️
> We have two types of builds for The Channel: "pinned" and "unpinned." A pinned build is a reproducible build with the build environment and dependency versions fixed by the development team. In contrast, unpinned builds use the dependency versions provided by the build platform. Unpinned builds tend to be quicker because the pinned build environment must be built from scratch. Pinned builds, in addition to being reproducible, ensure the compiler remains the same between builds of different Channel major versions. The Channel requires the compiler version to remain the same, otherwise its state might need to be recovered from a portable snapshot or the chain needs to be replayed.

> ⚠️ **A Warning On Parallel Compilation Jobs (`-j` flag)** ⚠️
> When building C/C++ software, often the build is performed in parallel via a command such as `make -j "$(nproc)"` which uses all available CPU threads. However, be aware that some compilation units (`*.cpp` files) in The Channel will consume nearly 4GB of memory. Failures due to memory exhaustion will typically, but not always, manifest as compiler crashes. Using all available CPU threads may also prevent you from doing other things on your computer during compilation. For these reasons, consider reducing this value.

> 🐋 **Docker and `sudo`** 🐋
> If you are in an Ubuntu docker container, omit `sudo` from all commands because you run as `root` by default. Most other docker containers also exclude `sudo`, especially Debian-family containers. If your shell prompt is a hash tag (`#`), omit `sudo`.

#### Pinned Reproducible Build
The pinned reproducible build requires Docker. Make sure you are in the root of the `The-Channel` repo and then run
```bash
DOCKER_BUILDKIT=1 docker build -f tools/reproducible.Dockerfile -o . .
```
This command will take a substantial amount of time because a toolchain is built from scratch. Upon completion, the current directory will contain a built `.deb` and `.tar.gz` (you can change the `-o .` argument to place the output in a different directory). If needing to reduce the number of parallel jobs as warned above, run the command as,
```bash
DOCKER_BUILDKIT=1 docker build --build-arg CHANNEL_BUILD_JOBS=4 -f tools/reproducible.Dockerfile -o . .
```

#### Unpinned Build
The following instructions are valid for this branch. Other release branches may have different requirements, so ensure you follow the directions in the branch or release you intend to build. If you are in an Ubuntu docker container, omit `sudo` because you run as `root` by default.

Install dependencies:
```bash
sudo apt-get update
sudo apt-get install -y \
        build-essential \
        cmake \
        git \
        libcurl4-openssl-dev \
        libgmp-dev \
        llvm-11-dev \
        python3-numpy \
        file \
        zlib1g-dev
```

To build, make sure you are in the root of the `The-Channel` repo, then run the following command:
```bash
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/usr/lib/llvm-11 ..

make -j "$(nproc)" package
```

Now you can optionally [test](#step-4---test) your build, or [install](#step-5---install) the `*.deb` binary packages, which will be in the root of your build directory.

### Step 4 - Test
The Channel supports the following test suites:

Test Suite | Test Type | [Test Size](https://testing.googleblog.com/2010/12/test-sizes.html) | Notes
---|:---:|:---:|---
[Parallelizable tests](#parallelizable-tests) | Unit tests | Small
[WASM spec tests](#wasm-spec-tests) | Unit tests | Small | Unit tests for our WASM runtime, each short but _very_ CPU-intensive
[Serial tests](#serial-tests) | Component/Integration | Medium
[Long-running tests](#long-running-tests) | Integration | Medium-to-Large | Tests which take an extraordinarily long amount of time to run

When building from source, we recommended running at least the [parallelizable tests](#parallelizable-tests).

#### Parallelizable Tests
This test suite consists of any test that does not require shared resources, such as file descriptors, specific folders, or ports, and can therefore be run concurrently in different threads without side effects (hence, easily parallelized). These are mostly unit tests and [small tests](https://testing.googleblog.com/2010/12/test-sizes.html) which complete in a short amount of time.

You can invoke them by running `ctest` from a terminal in your Channel build directory and specifying the following arguments:
```bash
ctest -j "$(nproc)" -LE _tests
```

#### WASM Spec Tests
The WASM spec tests verify that our WASM execution engine is compliant with the web assembly standard. These are very [small](https://testing.googleblog.com/2010/12/test-sizes.html), very fast unit tests. However, there are over a thousand of them so the suite can take a little time to run. These tests are extremely CPU-intensive.

You can invoke them by running `ctest` from a terminal in your Channel build directory and specifying the following arguments:
```bash
ctest -j "$(nproc)" -L wasm_spec_tests
```

We have observed severe performance issues when multiple virtual machines are running this test suite on the same physical host at the same time, for example in a CICD system. This can be resolved by disabling hyperthreading on the host.

#### Serial Tests
The serial test suite consists of [medium](https://testing.googleblog.com/2010/12/test-sizes.html) component or integration tests that use specific paths, ports, rely on process names, or similar, and cannot be run concurrently with other tests. Serial tests can be sensitive to other software running on the same host and they may `SIGKILL` other `channeld` processes. These tests take a moderate amount of time to complete, but we recommend running them.

You can invoke them by running `ctest` from a terminal in your Channel build directory and specifying the following arguments:
```bash
ctest -L "nonparallelizable_tests"
```

#### Long-Running Tests
The long-running tests are [medium-to-large](https://testing.googleblog.com/2010/12/test-sizes.html) integration tests that rely on shared resources and take a very long time to run.

You can invoke them by running `ctest` from a terminal in your Channel build directory and specifying the following arguments:
```bash
ctest -L "long_running_tests"
```

### Step 5 - Install
Once you have [built](#step-3---build) The Channel and [tested](#step-4---test) your build, you can install The Channel on your system. Don't forget to omit `sudo` if you are running in a docker container.

We recommend installing the binary package you just built. Navigate to your Channel build directory in a terminal and run this command:
```bash
sudo apt-get update
sudo apt-get install -y ./the-channel_*.deb
```

It is also possible to install using `make` instead:
```bash
sudo make install
```

## Bash Autocomplete
`chan` and `channel-util` offer a substantial amount of functionality. Consider using bash's autocompletion support which makes it easier to discover all their various options.

For our provided `.deb` packages simply install Ubuntu's `bash-completion` package: `apt-get install bash-completion` (you may need to log out/in after installing).

If building from source install the `build/programs/chan/bash-completion/completions/chan` and `build/programs/channel-util/bash-completion/completions/channel-util` files to your bash-completion directory. Refer to [bash-completion's documentation](https://github.com/scop/bash-completion#faq) on the possible install locations.

## Acknowledgements
The Channel is built by [Tetra Grids](https://github.com/TetraGrids) standing on the shoulders of:

- [Spring](https://github.com/AntelopeIO/spring) — the Antelope protocol node software (base: Spring 2.0.0-dev)
- [Antelope reference contracts](https://github.com/AntelopeIO/reference-contracts) — the core system contracts, renamed `ra.*`
- [Wire Network](https://github.com/Wire-Network) — signing for many types
- [Tonomy](https://github.com/Tonomy-Foundation) — the human-first resource model
- [XPR Network / Proton](https://github.com/ProtonProtocol) — the RAM market

All upstream code is used under its original open-source licenses (see [LICENSE](./LICENSE) and the per-library license files).

---

<sub><sup>The Channel is the channel in which attention flows. The tetrahedron is the primary force carrier of the universe; the other geometric solids are higher versions of this base encoding.</sup></sub>

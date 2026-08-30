# The Channel

**A Human-First Blockchain for Peer-to-Peer Trust, Attention, and Pure Liquid Finance**

Douglas Butner  AKA Gudasol (🜛)
Tetra Grids, LLC  (⟁)
Maryland, United States

v0.3.0

**Important Notice.** This paper is a living reference for what The Channel includes and will include: node software, system contracts, Pure Liquid Finance, and the Tetra layer documented at [know.tetra.earth](https://know.tetra.earth). It will be updated as those pieces land in code.

---

## Abstract

The Channel is a human-first layer-1: named accounts, scoped permissions, and a resource model people can use, with **Pure Liquid Finance** as a native economic primitive. The node software is a C++ implementation of the Channel protocol by Tetra Grids, derived from Spring 2.0 (the Antelope protocol) with Savanna consensus.

Pure Liquid tokens vault one hundred percent of supply into **Uniswap v3-style concentrated-liquidity (CLMM)** pools and **lock** those positions. Circulating units exist only because someone bought them out of those pools. Rewards are a transfer fee, not inflation. The in-tree reference is [`contracts/ra.pure`](./contracts/ra.pure). A prior Pure Liquid experiment on XPR became the **#1 volume community project on Antelope** and the **#1 locked stablecoin liquidity** on that network.

Every bridged asset on The Channel gets a **pure liquid twin** with flex-token technology: locked CLMM inventory as the redemption guarantee, plus a holder income stream payable in **any token on the network**. Combined with **systemic CLMM pools**, that is always-on liquidity for the underlying. **Half of the fees** return to **`ra.org` backpay**—block producers and everyone else the chain owes—through a **decentralized proposal system**. Assets join the set **democratically**: community commitment is measured by **liquidity posted on the other native chains**, not by a committee picking tickers.

Weight is not one-token-one-vote and not “one account one human.” Each verified person starts with a base. Large RA holdings add only a little. Additional weight comes from RA held, CPU actually burned on-chain, and the **graph of social verification**. That is the spirit of quadratic funding [24] without the false assumption that every voting key is a person because it can sign.

Interest-bearing debt is a **mathematically imbalanced game**: the interest due cannot exist as money at the same time as the principal [29][30]. The Channel is built so the **next game of Earth** can unfold—time-based, abundant, in resonance with gifts like the Sun’s continual life—not a closed loop of compounding that can never be repaid. Time-token and biomimetic systems from the 2020 Web 4 work [31] are reflected here. The application layer is Tetra: **Passport**, **Discovery**, **Recurve**, **Localize**, and seasonal **Carnivals** [25].

---

## 1. Introduction

Blockchain promised better money and better coordination. For that to hold, a chain has to match centralized systems on speed and price, then add what they cannot: custody without a bank, rules without a clerk, and tokens whose backing is checkable. Combined public-chain throughput still struggles to host a mainstream application. Worse, the apps that ship fail on humans: keys that look like noise, gas that surprises, liquidity that can be pulled, and “backing” that is a screenshot.

The Channel is built from the account up. It is ordinary node software (`channeld`, `chan`, `keyd`) and core contracts in the `ra.*` namespace. Tetra Grids will run it; others can run it independently.

The wider setting is not only “better DeFi.” Nearly all money is created as interest-bearing debt. In a closed system the stock of money is less than principal plus interest at once; some debtors must fail the musical chairs no matter how productive they are [29][30]. Governments know simpler remedies (jubilee, principal/interest split, write-down) and will not run them at scale: **inertia in the incumbent system**. The Channel is a parallel settlement layer that can **kickstart a global economic reset** because financial energetics will rebalance whether or not treasuries vote to [32].

Five commitments:

1. **Human-first accounts.** Named accounts, scoped permissions, WebAuthn and external-wallet keys, apps that pay the RAM their users’ rows occupy.
2. **Honest resources.** RAM at a governance-set fixed price; CPU/NET that users rent or stake; producers that produce blocks.
3. **Pure Liquid Finance on CLMM.** Full supply locked in Uniswap v3 ranges against a real counter-asset. Every bridged token gets a pure liquid version with flex-token technology. Transfer energy is redistributed; it is not printed. Half of pool fees pay the people who keep the chain alive.
4. **Attention as a first-class layer.** Tetra turns daily RA, offers, regional charts, fractal elections, and carnivals into the reason to be on the chain at all.
5. **Abundance and time.** Foundations of a society in resonance with systems of abundance—like the Sun’s continual gift of life—and with **time-based** accounting, not compounding interest on a finite token stock [31].

---

## 2. Background and Related Work

Bitcoin [1] and Ethereum [2] proved scarcity and general-purpose contracts. They also proved that poor accounts, expensive storage, and wrapped assets will not replace Web 2.0 by themselves.

The five failures we treat as primary:

1. **Decentralization** — block production and finality are not one operator.
2. **Security** — integrity of state *and* of advertised liquidity.
3. **Scalability** — throughput that can host real applications.
4. **Onboarding** — a path that looks like a login.
5. **Interoperability** — identity and value can move without a custodian as the only option.

### 2.1 Decentralization and security

Decentralization is scored by how production and finality are distributed. Concentration invites censorship, denial of service, or history edits [3]. Ethereum has faced builder/relay censorship [4][5]. Bitcoin remains a high-water mark for settlement security [7] at the cost of throughput.

Delegated Proof-of-Stake raised performance [8] and also concentrated operational power in a small producer set [9][10][11]. Antelope-family chains inherit a producer schedule: on the order of twenty-one producers, 500 ms slots, twelve-block rounds. **Savanna** (Spring 2.0, The Channel) adds **finalizers** who vote with BLS signatures and form a quorum certificate. Production and finality are different jobs.

Token security is a second axis. A mint with an admin key, or a CLMM position the team can `subliquid`, is not a reserve. Bridge hacks showed that moving assets off native consensus is a nine-figure surface [14][15][16]. Pure Liquid Finance does not solve bridging. It makes a narrower claim: **the reserve is the locked CLMM position**. Explorers read the pool, the position, and the lock expiry.

### 2.2 Cross-chain

Bridges and oracles reintroduce custody [12][13]. Atomic swaps avoid a custodian but scale poorly for ordinary users [17][18]. The Channel’s first step is **many key types on one account**, plus:

- `ra.authex` — a Channel account attests control of an address on another chain.
- `ra.claim` — a deposit seen elsewhere is credited; the user unlocks RA from a pre-funded treasury.

Version 1 of `ra.claim` uses governance-registered relayers. Later versions can bind claims to EM/ED signatures once `EM_ED_KEYS` is active. Relayers are trusted until that ladder is climbed; rotate them, keep the treasury equal to unclaimed credits.

### 2.3 Scalability

Bitcoin’s ~7 TPS made “scale later” a founding myth. Antelope-style chains already occupy a different point: WASM contracts, 0.5 s blocks, resource limits instead of a global gas auction for every click. The Channel keeps that engine and Savanna finality. Vertical scaling (hardware and on-chain limits) is the near-term lever.

### 2.4 Onboarding

Keys, hex addresses, and gas bounce non-specialists [19]. Named accounts and hierarchical permissions were the right idea. `loginwithapp` gives each app a permission, not a copy of `owner`. WebAuthn is a device-bound credential on-chain. EM (EIP-191) and ED (ed25519) let Ethereum and Solana wallets sign Channel transactions.

### 2.5 Related economic designs

**THORChain.** Continuous liquidity pools provide always-on exchange, on-chain prices, and fee revenue that is split between liquidity providers and node operators via an incentive pendulum [22][23]. The Channel takes the *systemic* idea—protocol pools that pay the people who secure and operate the network—and implements the pools as **Uniswap v3 CLMM**, not as constant-product \(x \cdot y = k\). Fees flow into **backpay**, not only into a bond-vs-pool ratio.

**Uniswap v3.** Concentrated liquidity into ticks [28]. A position is a range, not a fungible LP mint. That is what makes locking a named vault possible.

**Pure Liquid in production.** The same vault-and-reflection design was run as a community project on XPR / Antelope. It became the **#1 volume community project on Antelope** and held the **#1 locked stablecoin liquidity** on XPR. `ra.pure` is The Channel’s reference for that token class.

**Web 4 (2020).** Time tokens, proof of individuality, information entropy, and geosocial systems—biomimetic modules for collaboration in harmonic resonance with natural cycles (the Sun’s abundance, rising entropy, one human one account) [31]. The Channel reflects those time-based systems in daily RA, decaying social graphs, Carnival seasons, and flex rewards that are a flow, not a print.

**Quadratic funding.** Gitcoin-style matching weights *breadth* of support over whale size [24]. The failure mode is sybil: if keys are cheap, “many small contributors” are many bots. The Channel keeps the spirit (do not let capital dominate) and scores **verified humans**, not lungs attached to a keypair.

**Global debt reset.** Dual-asset and time-based sketches for separating principal from interest so the circuit can clear without requiring money that was never issued [32]. Section 13 is how The Channel operationalizes that necessity.

---

## 3. Layer-1 Consensus

The Channel runs **Spring 2.0 Savanna** on an Antelope producer schedule.

1. **Block production.** A scheduled producer builds a block every 500 ms. Each producer produces a round of 12 blocks. A missed slot is a 0.5 s gap. Early header-validated propagation and round-internal scheduling are inherited from Leap/Spring.
2. **Finality (Savanna).** Finalizers vote with BLS keys. Quorum certificates make blocks irreversible under Savanna’s fork-choice and safety rules. Snapshot format v7+ in this repository carries Savanna state (finalizer policies, QC claims).

Producers and finalizers are distinct. Token-weighted voting still seats producers (`ra.system`). Finalizer keys are registered on-chain. The set that proposes blocks is not automatically the set that finalizes them.

- Block interval: **500 ms**.
- Producer repetitions per round: **12**.
- `max_block_cpu_usage` and `max_block_net_usage` remain consensus parameters.

Finality latency depends on finalizer policy, network, and QC aggregation. Measure it on the running network.

---

## 4. RA, Resources, and Who Pays

**RA** (4 decimals, overridable with `-DCORE_SYMBOL_NAME`) lives in `ra.token`. It meters RAM, `ra.resources` plans, and system accounting. RA is not itself a Pure Liquid token.

On Antelope, **RAM** is state and **CPU/NET** is compute and bandwidth. The Channel prices RAM at **0.0020 RA / byte** (10% fee by default), with age-based purchase caps (4 MiB in year one, +2 MiB per year, hard cap 22 MiB) and a 4444-byte gift at account creation. Anyone may buy RAM; **apps are expected to pay user-row RAM** (`payer = get_self()` once `RAM_RESTRICTIONS` is active). A Bancor path (`buyramsys` / `sellramsys`) remains for the permission-gated system market. REX is disabled. `ra.resources` is subscription rental (prefund with memo `fund`).

| Cost | Who pays |
|------|----------|
| User-row RAM for an app’s tables | The app |
| CPU/NET for a user’s click | The user, a `ra.resources` plan, or powerup |
| Block production and org backpay | **CLMM pool fees** into `ra.org` / `ra.bpay` (see §6), plus any configured inflation |
| Tetra daily RA | Local stake and level rules (see §8–§12) |

Protocol features (`RAM_RESTRICTIONS`, `ONLY_BILL_FIRST_AUTHORIZER`, `WEBAUTHN_KEY`, `EM_ED_KEYS`) are activated after `ra.boot`. `FORWARD_SETCODE` should stay off if `ra` is to keep intercepting `setcode`.

Sensitive system actions go through `ra.msig` and `ra.wrap`. RAM price, fee, and ceilings are governance parameters (`setramoption`).

---

## 5. Pure Liquid Finance (CLMM)

How The Channel makes **purely liquid-backed tokens** using **Uniswap v3 concentrated-liquidity pools with locked assets**. Reference: [`contracts/ra.pure`](./contracts/ra.pure) and [`contracts/ra.pure/alcor-exchange/alcorswap_interface.hpp`](./contracts/ra.pure/alcor-exchange/alcorswap_interface.hpp).

The Channel does **not** use constant-product AMM pools (\(x \cdot y = k\) across all prices) as the vault. Vaults are **CLMM ranges**: ticks, `sqrtPriceX64`, positions, and locks.

### 5.1 Definition

A token is **pure liquid** when:

1. **Full-supply vault.** At genesis, 100% of `max_supply` is placed into one or more CLMM positions as the token side of the pool.
2. **Lock.** Those positions are locked (`lockpos`) so `subliquid` and `transferpos` fail until `unlockTime`. A lock may be **extended**, never shortened. There is no admin “unlock now.”
3. **Circulation = purchase.** Tokens leave the vault only when a trader swaps counter-asset in. Distributions are **transfer fees**, not unbacked mints.
4. **Redeemability.** Remaining counter-asset in the locked positions is the backing. Selling back into the same pools is redemption.

Tokens with burns and farms but no full vault are not pure liquid. Do not blur that line.

### 5.2 Why Uniswap v3 CLMM

Uniswap v3 puts inventory on a price interval \([p_a, p_b]\) [28].

- A full-range constant-product launch priced too cheap creates a pump-and-dump: a tiny buy marks a huge fully diluted value on inventory that was never paid for at that price.
- A **ranged vault** sets a floor and a ceiling in market-cap space so the first buy is a gift, not a steal, with inventory left for a long curve.
- Inside the range, a dollar of liquidity is many times as deep as the same dollar spread across all prices. Outside the range, the position is one-sided and earns no swap fees until price returns.
- A position is identified by `(poolId, owner, tickLower, tickUpper)`. That is what locking a named vault requires.

Pool state (v3-style):

- `sqrtPriceX64` (Q64.64), current tick, fee, tick spacing, global fee growth, liquidity.
- Positions: liquidity, uncollected `feesA` / `feesB`.
- Locks: `pos_id` → `unlockTime`. **Absent row means unlocked.**

Ticks must be multiples of `tickSpacing`, `tickLower < tickUpper`, within ±443636. \(\mathrm{sqrtPriceX64} = \sqrt{1.0001^{\mathrm{tick}}} \cdot 2^{64}\). Launchers typically precompute that off-chain.

### 5.3 What the lock does

| Allowed while locked | Forbidden while locked |
|----------------------|-------------------------|
| `collect` (harvest swap fees) | `subliquid` |
| Trading *against* the pool | `transferpos` |
| Extending `unlockTime` | Shortening `unlockTime` |

Fees still flow from a vault nobody can rug. Harvested fees route to reflections, buybacks, and **protocol backpay** (§6) without reclaiming core inventory.

**One-sided launch.** If the range sits entirely above or below the current tick, `addliquid` consumes only one side. The market supplies the counter-asset as people buy in.

### 5.4 Launch sequence

From a contract with `eosio.code` on `active`:

1. `forge` + `mint` 100% of supply.
2. `createpool(tokenA, tokenB, sqrtPriceX64, fee)` — tokens sorted by contract, then symbol.
3. Activate the pool if the swap contract charges a fee.
4. `deposit` both sides.
5. `addliquid(..., tickLower, tickUpper, ...)`.
6. `lockpos(..., unlockTime)`.

`addliquid` / `lockpos` cannot see a `poolId` created in the same action’s inline stream until that action returns.

```text
  max supply ──mint──► launcher
                         │
                         ├─ createpool
                         ├─ deposit A, deposit B
                         ├─ addliquid  [tickLower, tickUpper]
                         └─ lockpos    until unlockTime
                                │
                                ▼
                     CLMM vault (locked)
                      ▲              │
            counter-asset in     token out
            (backing)            (circulation)
                      │              ▼
                   traders ◄──── swaps ────► holders
                                                │
                                         transfer tax
                                                ▼
                                         reflection pool
                                                │
                                            reflect()
                                                ▼
                                         holders / chosen reward token
```

### 5.5 `ra.pure`

**Lifecycle.** `forge`, `mint` (issuer only), `smelt` (burn), `open` / `close`.

**State.** `supply`, `max_supply`, `issuer`, `reflection_pool`, `burn_pool`, `project_pool`.

**Flexers.** `owner`, `balance`, `is_banned`, `flextoken`, `tree` + `tree_rate`, `custom_memo`.

**Rates.** Basis points / 10000 on a settings singleton, with pagination `start_key` / `limit`.

**Transfer tax.** Fees book into the three pools unless the sender is the contract (distribution) or banned. DEX inventory is subtracted from the reflection denominator so locked vault balances do not soak holder yield.

**`reflect`.** Permissionless. Fibonacci-smoothed slices so one click does not empty the pool. Inheritance split by `tree_rate`. Optional swap into a registered reward token via the CLMM (`interestoken`). Then burn and project legs.

**Inheritance.** `inheritance` / `inheritmemo`. Memo substitutes `@@`, `$$`, `**`.

**Fee hook.** Collected CLMM fees (`Col…` memos in the reference) can be forwarded into the human budget that pays contributors and, at protocol scale, into **`ra.org`**.

**Renounce.** Self-ban from tax/reflections; unban is contract-auth only.

Pure liquid backing is not a bank balance, an algorithmic mint, a fractional reserve, or a custodian attestation. It is locked CLMM inventory plus the counter-asset swaps have left in the curve.

### 5.6 Bridged assets, flex technology, democratic listing

The Channel does not wrap foreign tokens as an IOU with a hope of exit liquidity. For **each bridged token**, the protocol mints a **pure liquid version** and applies **flex-token technology**: transfer-tax reflections, holder-chosen reward token (any listed asset on the network), inheritance, and a locked Uniswap v3 range against the underlying.

Together with **systemic CLMM pools** (§6):

- **Redemption guarantee.** Holders can sell back into the locked range. The underlying is the inventory in the vault, not a bridge operator’s promise after the fact.
- **Income in any token.** Reflections and harvested swap fees can be flexed into whichever listed asset the holder chooses.
- **Half the fees to backpay.** Of CLMM and flex-fee flow, **50%** funds `ra.org` / `ra.bpay` (producers and all approved creditors). The other half stays with holders (and the vault’s own deepening), so the people who hold the liquid twin and the people who run the chain are paid from the same volume.

**Listing is democratic.** An asset is added when the community **commits liquidity on the other native chain**—locked or bonded depth there, not a foundation vote and not a market-cap screenshot. Commitment of real inventory on the home chain is the signal. That is the same spirit as proposal weight: skin in the game that is hard to fake with empty accounts.

---

## 6. Systemic CLMM Fees, Backpay, and Proposals

THORChain showed that **liquidity can be a protocol function**: always-on pools, fees commensurate with demand for that liquidity, and a split of that revenue between the people who provide capital and the people who run nodes [22][23]. The Channel takes that settlement idea and changes the market-maker: **Uniswap v3 CLMM**, concentrated and lockable, not a full-range constant-product curve.

### 6.1 Systemic pools

Protocol-owned (or protocol-locked) CLMM positions are **systemic**. They are not a team wallet that might `subliquid`. They are on-chain positions with locks, harvested by the protocol. Swap volume against those pools produces `feesA` / `feesB`. Combined with flex-token transfer tax on the pure liquid twins, that flow is the primary operating income of the chain.

**Half** of it is **backpay** (`ra.org`, `ra.bpay`, approved proposals). The rest is holder yield and vault deepening. Income is not burned for spectacle and not captured by a foundation account as residual equity.

### 6.2 `ra.org` as the paymaster

`ra.org` already pays *roles*: base salary, optional bucket, self / manager / peer review (0–10), then a trickle of incoming bucks proportional to each person’s `owed`.

Weight on close (basis points): **1% guaranteed** + **33% × self/10** + **33% × manager average/10** + **33% × peer average/10**. Missing self or manager skips the seat; missing peers skips if any eligible peer exists.

Incoming transfers trickle out: each creditor receives `pool * owed_i / total_owed`.

**Block producers are creditors too.** `ra.bpay` already holds producer rewards. The design is that **half of CLMM and flex-fee harvests** fund `ra.bpay` (producers) and `ra.org` (everyone else owed: roles, approved proposals, regional work). Inflation may still exist as a backstop; fee income is the intended steady state, in the same spirit as THORChain preferring swap fees plus a reserve over printing as the only story [23].

### 6.3 Decentralized proposals

Who is owed, for what, is not only an admin granting seats. Anyone can open a **proposal**: pay this account (or this role, or this producer set) this much, for this work, with this proof.

Settlement is `ra.org` `owed` (or an equivalent proposal table that `closeperiod` / trickle already understand). Fees from systemic CLMM pools are the inflow. Trickle is the outflow. No separate “treasury multisig spends when it feels like it” as the happy path.

### 6.4 Human weight (not quadratic theater)

Quadratic funding and quadratic voting try to stop whales: matching or vote-cost grows with the *square root* of people, not the sum of dollars [24]. That only works if each “person” is a person.

A system that gives every key equal weight because a key can sign is giving equal weight to **anyone who can spawn accounts**—not to anyone who can breathe. Sybil farms pass that test. Humans who share one household and one passport do not get extra keys from breathing; bots do.

The Channel’s proposal weight is a **verified-human score**, then extras that are hard to fake in bulk:

| Component | Role | Whale / sybil note |
|-----------|------|--------------------|
| **Base** | One share per socially verified identity | No base for unverified keys |
| **RA held** | Skin in the unit the chain meters | **Concave / capped** — large stacks add only a little |
| **CPU burned** | Real activity on this chain | Renting CPU has a cost; idle whales do not look busy |
| **Social graph** | Passport connections, Recurve verification, offer completions | Three already-verified humans on video for Level 4 [25]; connections fade without continued interaction |

Formally, a sketch (coefficients governance-set, functions concave):

\[
W_i = B \cdot \mathbf{1}_{\mathrm{verified}} + c_{\mathrm{RA}}\,u(\mathrm{RA}_i) + c_{\mathrm{CPU}}\,u(\mathrm{CPU}_i) + c_{G}\,u(G_i)
\]

where \(u\) is increasing and concave (or hard-capped), \(G_i\) is a graph score (not raw friend count—weighted, decaying connections to other verified nodes), and \(B\) is the individual floor. **Minimum additional weight to large token holdings** means \(c_{\mathrm{RA}}\,u(\mathrm{RA})\) saturates quickly. A billion RA does not buy a million votes.

Proposals that many verified, active, connected people support outrank a whale clicking once. That is quadratic funding’s *spirit*. The representation is **Tetrans who showed up**—passports, CPU, graph—not a headcount of keypairs.

---

## 7. Identity and Signing

A Channel account is an Antelope account: **owner** and **active**, hierarchical permissions, 12-character names.

`channeld` accepts **K1**, **R1**, **WebAuthn**, **EM** (EIP-191), and **ED** (ed25519). `EM_ED_KEYS` must be activated for `recover_key` to accept the last two. `ra.authex` **records** an external address; it does not grant Channel signing. EM/ED keys on a permission do.

`ra.system::newapp` registers an app. `loginwithapp(user, app, parent, key)` creates a permission named after the app. Compromising a game key does not drain `owner`.

`ra.claim` v1: relayer `credit`, user `claim`. Stronger bindings are in `contracts/ra.claim/FUTURE.md`.

---

## 8. Passport

Tetra’s first product [25]. After KYC and redeeming an invite, a Tetran mints a **mutable Passport**—profile, offers, stamps, accolades. Connections are displayed; they are not stuffed into the NFT.

**Profile.** Region (required), optional city, how you want to connect locally and worldwide, a picture, optional links.

**Offers.** A description of an action, acceptance criteria, and a price: free, cost RA, or *pay* RA to complete. Regional, national, or global. Completer does the action; offerer verifies; RA moves. If the offerer cannot pay, it comes out of their daily share until settled. Views, clicks, and conversions feed Discovery.

**Connections.** Formed by completing offers, electing leaders, and upvoting. Tetran-to-Tetran and Tetran-to-region. Strength **fades per day** and grows with continued interaction. The graph is the \(G_i\) in §6.4.

**Stamps.** Complete offers in other regions.

**Accolades.** Top a Discovery chart, become a Chief, and similar—on-chain, used as gates.

**Rules.** One passport per human. No illegal offers. If it is illegal in a region, it is not a global offer.

Passport is how The Channel knows a user is not “a key that can breathe.” It is the identity object proposal weight and Recurve both sit on.

---

## 9. Discovery

Daily competition to rank **Passports**, **offers**, and **content** on regional, national, and global charts [25]. Charts reset daily; history is kept for any day, week, month, or season.

**Ups** cost 1 RA. After a given level, Tetrans earn a **national RA** (USARA, COLRA, …) that only spends on that nation’s charts. Local wins bubble up.

Default rank: Ups plus offer volume in RA. Because those weights match, you can climb by getting people to *take the offer*, not only by buying Ups. Chiefs (via Localize) may retune the regional algorithm: more weight on local RA, local offers, or local connections.

Seasonal cumulative rank becomes a Passport accolade: a lasting “the people loved this” stamp. That season is the Carnival calendar (§12).

Discovery is **mining attention** in public: limited daily RA, regional blinders first, then national and global heat.

---

## 10. Recurve

Fractal governance to elect regional (and later national) leaders [25]. Process is based on Fractally: small groups, face-to-face (Zoom), consensus recorded on-chain. Tetra’s founder was the 100th member of the original Fractally.

**Social verification (Level 4).** Three already-verified people meet the prospect on video. The prospect takes an oath; proof is uploaded on-chain. **This is the sybil gate** for elections and for the base term \(B\) in proposal weight. Unverified accounts do not get “one human” weight.

**Representatives (Reps).** Elected at fortnightly meetings in breakouts of 4–6. Each person presents contributions from the last two weeks; the group picks who represented them best and signs on-chain. Term: two months, or longer if they hold a role and attend at least monthly. Reps may propose House Rules and Procedures.

**Chiefs.** Only Reps run. Campaign silence until six months before the election. One new Chief every six months, two-year term, four Chiefs per region, staggered. After a term, skip the next election, eligible after that. Ratify or veto House Rules and Procedures. Split 50% of regional Baron/Steward revenue.

**Meetings.** ~1.5 hours (longer if a second round). Landing, presentations, consensus, regroup. Winners of small rooms can be folded up until a regional winner is chosen.

**Projects.** Reps (higher levels) direct RA to projects with seasonal deliverables, rated 0–100. Activated per region after Recurve has enough activity there.

Global Recurve waits on a large registered base; local Recurve is activated per region. Until then, Passport and Discovery still run.

---

## 11. Localize

How an activated region becomes a distinct place [25]. Recurve elects; Localize is the powers and the economy those leaders run.

**Activation.** Four founding members, stake TETRA above the average regional stake, same four nominated as founding chiefs, KYC. The region must beat the current lowest active region on population and activity. Founding chiefs serve until elections; the least-staked founding chief transitions first.

**House Rules.** Reps write, Chiefs ratify (all four to approve; a 3–1 can pass next session after a new Chief). Can change Discovery weights, passport requirements, Recurve meeting shape, allowed offer types—within global parameters.

**Procedures.** How events run, channels, local RA extras, community standards.

**Roles.** Chiefs approve (3/4) for Reps: Content Creator, Educator, Recruiter, Connector, Event Planner, Master of Coin, Developer, and region-defined roles. These seats are natural `ra.org` roles: salary, review, trickle from **CLMM fee backpay** as well as local RA.

**Economy.** Daily RA capacity tracks TETRA staked to the locale. Claim cap sketch: `staked TETRA + 12`. Each TETRA staked adds to daily distribution. Local RA variants carry 2–4× influence on local charts.

**Landing.** **HuRA** gatherings (full moon, and otherwise) organized by Chiefs and Reps. Tetra-enabled venues: RA redemption, meetups, offer fulfillment, mapped in-app.

Localize is where “the chain pays humans” meets a map: chiefs, reps, and roles are the owed set `ra.org` already knows how to trickle.

---

## 12. Carnivals

A **Carnival** is the three-month season Tetra already scores [25].

**Rhythm.**

- **Every day:** claim RA, climb Discovery, take or give offers.
- **Every full moon:** HuRA—host, co-create, or join, once the region is activated.
- **Every three months:** Carnival season. Competitions run the whole season. **Three Zoom meetings** across the season. A **three-day voting period** at the end.

Carnival voting is the same *kind* of weight as §6.4: verified humans, graph, activity, modest RA—not a token snapshot and not a bot headcount. Season accolades (Discovery cumulative rank, project ratings, Recurve service) land on the Passport.

Carnivals are not a conference series bolted onto a chain. They are the calendar that turns daily RA and CLMM fee income into a public, regional, then national story: who showed up, who was loved, who is owed.

---

## 13. Abundance, Time, and the Next Game of Earth

Two games can be named.

**The incumbent game** is interest-based debt. Money is issued as a loan. The principal is created; the interest is not. In a closed system \(M(t) < D(t) + I(t)\): the money needed to pay all obligations cannot exist at once [29][30]. Interest compounds. Global debt has already been measured in the hundreds of trillions against a money stock that cannot cover it [32]. The result is a race: someone must default, absorb inflation, or be bailed out. That is not a moral failing of debtors. It is arithmetic. It is **financial energetics out of balance**—a sink that does not match the source.

**The Channel’s game** is the next game of Earth: a settlement layer whose native flows are **time-based and abundant**, in the sense Web 4 named in 2020 [31]. The Sun does not invoice the planet for last year’s light. Life is a continual gift. Time tokens, daily RA, Carnival seasons, decaying connection graphs, and flex income from *volume that already happened* are the same pattern: a flow you can claim in an interval, not a principal that must be repaid with money that was never minted.

Abundant systems are not “infinite print.” They are **foundations that match how living systems actually work**: one human, one passport; one interval, one claim; liquidity that stays in the vault so redemption is a trade, not a prayer; fees that pay the people who showed up. Those are the foundations a society needs if it is to be in **resonance with systems of abundance** instead of in resonance with a ledger that can never clear.

The contrast is the point. Interest-on-interest cannot be the last word on Earth. The Channel is built so another game can unfold in public, on-chain, with backing you can read.

---

## 14. Clearing Compounding Debt: a Global Economic Reset

Simple solutions to runaway compounding already exist: debt jubilees (as old as Mesopotamia), principal/interest separation, write-downs that preserve credit *relationships* while dropping the impossible remainder [32]. The 2020–2024 Global Debt Reset writing proposed dual assets so principal history and time-value can be recognized without requiring the old interest to exist as money [32].

**Governments will not implement those solutions at the scale required**, not because the math is unknown, but because of **inertia**: incumbents, contracts, ratings, and political risk all sit on the current game. Waiting for a global treaty to rebalance financial energetics is not a plan.

The Channel does not replace a central bank by decree. It **kickstarts a reset by necessity**:

1. **A parallel unit of account and a parallel liquidity.** Pure liquid twins of bridged assets, locked CLMM, redemption into the underlying. People can move value into a circuit that *can* clear.
2. **Income that is a share of flow, not a new loan.** Flex reflections and half of systemic fees to backpay. Holders and operators are paid from volume, not from issuing more principal-plus-interest.
3. **Time-based claims.** Daily RA, seasons, verification—Web 4 time tokens in a living economy [31]—so influence and livelihood are not only “who compounded first.”
4. **Democratic listing.** Which foreign assets get a pure liquid twin is decided by **liquidity commitment on native chains**, so the reset is opted into by people putting inventory at risk, not by a ministry.

When enough volume and attention live on a chain whose vaults can redeem and whose fees pay humans, the old circuit is no longer the only game. That is a **global economic reset** in the only way one actually starts: a better-balanced energetic path that people can take without waiting for the old system to vote itself out of existence.

---

## 15. Conclusion

The Channel is a chain people can log into, that prices storage honestly, that finalizes under Savanna, and that issues **pure liquid twins of bridged assets** whose backing is a **locked Uniswap v3 CLMM vault**, with flex-token income in any listed token and **half the fees** as backpay.

Systemic CLMM pools take THORChain’s lesson—liquidity as protocol infrastructure that pays operators [22][23]—and pay **verified Tetrans**. Assets are listed by liquidity committed on native chains. Quadratic funding’s instinct (breadth over whales) stays. The lie that every key is a person does not.

The interest-debt game cannot be repaid in full; it was never designed to [29][30][32]. The Channel is the next game of Earth: time, abundance, attention, and liquidity that stays. Tetra’s four products—Passport, Discovery, Recurve, Localize—and the Carnival season are why attention flows in this channel. The tetrahedron is Tetra Grids’ encoding of that base; the other solids are higher versions of it.

---

## Acknowledgements

The Channel stands on Spring / Antelope, the Antelope reference contracts (renamed `ra.*`), Tonomy’s login and resource patterns, XPR Network’s RAM market, Uniswap v3, THORChain’s treatment of continuous liquidity and node/LP fee splits, Fractally, Web 4 (2020), and the Pure Liquid experiment that led Antelope community volume and XPR locked stablecoin liquidity. Tetra product definitions live at [know.tetra.earth](https://know.tetra.earth).

---

## References

[1] S. Nakamoto, “Bitcoin: A Peer-to-Peer Electronic Cash System,” 2008.

[2] V. Buterin, “Ethereum: A Next-Generation Smart Contract and Decentralized Application Platform,” 2014.

[3] C. Li and B. Palanisamy, DPoS decentralization literature (Steemit / BitShares / EOS).

[4] [5] Ethereum builder/relay censorship and OFAC-related exclusion (2022–).

[7] Bitcoin as settlement security versus more expressive chains.

[8] D. Larimer, Delegated Proof-of-Stake.

[9] [10] [11] DPoS turnout, wealth concentration, and collusion.

[12] [13] Bridge and oracle trust assumptions.

[14] Wormhole bridge exploit, February 2022.

[15] Ronin (Axie Infinity) bridge exploit, March 2022.

[16] Harmony Horizon bridge exploit, June 2022.

[17] [18] Atomic cross-chain swaps: design versus UX.

[19] Wallet, key, and gas UX as adoption barriers.

[22] THORChain, “Continuous Liquidity Pools,” https://docs.thorchain.org/technical-documentation/thorchain-finance/continuous-liquidity-pools — always-on pools, slip-based fees, fee revenue for the protocol.

[23] THORChain economic model and incentive pendulum (node operators vs liquidity providers).

[24] Gitcoin / Buterin et al., quadratic funding and quadratic voting; sybil as the binding constraint.

[25] Tetra Knowbase, https://know.tetra.earth — Passport, Discovery, Recurve, Localize, Life as a Tetran (carnivals, HuRA).

[28] H. Adams et al., Uniswap v3 Core (concentrated liquidity, ticks, positions).

[29] M. Binswanger, “Is there a growth imperative in capitalist economies? A circular flow perspective,” *Journal of Post Keynesian Economics*, 31(4), 2009 — \(M(t) < D(t) + I(t)\).

[30] S. Keen, work on debt-based monetary circuits and the impossibility of simultaneous full repayment of principal plus interest (e.g. 2009, 2015). See also McLeay, Radia & Thomas, Bank of England, “Money creation in the modern economy,” 2014.

[31] D. J. Butner, “The Web 4 Manifesto: Provable Democracy,” 2020, https://github.com/dougbutner/web-4 — time tokens, proof of individuality, information entropy, geosocial systems; biomimicry and the Sun’s abundance.

[32] D. J. Butner, Global Debt Reset, https://github.com/dougbutner/web-4/tree/master/Global-Debt-Reset — interest paradox, dual-asset (principal / time) sketches, why a clearing is necessary; BIS global-debt figures cited therein.

---

*© Tetra Grids, LLC. The Channel protocol and this paper are intended for open implementation. RA, Pure Liquid tokens, and CLMM locks carry market risk; locked liquidity is only as strong as the lock table and the chain that enforces it.*

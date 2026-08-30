# Pure Liquid

**Companion to [The Channel](./WHITEPAPER.md)**

Douglas Butner, AKA Gudasol (🜛)  
Tetra Grids, LLC (⟁)  
Maryland, United States

v1.0.0

[The Channel](./WHITEPAPER.md) is the protocol. This paper is the money: how a token can be turned back into what it claimed to be, and how holding it can pay you for *use*.

> If the reserve can walk away, it is not a reserve.

---

## Why this exists

Most “backed” tokens fail a child’s test. Can I sell this for the thing on the label? Does anyone get paid because the token *moved*, or only because a story was told?

We ran the answer in public. A community project on XPR became the number one volume community project on Antelope, and the number one locked stablecoin liquidity on that network. **The Channel** makes that pattern native.

I will define every term before I lean on it.

---

## 1. The problem with “backed”

What people call backing is usually:

- A mint with a hidden key.
- A pool the team can empty.
- A wrap: a claim on another chain, and no inventory here to sell into.

That is not backing. That is a screenshot.

---

## 2. Ranged liquidity

**Ranged liquidity** (concentrated liquidity in the Uniswap v3 sense) means a pool that is only a market *inside a price band*. Below the floor or above the ceiling, the position sits as one asset and earns no swap fees until price comes home.

A pool that pretends to be a market at every price from zero to infinity spreads capital thin. A launch priced like a penny can mark a nation-sized value on units nobody paid that price for. A **range** is a gift on the first buy, and inventory left for a long curve.

This is not a vendor. It is an idea: a current price, a band, fees while trades cross the band.

---

## 3. Lock

**Locked** means the inventory in that band cannot be removed until a stated time. The lock may be extended. It may not be shortened by privilege. Traders may still trade against it. Fees may still be taken. The core cannot be rugged.

That is the reserve. You can read the pool, the band, and the end of the lock.

---

## 4. Pure Liquid

A token is **Pure Liquid** when:

1. **Full supply in the vault.** At the start, the entire maximum supply is the token side of one or more ranged positions.
2. **Those positions are locked.**
3. **Circulation is purchase.** Units leave when someone swaps the other asset in. What holders receive is a **fee on activity**, not new units against empty supply.
4. **Redemption is the same pools.** You recover the underlying by selling back. The counter-asset still in the curve is the backing.

A farm with a burn and no vault is not Pure Liquid. Do not blur that line.

Backing is not a bank, not a printer, not a fraction, not a letter from a custodian. It is locked inventory plus what the market has left in the range.

---

## 5. Activity-based interest (reflections)

**Activity-based interest** is a share of fees from transfers and from swaps, paid to holders in proportion to what they hold (above dust). It is not a rate a borrower promised. It is a cut of *use*.

On each transfer, a configured slice lands in a **reflection pool** (and optionally a project pool or a burn). Anyone may splash the pool. Holders get their share in-wallet.

**Reflect** means the share stays in the same token.

**Choose another token** means that share is sold through the same ranged markets into an asset the holder picked. That is the flexible reward: you are not sentenced to earn only the ticker you already hold. The mechanism is the one just defined: activity-based interest, routed.

Payouts are smoothed so one click does not empty the pool. Inventory still sitting in the vault is not treated as a “holder,” so the lock does not drink the yield.

**Inheritance** means you can point a fraction of that stream at another account, with a memo, without giving away the stack. Time, again. A grandchild can receive a flow.

Half of these fees, together with half of the fees from **systemic pools** (protocol-owned ranged vaults that stay on), pay the people **The Channel** owes: producers and approved work. The other half is holder income and a deeper vault. Volume is the engine. Bots pay the tax. Holders who stay keep the better side.

This is interest in the only honest sense. It is not interest on a loan that created the money you must repay.

---

## 6. Two versions of every bridged asset

When an asset is brought onto **The Channel**, it is not one ticker with a story.

**Version A: Bridged.** The coins remain under the native chain’s consensus. Here you hold a claim. You unlock by the rules of that claim.

**Version B: Pure Liquid.** A twin is placed into locked ranged liquidity against that underlying (or against inventory the community actually posted). Redeem by selling into the vault. Earn activity-based interest in the twin or in any other listed token.

You may hold A, or B, or both. They are not the same object. A maps custody. B is a market that cannot quietly un-back itself until the lock ends.

**Listing B is democratic.** The twin appears when people **commit liquidity on the native chain**. Real inventory there is the vote. Empty accounts do not list a market.

---

## 7. Systemic pools

**Systemic pools** are ranged vaults the protocol owns and locks. They stay on, in the spirit of [continuous liquidity](https://docs.thorchain.org/technical-documentation/thorchain-finance/continuous-liquidity-pools): people need to trade, fees exist, those fees belong to the network.

They sit beside the Pure Liquid twins. Together they are the guarantee. If you hold B, there is a curve to sell into. If the protocol is in the pair, that curve is on the chain’s own books.

---

## 8. What holders get

- **Liquidity for redemption** of the underlying, as strong as the lock and the depth of the range.
- A **stream** in any listed token on The Channel.
- Operators paid from the same volume (the half that is backpay). The game is not “print to pay the machines.”

[The Channel](./WHITEPAPER.md) is consensus, RA, identity, recovery, Tetra. This paper is the vault and the stream.

---

## References

- [The Channel](./WHITEPAPER.md)
- [Uniswap v3 Core](https://uniswap.org/whitepaper-v3.pdf)
- [THORChain: Continuous Liquidity Pools](https://docs.thorchain.org/technical-documentation/thorchain-finance/continuous-liquidity-pools)
- [Web 4 Manifesto](https://github.com/dougbutner/web-4) (2020)

---

*© Tetra Grids, LLC. Pure Liquid carries market risk. Ranges can still lose to price. What they cannot do, while locked, is vanish because someone had a key.*

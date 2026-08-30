# The Channel

**A Human-First Blockchain for Peer-to-Peer Trust, Attention, and Pure Liquid Finance**

Douglas Butner, AKA Gudasol (🜛)  
Tetra Grids, LLC (⟁)  
Maryland, United States

v1.0.0

This paper is living. As The Channel is built, this text will move with it. A companion, [Pure Liquid](./PURE-LIQUID.md), is the financial layer in full.

> The truth is, all systems have some level of trust, even if that is just trust in the system itself. If we try to get rid of trust because we are fearful, that is lame. We grow trust on purpose.

---

## Abstract

Bitcoin named the original goal: **return power to the people**. It tried to do that by *removing* trust. **The Channel** keeps the goal and reverses the method. We create **networks of trust** through repeated attention to human effort and experience over time.

Bitcoin channels money to machines, to those who build the machines, to those who extract the rare earth minerals those machines require, and to those who generate the power. That power is burned as fossil fuel. It is also sourced from “renewables,” often requiring rare earth minerals. Earth is then organized around leaky mines, chip factories, loud data centers, and power plants. That organization is unnecessary and unnatural.

The Channel organizes people. It is **time-based**: rewarding attention for the value we're offering the world in democratic competetions and daily income gleamed from system-wide opt-out taxes on movemment of assets every interval of life, not to whoever can waste the most energy this block. On that ground we add an evolutionary financial layer. We call a currency **Pure Liquid** when its whole supply is locked as ranged liquidity against a real or more trusted asset (so it can be redeemed instantly without any third party constraint) and **Flex**, when it pays **activity-based interest** (a share of real volume) to accounts, in the same token or in another token on the network, because the reward asset is flexible to the desire of the holder.

People come together and share ideas. That's the valuable societal change.

Assets come together to provide sane economies. That's the basis of economies based on surplus, not debt.

---

## Technical overview

**The Channel** is a human-first layer-1: names you can say, permissions you can nest, resources you can understand. Tetra Grids implements it in C++ from Spring 2.0 (Antelope) with Savanna consensus. You can run the node. Others can run it too.

We use **RA** for the system token. RA meters storage and compute. RA is redeemable for provable attention through human-verified interaction with content through the discovery network. RA is not Pure Liquid; it is liquid for attention in the discovery network.

**Ranged liquidity** means a pool that is a market only inside a chosen price band (the Uniswap v3 idea: capital is not smeared from zero to infinity). **Locked** means that inventory cannot be pulled until a stated time. **Pure Liquid** means the entire supply starts in those locked bands; circulating units were bought out; income is a fee on activity, not a print. We already ran this in public. It became the number one volume community project on Antelope, and the number one locked stablecoin liquidity on XPR.

Every asset that arrives from another chain has **two versions**:

1. **Bridged:** a claim on coins that stayed under the native chain’s consensus.
2. **Pure Liquid:** a vaulted twin. Locked ranged liquidity against that underlying. Redeem by selling into the vault. Activity-based interest in that twin or in any other listed token.

**Systemic pools** are protocol-owned ranged vaults that stay on. Half of their fees, and half of Pure Liquid activity fees, pay producers and everyone the network owes. The rest stays with holders. Which assets get a Pure Liquid twin is decided by **liquidity committed on the native chains**, not by a committee.

Who is owed is proposed in the open. Weight starts with a verified person, then activity on this chain, then a living social graph. A large RA stack adds only a little.

Read [Pure Liquid](./PURE-LIQUID.md) for the vault and the stream.

---

## 1. The problem

Democracy and money were supposed to put the many over the few. Both now run slow, inaccurate, and captured. Blockchain promised a way out. For that promise to hold, a chain must be fast and cheap, and then add what a bank cannot: custody you can check, rules without a clerk, tokens whose backing you can read.

Bitcoin proved digital scarcity. It also proved that if you pay machines to ignore people, Earth rearranges around extraction. Ethereum proved general contracts. It also proved that hex, surprise fees, and wrapped assets with pullable liquidity will not, by themselves, return power to anyone.

In this paper I will show The Channel as a protocol: time-based claims, a chain people can log into, Pure Liquid as the money layer, identity that can recover without giving up keys, and (only at the end) the human application that makes attention worth settling.

---

## 2. Time

**Time-based** means a claim is stamped to a person and an interval. You cannot mint two days of influence in one second by spinning keys. Daily RA, seasons, and ties that fade without continued attention are that pattern. It is the same instinct as [Web 4](https://github.com/dougbutner/web-4) (2020): biomimetic, in resonance with the Sun’s continual gift of life, not a principal that must be repaid with money that was never issued.

Abundant systems are not infinite print. They are foundations that match living systems. The Channel is built so the next game of Earth can unfold in public.

---

## 3. The chain, in brief

**Consensus.** A scheduled producer writes a block every half second, twelve blocks to a turn. **Finalizers** are a second duty: they vote with aggregated signatures so a block can become irreversible without a long maybe. The set that proposes is not automatically the set that finalizes.

**Accounts.** Human-readable names. Nested permissions. A device-bound credential, or a key from another ecosystem, can sit on an account. An application gets its own permission. It never needs the keys that own you.

**Resources.** Storage is sold at a fixed price in RA. Applications are expected to pay for the rows they create for users. Users pay for compute, or rent it. Producers produce blocks.

**Crossing chains.** You can attest an address elsewhere. A deposit on another chain can be credited here and claimed. Until proofs replace them, the people who credit deposits are trusted operators. They can be rotated.

That is enough chain. Vaults and reflections are [Pure Liquid](./PURE-LIQUID.md).

---

## 4. Pure Liquid, for the chain

**Ranged liquidity:** only a market between a floor and a ceiling. A dump across all prices can mark a huge value on inventory nobody paid for. A range is a gift on the first buy and a long curve after.

**Locked:** the band cannot be emptied until time says so. The lock can grow. It cannot be cut short by privilege. Fees can still be taken. The vault is the reserve.

**Pure Liquid:** all supply in those locked bands; circulation is purchase; redemption is selling back. Backing is the counter-asset still in the curve.

**Activity-based interest:** holders receive a share of transfer and swap fees. Same token, or another listed token. Not inflation against empty air.

**Two versions of a bridged asset.** Bridged is the claim. Pure Liquid is the redeemable twin plus the stream. Listing the twin is a vote made of inventory on the native chain.

**Systemic pools:** the same idea at protocol scale. Always on. Half the fees pay the people who run and tend the network. The rest pays holders and deepens vaults. Proposals are public. Weight is a verified human, then compute actually used on this chain, then connections. Whales do not buy the room.

---

## 5. Identity and social recovery

An account is a name. Permissions are nested. A leaked game key does not drain the rest.

**Social recovery:** you still hold your keys. You may set limits so a large transfer, or a transfer that does not look like you, needs people **upstream** of you (those who invited and vouched, in a graph that fades without attention) to confirm. Better than a seed in a drawer. Better than a chain that cannot help when the key is gone. The object that stores this graph is defined in the next section.

---

## 6. Tetra

Tetra is why anyone would live here. Tetrans spend daily RA on **offers**: an action, a cost or a payment in RA (or none), a way to know it was done. Four products, then a season.

**Passport.** Who you are, where you are, what you offer, stamps, accolades. One per human. Connections form when you complete offers, elect, and upvote. They fade if you vanish. This is the upstream for social recovery. This is the base of honest weight.

**Discovery.** Daily charts of people, offers, and work. Regional first, then national, then global. Attention is spent as RA.

**Recurve.** Fractal elections in small rooms, face to face. Already-verified people meet a new person. An oath goes on the record. That is how a key becomes a human.

**Localize.** A region turns on when people stake and found it. Elected leaders set house rules and roles inside global bounds. Gatherings. Places on a map.

**Carnivals.** Three months of competition, meetings, and a short vote at the end. Accolades land on the Passport.

Tetra is the channel in which attention flows. The tetrahedron is the base encoding. The other solids are higher versions of it.

---

## Conclusion

**The Channel** is a chain you can log into, that prices storage honestly, that finalizes under Savanna, and that treats **trust as something people grow**. Bitcoin paid machines. We attend to humans. Pure Liquid is how value stays redeemable and how activity becomes interest in the token you choose.

The companion is [Pure Liquid](./PURE-LIQUID.md).

---

## Acknowledgements

Dan Larimer, who authored delegated proof of stake and the fractal, nested design of the original EOSIO system.

All who contributed to the code this work was pulled upon: Spring and Antelope, the reference system contracts, Tonomy’s human login and resource patterns, XPR Network’s RAM market, Uniswap v3 as the idea of ranged liquidity, and the Pure Liquid experiment that led Antelope community volume and XPR locked stablecoin liquidity.

King Solomon. Alexander the Great. Alexander Hamilton. The founding fathers.

Tetra: [know.tetra.earth](https://know.tetra.earth).

---

## References

1. S. Nakamoto, [Bitcoin: A Peer-to-Peer Electronic Cash System](https://bitcoin.org/bitcoin.pdf) (2008).

2. V. Buterin, [Ethereum: A Next-Generation Smart Contract and Decentralized Application Platform](https://ethereum.org/en/whitepaper/) (2014).

8. D. Larimer, [Delegated Proof-of-Stake and the EOSIO technical whitepaper](https://github.com/EOSIO/Documentation/blob/master/TechnicalWhitePaper.md).

22. THORChain, [Continuous Liquidity Pools](https://docs.thorchain.org/technical-documentation/thorchain-finance/continuous-liquidity-pools).

25. Tetra, [Knowbase](https://know.tetra.earth).

28. H. Adams et al., [Uniswap v3 Core](https://uniswap.org/whitepaper-v3.pdf).

31. D. J. Butner, [The Web 4 Manifesto: Provable Democracy](https://github.com/dougbutner/web-4) (2020).

32. D. J. Butner, [Global Debt Reset](https://github.com/dougbutner/web-4/tree/master/Global-Debt-Reset).

---

*© Tetra Grids, LLC. The Channel is intended for open implementation. RA, Pure Liquid, and locked liquidity carry market risk. A lock is only as strong as the chain that enforces it.*

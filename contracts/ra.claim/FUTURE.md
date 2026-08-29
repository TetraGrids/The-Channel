# Cross-chain claim (v1 and the road ahead)

`ra.claim` is a **deposit-there / claim-here** unlock. It is **not** Wire OPP: no outposts, batch operators, `msgch`, underwriters, or light-client proofs.

## v1 — trusted relayer

1. Governance (`ra.claim`) registers one or more relayer accounts with `addrelayer`.
2. A relayer, after seeing a confirmed deposit on an external chain, calls `credit(chain, ext_txid, recipient, quantity)`.
3. The recipient calls `claim(id)` and receives RA from this contract’s treasury.

`ra.authex` is a separate identity registry: a Channel account attests that it controls an address on another chain. v1 **does not** require an authex link to claim. The treasury must be pre-funded with RA (issue/transfer into `ra.claim`).

This matches the first version we can ship without new host functions or a proving network.

## Later — bring it up to speed

These steps can land without replacing v1 tables. Relayer `credit` stays as a fallback.

1. **Activate `EM_ED_KEYS`** so stock `recover_key` accepts EM (EIP-191) and ED (ed25519) signatures. That is already in `channeld`; it is a boot-time feature, not a new intrinsic.
2. **Signature-verified `createlink`** on `ra.authex` (Wire-style): user signs `"<pubkey>|<account>|<chain>|<nonce>|createlink auth"` with MetaMask or Phantom. Optional later: Wire’s non-aborting `try_recover_key` and contract-readable `get_permission`.
3. **User-submitted claim** bound to a linked key: the same signature scheme over `(ext_txid, recipient, amount)` so a user can claim without a relayer once a deposit is recorded.
4. **Only then** consider a proving path (headers / receipts / state proofs). That is a new protocol, not a v1 patch.

Until (3)–(4) exist, treat relayers as trusted operators: rotate them with `rmrelayer` / `addrelayer`, and keep the treasury balance equal to unclaimed credits.

# Cross-chain claim (v1 and signed keys)

`ra.claim` is a **deposit-there / claim-here** unlock. It is **not** Wire OPP: no outposts, batch operators, `msgch`, underwriters, or light-client proofs.

## v1 — trusted relayer

1. Governance (`ra.claim`) registers one or more relayer accounts with `addrelayer`.
2. A relayer, after seeing a confirmed deposit on an external chain, calls `credit(chain, ext_txid, recipient, quantity)`.
3. The recipient calls `claim(id)` and receives RA from this contract’s treasury.

`ra.authex` is a separate identity registry. Unsigned `createlink` is a self-attested address string. v1 **does not** require an authex link to `claim`. The treasury must be pre-funded with RA (issue/transfer into `ra.claim`).

Relayer `credit` stays as the way deposits are recorded until a proving path exists.

## Signed links and claims (EM / ED)

Requires the `EM_ED_KEYS` protocol feature (already in `channeld` and `scripts/launch-devnet.py`). That raises supported key types to 5 so host `recover_key` accepts EM (EIP-191) and ED (ed25519). Stock CDT `eosio::signature` still has no EM/ED alternatives; the contracts pass **fc-packed signature bytes** into the `recover_key` intrinsic.

### `ra.authex::linksig`

The Channel account must authorize. The same user signs this exact UTF-8 string (no extra spaces):

```text
<pubkey>|<account>|<chain>|<nonce>|createlink auth
```

- `<pubkey>` is the Channel string (`PUB_EM_0x…` or `PUB_ED_…`)
- `<nonce>` starts at `0` per account and increments on each successful `linksig`

Digest is `sha256(message)`. Wallets must sign that **32-byte digest** the same way they sign a Channel transaction:

- **MetaMask / EM:** `personal_sign` of the 32 digest bytes (EIP-191 prefix `"\x19Ethereum Signed Message:\n32"`). Do not `personal_sign` the UTF-8 message string; that hash will not match `recover_key`.
- **Phantom / ED:** sign the ASCII hex of the digest (64 hex characters), matching host ED recovery.

`linksig` stores the recovered packed pubkey on the `links` row (`packedkey`). Unsigned `createlink` / `adminlink` leave `packedkey` empty, so those rows cannot satisfy `claimsig`.

### `ra.claim::claimsig`

A deposit must already be `credit`ed. Anyone may submit `claimsig(id, sig)`; the first authorizer pays CPU. The signer is the linked EM/ED key, not the Channel account. Message:

```text
<ext_txid>|<recipient>|<quantity>|<chain>|<id>|claim auth
```

- `<ext_txid>` is 64 lowercase hex characters (no `0x`)
- `<quantity>` is `asset::to_string()` (for example `7.0000 RA`)
- Same sha256 + `recover_key` rules as `linksig`

v1 `claim(id)` with `require_auth(recipient)` remains.

## Later

1. Optional: Wire’s non-aborting `try_recover_key` and contract-readable `get_permission`.
2. On-chain proving (headers / receipts / state proofs). That is a new protocol, not a v1 patch.

Until proving exists, treat relayers as trusted operators: rotate them with `rmrelayer` / `addrelayer`, and keep the treasury balance equal to unclaimed credits.

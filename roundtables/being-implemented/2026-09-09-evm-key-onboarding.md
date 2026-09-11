# EVM key onboarding (signature-verified authex + claims)

- **Date:** 2026-09-09
- **People:** Douglas, Michael, Henrik
- **Target:** Oct 24 testnet
- **Roadmap:** [`contracts/ra.claim/FUTURE.md`](../../contracts/ra.claim/FUTURE.md)

Enable users to sign with EVM keys (MetaMask / Phantom) for a full onboarding flow without relying only on trusted relayers: activate `EM_ED_KEYS`, signature-verified link in `ra.authex`, user-submitted claims in `ra.claim`, lazy account creation paid from the bridge, tests, keep v1 relayers.

## Repo check (do this before any prompt)

`FUTURE.md` and the current contracts already describe `linksig`, `claimsig`, packed EM/ED `recover_key`, and `scripts/launch-devnet.py` activating `EM_ED_KEYS`. Treat the prompts below as a **gap list** against the tree, not a blank rewrite. Implement only what is still missing (lazy account creation, remaining tests, boot-time activation on non-devnet, wallet digest rules).

## Workflow order

1. Activate `EM_ED_KEYS` (core runtime prerequisite)
2. Signature-verified createlink / `linksig` in `ra.authex`
3. User-submitted claims in `ra.claim`
4. Bridge / lazy account creation
5. Unit tests for signature verification
6. Relayer fallback (parallel with signed claims)
7. Testnet (Oct 24)

## Exact Cursor prompts

### 1. Activating EM_ED_KEYS (prerequisite)

```
@antelope-core-dev Enable the EM_ED_KEYS feature flag so the Channel runtime accepts EIP-191 (EVM)
and ed25519 signatures in recover_key and account authorities. This is already in channeld,
just needs boot-time activation before ra.authex and ra.claim changes deploy.
```

### 2. EVM key signature support in ra.authex

```
@antelope-dev-style Implement signature-verified createlink in ra.authex:
Allow users to sign a message with their EVM key (MetaMask/Phantom) proving they control an external address,
then link that key to their Channel account without requiring prior authorization.
The signature format should be: "<pubkey>|<account>|<chain>|<nonce>|createlink auth"
```

### 3. User-submitted claims in ra.claim

```
@antelope-dev-style Extend ra.claim to support user-submitted, signature-verified claims:
Once a user has an authex link, they should be able to claim a deposit by signing (ext_txid, recipient, amount)
with their linked EVM key instead of requiring a relayer. Keep the relayer model as fallback for v1.
```

### 4. Bridging account creation flow

```
@antelope-dev-style Implement lazy account creation for cross-chain onboarding:
When a user bridges funds from an EVM chain and wants to claim on Channel,
allow them to sign a claim action with their external key.
The system should create their account (using the bridged funds to pay RAM)
only when they submit that signed claim, avoiding the chicken-and-egg problem.
```

### 5. Testing signature verification

```
@antelope-dev-style Add unit tests for signature verification in ra.authex and ra.claim:
Test EIP-191 signature validation, nonce uniqueness, chain/account binding, and signature replay protection.
Ensure tests cover both MetaMask and Phantom wallet signing formats.
```

### 6. Relayer fallback and upgrade path

```
@antelope-dev-style Ensure backward compatibility:
Keep addrelayer, rmrelayer, and the trusted relayer credit/claim flow working in parallel
with signature-verified claims so v1 relayers can still operate during the upgrade.
```

## Combined prompt (optional)

If you want the meeting context in one session:

```
@antelope-dev-style Based on the Sep 9 meeting discussion with Michael and Henrik,
implement the roadmap from contracts/ra.claim/FUTURE.md step-by-step:
1. Activate EM_ED_KEYS in Channel runtime
2. Add signature-verified createlink to ra.authex supporting EIP-191
3. Implement user-submitted claims in ra.claim with signature verification
This enables users to sign with EVM keys (MetaMask/Phantom) for a full onboarding flow
without relying on trusted relayers. Target Oct 24 testnet launch.
```

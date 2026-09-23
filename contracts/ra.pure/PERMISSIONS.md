# flex.token pool permissions

Protocol ranged vaults are positions this contract owns on an Alcor v2 swap (`swap.alcor` on XPR; configurable here). The interface is `alcor-exchange/alcorswap_interface.hpp`, matched to the live `swap.alcor` ABI (actions `createpool`, `addliquid`, `lockpos`, `collect`, table `pools`).

`position.hpp` and `tick.hpp` are the upstream Alcor internals for reference. They are not compiled; the contract only sends the actions in the interface.

## Who may do what

| Action | Authority | Effect |
| --- | --- | --- |
| `setswap` | `flex.token` | DEX account. Default if unset: `swap.alcor`. |
| `setexempt` | `flex.token` | Market accounts that skip the reflection tax. The swap account is always exempt. |
| `openpool` | `flex.token` | `createpool` for a sorted pair. Does not move liquidity. |
| `seedpool` | `flex.token` | `deposit`, `addliquid`, `lockpos`. Records the Alcor pool id. |
| `collectpool` | `flex.token` | `collect` to this contract or `setconfig`'s project account. |
| `addpool` / `setconfig` / `forge` | `flex.token` | Reflection routes and rates. Unchanged. |
| `subliquid`, `transferpos` | nobody | Not exposed. A lock on Alcor also rejects them until `unlock_time`, and a lock cannot be shortened. |

`seedpool` spends tokens this contract already holds (transfer them in first). If the pool is inactive, it also pays `system.activeFee` via memo `activepool#<id>`. Pass the id from Alcor's `logpool`; inline `createpool` does not reveal it in the same action.

Ticks must be multiples of the pool spacing (10 / 60 / 200 for fees 500 / 3000 / 10000). `unlock_time` is seconds since epoch and must be in the future.

## Account permission

`flex.token@active` must include `flex.token@eosio.code`, or every inline transfer, `createpool`, `addliquid`, and `lockpos` is rejected:

```bash
chan set account permission flex.token active --add-code
```

The contract pays the RAM for the pool, position, ticks, bitmap, and lock rows.

## What changed versus the XPR-only hardcoding

- Swap account is `setswap`, not a fixed `swap.alcor` scattered through `transfer` / `reflect`.
- `alcor` and `gold.mon3y` are no longer implicit tax exemptions. Add them with `setexempt` if a deployment still needs that.
- LP-fee notifies (`Col…` memos from the swap account) pay `project_account`, not a hardcoded `reflections` name. If that account is this contract, the tokens stay here (`transfer` cannot target itself).
- `reflect` burns with `smelt` (the action that exists). The old inline name `burn` could not succeed.

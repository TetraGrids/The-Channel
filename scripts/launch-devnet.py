#!/usr/bin/env python3
"""Boot a local Channel single-producer chain with ra.* contracts and RA.

Requires built channeld/chan/keyd and compiled contract wasm (cmake
-DBUILD_CONTRACTS=ON, or artifacts from the Compile contracts Action).

  python3 scripts/launch-devnet.py --reset
"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import signal
import subprocess
import sys
import time
import urllib.error
import urllib.request
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]

# Matches -DCHANNEL_ROOT_KEY default in CMakeLists.txt / genesis/channel_genesis.json
DEFAULT_PUBLIC_KEY = "PUB_K1_6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5BoDq63"
DEFAULT_PRIVATE_KEY = "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"

PREACTIVATE_FEATURE = "64fe7df32e9b86be2b296b3f81dfd527f84e82b98e363bc97e40bc7a83733310"

# Spring 2.0 builtin digests (libraries/chain/protocol_feature_manager.cpp).
# Order satisfies feature dependencies. FORWARD_SETCODE is required for SAVANNA;
# ra.system implements setcode once it is on `ra`.
PROTOCOL_FEATURES = [
    ("ONLY_LINK_TO_EXISTING_PERMISSION", "f3c3d91c4603cde2397268bfed4e662465293aab10cd9416db0d442b8cec2949"),
    ("REPLACE_DEFERRED", "9908b3f8413c8474ab2a6be149d3f4f6d0421d37886033f27d4759c47a26d944"),
    ("NO_DUPLICATE_DEFERRED_ID", "45967387ee92da70171efd9fefd1ca8061b5efe6f124d269cd2468b47f1575a0"),
    ("FIX_LINKAUTH_RESTRICTION", "a98241c83511dc86c857221b9372b4aa7cea3aaebc567a48604e1d3db3557050"),
    ("DISALLOW_EMPTY_PRODUCER_SCHEDULE", "2853617cec3eabd41881eb48882e6fc5e81a0db917d375057864b3befbe29acd"),
    ("RESTRICT_ACTION_TO_SELF", "e71b6712188391994c78d8c722c1d42c477cf091e5601b5cf1befd05721a57f3"),
    ("ONLY_BILL_FIRST_AUTHORIZER", "2f1f13e291c79da5a2bbad259ed7c1f2d34f697ea460b14b565ac33b063b73e2"),
    ("FORWARD_SETCODE", "898082c59f921d0042e581f00a59d5ceb8be6f1d9c7a45b6f07c0e26eaee0222"),
    ("GET_SENDER", "1eab748b95a2e6f4d7cb42065bdee5566af8efddf01a55a0a8d831b823f8828a"),
    ("RAM_RESTRICTIONS", "1812fdb5096fd854a4958eb9d53b43219d114de0e858ce00255bd46569ad2c68"),
    ("WEBAUTHN_KEY", "927fdf78c51e77a899f2db938249fb1f8bb38f4e43d9c1f75b190492080cbc34"),
    ("WTMSIG_BLOCK_SIGNATURES", "ab76031cad7a457f4fd5f5fca97a3f03b8a635278e0416f77dcc91eb99a48e10"),
    ("ACTION_RETURN_VALUE", "69b064c5178e2738e144ed6caa9349a3995370d78db29e494b3126ebd9111966"),
    ("CONFIGURABLE_WASM_LIMITS2", "8139e99247b87f18ef7eae99f07f00ea3adf39ed53f4d2da3f44e6aa0bfd7c62"),
    ("BLOCKCHAIN_PARAMETERS", "70787548dcea1a2c52c913a37f74ce99e6caae79110d7ca7b859936a0075b314"),
    ("GET_CODE_HASH", "d2596697fed14a0840013647b99045022ae6a885089f35a7e78da7a43ad76ed4"),
    ("CRYPTO_PRIMITIVES", "68d6405cb8df3de95bd834ebb408196578500a9f818ff62ccc68f60b932f7d82"),
    ("GET_BLOCK_NUM", "e5d7992006e628a38c5e6c28dd55ff5e57ea682079bf41fef9b3cced0f46b491"),
    ("BLS_PRIMITIVES2", "c0cce5bcd8ea19a28d9e12eafda65ebe6d0e0177e280d4f20c7ad66dcd9e011b"),
    ("DISABLE_DEFERRED_TRXS_STAGE_1", "440c3efaaab212c387ce967c574dc813851cf8332d041beb418dfaf55facd5a9"),
    ("DISABLE_DEFERRED_TRXS_STAGE_2", "a857eeb932774c511a40efb30346ec01bfb7796916b54c3c69fe7e5fb70d5cba"),
    ("SAVANNA", "01a8bebb4922c38293a8909668f7d00ba4a21a455c69719ad7dcc33ea1e631c0"),
    ("EM_ED_KEYS", "dfc2e8e511691cb1e4c3e0792c48e2e882f74117e08b68b48f987903416bdd86"),
]

# SAVANNA depends on FORWARD_SETCODE. Skipping FORWARD_SETCODE also skips SAVANNA.
SAVANNA_AND_FORWARD = {"FORWARD_SETCODE", "SAVANNA"}

SYSTEM_ACCOUNTS = [
    "ra.bpay",
    "ra.msig",
    "ra.names",
    "ra.ram",
    "ra.ramfee",
    "ra.saving",
    "ra.stake",
    "ra.token",
    "ra.vpay",
    "ra.rex",
    "ra.fees",
    "ra.wrap",
    "ra.powup",
    "ra.reserv",
    "ra.resources",
    "ra.authex",
    "ra.claim",
    "ra.org",
    "flex.token",
]

CODE_ACCOUNTS = ("ra.msig", "ra.wrap", "ra.bpay", "ra.fees", "ra.resources", "ra.claim", "ra.org")

procs: list[subprocess.Popen] = []


def log(msg: str) -> None:
    print(f"launch-devnet: {msg}", flush=True)


def fail(msg: str) -> None:
    print(f"launch-devnet: error: {msg}", file=sys.stderr, flush=True)
    sys.exit(1)


def run(cmd: list[str], *, check: bool = True) -> subprocess.CompletedProcess:
    log(" ".join(cmd))
    result = subprocess.run(cmd, check=False, text=True)
    if check and result.returncode != 0:
        fail(f"command failed ({result.returncode}): {' '.join(cmd)}")
    return result


def first_existing(candidates: list[Path]) -> Path | None:
    for path in candidates:
        if path.is_file() and os.access(path, os.X_OK):
            return path
    return None


def find_binary(name: str, explicit: str | None, build_dir: Path) -> Path:
    if explicit:
        path = Path(explicit).expanduser().resolve()
        if not path.is_file():
            fail(f"{name} not found: {path}")
        return path
    found = shutil.which(name)
    if found:
        return Path(found)
    found_path = first_existing(
        [
            build_dir / "programs" / name / name,
            REPO_ROOT / "build" / "programs" / name / name,
            REPO_ROOT / "build-arm64" / "programs" / name / name,
        ]
    )
    if found_path:
        return found_path
    fail(f"could not find {name}; pass --{name} or put it on PATH")


def find_contract_dir(contracts_dir: Path, wasm_stem: str, extra_dirs: list[str] | None = None) -> Path | None:
    search = [contracts_dir / wasm_stem, contracts_dir]
    for extra in extra_dirs or []:
        search.insert(0, contracts_dir / extra)
    for directory in search:
        if (directory / f"{wasm_stem}.wasm").is_file():
            return directory
    return None


def wait_http(url: str, timeout: float) -> None:
    deadline = time.time() + timeout
    while time.time() < deadline:
        try:
            urllib.request.urlopen(url, timeout=2)
            return
        except (urllib.error.URLError, TimeoutError, OSError):
            time.sleep(0.2)
    fail(f"timed out waiting for {url}")


def http_post_json(url: str, body: dict) -> dict:
    data = json.dumps(body).encode()
    req = urllib.request.Request(url, data=data, headers={"Content-Type": "application/json"}, method="POST")
    with urllib.request.urlopen(req, timeout=30) as resp:
        raw = resp.read().decode()
        return json.loads(raw) if raw else {}


def chan_cmd(args: argparse.Namespace) -> list[str]:
    return [
        str(args.chan),
        "--url",
        f"http://127.0.0.1:{args.http_port}",
        "--wallet-url",
        f"http://127.0.0.1:{args.wallet_port}",
    ]


def set_contract(args: argparse.Namespace, account: str, contract_dir: Path, wasm_stem: str | None = None) -> None:
    stem = wasm_stem or contract_dir.name
    wasm = contract_dir / f"{stem}.wasm"
    if not wasm.is_file():
        found = list(contract_dir.glob("*.wasm"))
        if not found:
            fail(f"no wasm in {contract_dir}")
        wasm = found[0]
    abi_name = wasm.name.replace(".wasm", ".abi")
    run(
        chan_cmd(args)
        + ["set", "contract", account, str(contract_dir), wasm.name, abi_name, "-p", f"{account}@active"]
    )


def cleanup(*_sig) -> None:
    for proc in reversed(procs):
        if proc.poll() is None:
            proc.send_signal(signal.SIGTERM)
    for proc in procs:
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()


def start_keyd(args: argparse.Namespace) -> None:
    wallet_dir = args.data_dir / "wallet"
    if args.reset and wallet_dir.exists():
        shutil.rmtree(wallet_dir)
    wallet_dir.mkdir(parents=True, exist_ok=True)
    cmd = [
        str(args.keyd),
        "--unlock-timeout",
        "999999999",
        "--http-server-address",
        f"127.0.0.1:{args.wallet_port}",
        "--wallet-dir",
        str(wallet_dir),
    ]
    log(" ".join(cmd))
    procs.append(subprocess.Popen(cmd))
    wait_http(f"http://127.0.0.1:{args.wallet_port}/v1/wallet/list_wallets", args.wait)
    run(chan_cmd(args) + ["wallet", "create", "--to-console"])
    run(chan_cmd(args) + ["wallet", "import", "--private-key", args.private_key])


def start_channeld(args: argparse.Namespace) -> None:
    node_dir = args.data_dir / "node"
    if args.reset and node_dir.exists():
        shutil.rmtree(node_dir)
    node_dir.mkdir(parents=True, exist_ok=True)
    stderr = open(node_dir / "stderr.log", "w")
    cmd = [
        str(args.channeld),
        "--max-irreversible-block-age",
        "-1",
        "--max-transaction-time=200",
        "--contracts-console",
        "--genesis-json",
        str(args.genesis),
        "--blocks-dir",
        str(node_dir / "blocks"),
        "--config-dir",
        str(node_dir),
        "--data-dir",
        str(node_dir),
        "--chain-state-db-size-mb",
        "1024",
        "--http-server-address",
        f"127.0.0.1:{args.http_port}",
        "--p2p-listen-endpoint",
        f"127.0.0.1:{args.p2p_port}",
        "--enable-stale-production",
        "--producer-name",
        "ra",
        "--signature-provider",
        f"{args.public_key}=KEY:{args.private_key}",
        "--plugin",
        "eosio::http_plugin",
        "--plugin",
        "eosio::chain_api_plugin",
        "--plugin",
        "eosio::chain_plugin",
        "--plugin",
        "eosio::producer_api_plugin",
        "--plugin",
        "eosio::producer_plugin",
    ]
    log(" ".join(cmd))
    procs.append(subprocess.Popen(cmd, stdout=stderr, stderr=stderr))
    wait_http(f"http://127.0.0.1:{args.http_port}/v1/chain/get_info", args.wait)
    log("channeld is producing")


def activate_preactivate(args: argparse.Namespace) -> None:
    url = f"http://127.0.0.1:{args.http_port}/v1/producer/schedule_protocol_feature_activations"
    log(f"POST {url} PREACTIVATE_FEATURE")
    http_post_json(url, {"protocol_features_to_activate": [PREACTIVATE_FEATURE]})
    time.sleep(1.0)


def activate_features(args: argparse.Namespace) -> None:
    skip = set()
    if args.no_forward_setcode:
        skip |= SAVANNA_AND_FORWARD
        log("skipping FORWARD_SETCODE and SAVANNA (--no-forward-setcode)")
    for name, digest in PROTOCOL_FEATURES:
        if name in skip:
            continue
        log(f"activate {name}")
        run(
            chan_cmd(args)
            + ["push", "action", "ra", "activate", json.dumps([digest]), "-p", "ra@active"]
        )
        time.sleep(0.6)


def deploy_optional(args: argparse.Namespace, account: str, wasm_stem: str, extra_dirs: list[str] | None = None) -> None:
    contract_dir = find_contract_dir(args.contracts_dir, wasm_stem, extra_dirs)
    if contract_dir is None:
        log(f"skip {account}: {wasm_stem}.wasm not found under {args.contracts_dir}")
        return
    set_contract(args, account, contract_dir, wasm_stem)
    if account in CODE_ACCOUNTS:
        run(chan_cmd(args) + ["set", "account", "permission", account, "active", "--add-code", "-p", f"{account}@active"])


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Launch a local Channel chain and boot ra.* contracts.")
    parser.add_argument("--build-dir", type=Path, default=REPO_ROOT / "build")
    parser.add_argument("--channeld")
    parser.add_argument("--chan")
    parser.add_argument("--keyd")
    parser.add_argument("--contracts-dir", type=Path, default=None)
    parser.add_argument("--genesis", type=Path, default=REPO_ROOT / "genesis" / "channel_genesis.json")
    parser.add_argument("--data-dir", type=Path, default=REPO_ROOT / "devnet")
    parser.add_argument("--public-key", default=DEFAULT_PUBLIC_KEY)
    parser.add_argument("--private-key", default=DEFAULT_PRIVATE_KEY)
    parser.add_argument("--symbol", default="RA")
    parser.add_argument("--max-supply", default="1000000000.0000")
    parser.add_argument("--resource-fund", default="10000000.0000", help="RA transferred to ra.resources with memo fund")
    parser.add_argument("--http-port", type=int, default=8888)
    parser.add_argument("--p2p-port", type=int, default=9876)
    parser.add_argument("--wallet-port", type=int, default=8900)
    parser.add_argument("--wait", type=float, default=30, help="Seconds to wait for HTTP")
    parser.add_argument("--reset", action="store_true", help="Wipe --data-dir before start")
    parser.add_argument("--no-forward-setcode", action="store_true", help="Keep native setcode intercept; skips SAVANNA")
    parser.add_argument("--no-boot", action="store_true", help="Start node and wallet only")
    parser.add_argument("--exit-after-boot", action="store_true", help="Stop channeld and keyd after a successful boot")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    args.build_dir = args.build_dir.expanduser().resolve()
    args.genesis = args.genesis.expanduser().resolve()
    args.data_dir = args.data_dir.expanduser().resolve()
    if args.contracts_dir is None:
        for candidate in (args.build_dir / "contracts", REPO_ROOT / "build" / "contracts", REPO_ROOT / "build-arm64" / "contracts"):
            if (candidate / "ra.token" / "ra.token.wasm").is_file() or (candidate / "ra.boot" / "ra.boot.wasm").is_file():
                args.contracts_dir = candidate
                break
        else:
            args.contracts_dir = args.build_dir / "contracts"
    args.contracts_dir = args.contracts_dir.expanduser().resolve()

    if not args.genesis.is_file():
        fail(f"genesis not found: {args.genesis}")

    args.channeld = find_binary("channeld", args.channeld, args.build_dir)
    args.chan = find_binary("chan", args.chan, args.build_dir)
    args.keyd = find_binary("keyd", args.keyd, args.build_dir)

    signal.signal(signal.SIGINT, lambda *_: (cleanup(), sys.exit(130)))
    signal.signal(signal.SIGTERM, lambda *_: (cleanup(), sys.exit(143)))

    start_keyd(args)
    start_channeld(args)

    if args.no_boot:
        log(f"node up at http://127.0.0.1:{args.http_port} (boot skipped)")
        if args.exit_after_boot:
            cleanup()
        else:
            for proc in procs:
                proc.wait()
        return

    boot = find_contract_dir(args.contracts_dir, "ra.boot")
    token = find_contract_dir(args.contracts_dir, "ra.token")
    msig = find_contract_dir(args.contracts_dir, "ra.msig")
    system = find_contract_dir(args.contracts_dir, "ra.system")
    if not all((boot, token, msig, system)):
        fail(
            f"need ra.boot, ra.token, ra.msig, and ra.system wasm under {args.contracts_dir}. "
            "Configure with -DBUILD_CONTRACTS=ON or pass --contracts-dir."
        )

    for account in SYSTEM_ACCOUNTS:
        run(chan_cmd(args) + ["create", "account", "ra", account, args.public_key, args.public_key])

    set_contract(args, "ra.token", token, "ra.token")
    set_contract(args, "ra.msig", msig, "ra.msig")

    supply = f"{args.max_supply} {args.symbol}"
    run(chan_cmd(args) + ["push", "action", "ra.token", "create", json.dumps(["ra", supply]), "-p", "ra.token@active"])
    run(chan_cmd(args) + ["push", "action", "ra.token", "issue", json.dumps(["ra", supply, "genesis"]), "-p", "ra@active"])

    activate_preactivate(args)
    set_contract(args, "ra", boot, "ra.boot")
    time.sleep(1.0)
    activate_features(args)
    set_contract(args, "ra", system, "ra.system")

    run(chan_cmd(args) + ["push", "action", "ra", "init", json.dumps([0, f"4,{args.symbol}"]), "-p", "ra@active"])
    run(chan_cmd(args) + ["push", "action", "ra", "setpriv", json.dumps(["ra.msig", 1]), "-p", "ra@active"])
    run(chan_cmd(args) + ["set", "account", "permission", "ra.msig", "active", "--add-code", "-p", "ra.msig@active"])

    deploy_optional(args, "ra.wrap", "ra.wrap")
    if find_contract_dir(args.contracts_dir, "ra.wrap") is not None:
        run(chan_cmd(args) + ["push", "action", "ra", "setpriv", json.dumps(["ra.wrap", 1]), "-p", "ra@active"])

    deploy_optional(args, "ra.fees", "ra.fees")
    deploy_optional(args, "ra.bpay", "ra.bpay")
    deploy_optional(args, "ra.resources", "ra.resources")
    deploy_optional(args, "ra.authex", "ra.authex")
    deploy_optional(args, "ra.claim", "ra.claim")
    deploy_optional(args, "ra.org", "ra.org")
    deploy_optional(args, "flex.token", "flex.token", ["ra.pure"])

    if find_contract_dir(args.contracts_dir, "ra.resources") is not None:
        qty = f"{args.resource_fund} {args.symbol}"
        run(
            chan_cmd(args)
            + ["push", "action", "ra.token", "transfer", json.dumps(["ra", "ra.resources", qty, "fund"]), "-p", "ra@active"]
        )
        run(
            chan_cmd(args)
            + [
                "push",
                "action",
                "ra.resources",
                "addplan",
                json.dumps(["10.0000 " + args.symbol, "10.0000 " + args.symbol, 2592000, "1.0000 " + args.symbol, "devnet month"]),
                "-p",
                "ra@active",
            ]
        )

    info = http_post_json(f"http://127.0.0.1:{args.http_port}/v1/chain/get_info", {})
    log(f"boot complete  chain_id={info.get('chain_id')}  head={info.get('head_block_num')}  producer={info.get('head_block_producer')}")
    log(f"chan --url http://127.0.0.1:{args.http_port} --wallet-url http://127.0.0.1:{args.wallet_port} get info")
    log("Ctrl-C to stop")

    if args.exit_after_boot:
        cleanup()
    else:
        for proc in procs:
            proc.wait()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        cleanup()
        sys.exit(130)
    except Exception:
        cleanup()
        raise

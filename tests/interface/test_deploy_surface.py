#!/usr/bin/env python3
"""Checks you can make without compiling channeld.

The node build is a separate workflow. This one looks at the wasm and ABI
CDT emits for the contracts a deployment actually setcodes, and at the EOSIO
wire layout those actions use.

  python3 tests/interface/test_deploy_surface.py build-contracts
"""

from __future__ import annotations

import json
import struct
import sys
from pathlib import Path


# Action name -> ordered (field, abi type). This is the on-chain interface.
REQUIRED = {
    "ra.bios": {
        "newaccount": [("creator", "name"), ("name", "name"), ("owner", "authority"), ("active", "authority")],
        "setcode": [("account", "name"), ("vmtype", "uint8"), ("vmversion", "uint8"), ("code", "bytes")],
        "setabi": [("account", "name"), ("abi", "bytes")],
        "setpriv": [("account", "name"), ("is_priv", "uint8")],
        "activate": [("feature_digest", "checksum256")],
    },
    "ra.boot": {
        "newaccount": [("creator", "name"), ("name", "name"), ("owner", "authority"), ("active", "authority")],
        "setcode": [("account", "name"), ("vmtype", "uint8"), ("vmversion", "uint8"), ("code", "bytes")],
        "activate": [("feature_digest", "checksum256")],
    },
    "ra.token": {
        "create": [("issuer", "name"), ("maximum_supply", "asset")],
        "issue": [("to", "name"), ("quantity", "asset"), ("memo", "string")],
        "transfer": [("from", "name"), ("to", "name"), ("quantity", "asset"), ("memo", "string")],
        "open": [("owner", "name"), ("symbol", "symbol"), ("ram_payer", "name")],
    },
    "ra.system": {
        "init": [("version", "varuint32"), ("core", "symbol")],
        "newaccount": [("creator", "name"), ("name", "name"), ("owner", "authority"), ("active", "authority")],
        "buyrambytes": [("payer", "name"), ("receiver", "name"), ("bytes", "uint32")],
        "setcode": [
            ("account", "name"),
            ("vmtype", "uint8"),
            ("vmversion", "uint8"),
            ("code", "bytes"),
            ("memo", "string$"),
        ],
    },
    "ra.authex": {
        "createlink": [("account", "name"), ("chain", "name"), ("address", "string")],
        "linksig": [
            ("account", "name"),
            ("chain", "name"),
            ("address", "string"),
            ("pubkey", "string"),
            ("nonce", "uint64"),
            ("sig", "bytes"),
        ],
        "bridgelink": [("account", "name"), ("chain", "name"), ("address", "string"), ("packedkey", "bytes")],
    },
    "ra.claim": {
        "credit": [("chain", "name"), ("ext_txid", "checksum256"), ("recipient", "name"), ("quantity", "asset")],
        "claim": [("id", "uint64")],
        "claimsig": [("id", "uint64"), ("sig", "bytes")],
        "creditopen": [
            ("chain", "name"),
            ("ext_txid", "checksum256"),
            ("recipient", "name"),
            ("quantity", "asset"),
            ("pubkey", "string"),
            ("packedkey", "bytes"),
        ],
        "claimopen": [("id", "uint64"), ("sig", "bytes")],
        "cfgclaim": [("ram_bytes", "uint32"), ("ram_fee", "asset")],
    },
    "flex.token": {
        "transfer": [("from", "name"), ("to", "name"), ("quantity", "asset"), ("memo", "string")],
        "setswap": [("swap", "name")],
        "setexempt": [("account", "name"), ("on", "bool")],
        "openpool": [
            ("id", "uint64"),
            ("token_a", "extended_asset"),
            ("token_b", "extended_asset"),
            ("sqrt_price_x64", "uint128"),
            ("fee", "uint32"),
        ],
        "seedpool": [
            ("id", "uint64"),
            ("alcor_pool_id", "uint64"),
            ("amount_a", "asset"),
            ("amount_b", "asset"),
            ("tick_lower", "int32"),
            ("tick_upper", "int32"),
            ("unlock_time", "uint32"),
        ],
        "collectpool": [("id", "uint64"), ("recipient", "name"), ("max_a", "asset"), ("max_b", "asset")],
        "smelt": [("username", "name"), ("quantity", "asset"), ("memo", "string")],
    },
}

FORBIDDEN = {
    "flex.token": ["subliquid", "transferpos", "burn"],
}


class Fail(Exception):
    pass


def char_to_value(ch: str) -> int:
    if ch == ".":
        return 0
    if "1" <= ch <= "5":
        return (ord(ch) - ord("1")) + 1
    if "a" <= ch <= "z":
        return (ord(ch) - ord("a")) + 6
    raise Fail(f"invalid name character {ch!r}")


def encode_name(name: str) -> int:
    # eosio::name: 12 symbols of 5 bits in the high bits, optional 13th in the low 4.
    if not name or len(name) > 13:
        raise Fail(f"bad name {name!r}")
    value = 0
    for i in range(12):
        value <<= 5
        if i < len(name):
            value |= char_to_value(name[i]) & 0x1F
    value <<= 4
    if len(name) == 13:
        value |= char_to_value(name[12]) & 0x0F
    return value


def encode_symbol(precision: int, code: str) -> int:
    if precision > 18 or not code or len(code) > 7:
        raise Fail(f"bad symbol {precision},{code}")
    value = precision & 0xFF
    for i, ch in enumerate(code):
        value |= ord(ch) << (8 * (i + 1))
    return value


def parse_asset(text: str) -> tuple[int, int]:
    amount_s, code = text.split(" ")
    whole, _, frac = amount_s.partition(".")
    precision = len(frac)
    negative = whole.startswith("-")
    digits = (whole[1:] if negative else whole) + frac
    amount = int(digits or "0")
    if negative:
        amount = -amount
    return amount, encode_symbol(precision, code)


def pack_u32(n: int) -> bytes:
    return struct.pack("<I", n)


def pack_u64(n: int) -> bytes:
    return struct.pack("<Q", n)


def pack_varuint(n: int) -> bytes:
    out = bytearray()
    while True:
        b = n & 0x7F
        n >>= 7
        if n:
            out.append(b | 0x80)
        else:
            out.append(b)
            return bytes(out)


def pack_bytes(blob: bytes) -> bytes:
    return pack_varuint(len(blob)) + blob


def pack_string(text: str) -> bytes:
    raw = text.encode()
    return pack_varuint(len(raw)) + raw


def pack_field(abi_type: str, value) -> bytes:
    if abi_type == "name":
        return pack_u64(encode_name(value))
    if abi_type == "uint64":
        return pack_u64(value)
    if abi_type == "uint32":
        return pack_u32(value)
    if abi_type == "asset":
        amount, sym = parse_asset(value)
        return struct.pack("<qQ", amount, sym)
    if abi_type == "bytes":
        return pack_bytes(value)
    if abi_type == "string":
        return pack_string(value)
    raise Fail(f"test packer does not cover {abi_type}")


def test_wire() -> None:
    # Published eosio::name("eosio").
    if encode_name("eosio") != 6138663577826885632:
        raise Fail("name encoder drifted from eosio::name")
    if encode_name("ra") != 0xB980000000000000:
        raise Fail("system account name did not encode")
    # 9.0000 RA — amount 90000, symbol precision 4, code RA.
    amount, sym = parse_asset("9.0000 RA")
    if (amount, sym) != (90000, 0x415204):
        raise Fail(f"asset layout {amount:#x} {sym:#x}")
    claimopen = pack_field("uint64", 1) + pack_field("bytes", bytes.fromhex("abcd"))
    if claimopen.hex() != "010000000000000002abcd":
        raise Fail(f"claimopen wire {claimopen.hex()}")
    transfer = b"".join(
        [
            pack_field("name", "ra.claim"),
            pack_field("name", "alice"),
            pack_field("asset", "9.0000 RA"),
            pack_field("string", "cross-chain claim"),
        ]
    )
    expected = (
        "000000d2998880b9"
        "0000000000855c34"
        "905f0100000000000452410000000000"
        "1163726f73732d636861696e20636c61696d"
    )
    if transfer.hex() != expected:
        raise Fail(f"transfer wire {transfer.hex()}")


def load_abi(path: Path) -> dict:
    data = json.loads(path.read_text())
    if not str(data.get("version", "")).startswith("eosio::abi/"):
        raise Fail(f"{path} is not an eosio ABI")
    return data


def struct_fields(abi: dict, type_name: str) -> list[tuple[str, str]]:
    structs = {s["name"]: s for s in abi.get("structs", [])}
    seen = []
    while type_name:
        if type_name not in structs:
            raise Fail(f"ABI struct {type_name} missing")
        row = structs[type_name]
        seen.append(row)
        type_name = row.get("base") or ""
    fields = []
    for row in reversed(seen):
        for field in row.get("fields", []):
            fields.append((field["name"], field["type"]))
    return fields


def wasm_exports(blob: bytes) -> list[str]:
    if blob[:4] != b"\x00asm":
        raise Fail("wasm magic missing")
    if len(blob) < 1000:
        raise Fail("wasm is too small to be a contract")

    def uleb(i: int) -> tuple[int, int]:
        n = shift = 0
        while True:
            c = blob[i]
            i += 1
            n |= (c & 0x7F) << shift
            if c < 128:
                return n, i
            shift += 7

    off = 8
    names = []
    while off < len(blob):
        sid = blob[off]
        off += 1
        size, off = uleb(off)
        end = off + size
        if sid == 7:
            count, i = uleb(off)
            for _ in range(count):
                ln, i = uleb(i)
                name = blob[i : i + ln].decode()
                i += ln
                i += 1  # kind
                _, i = uleb(i)
                names.append(name)
            return names
        off = end
    raise Fail("wasm has no export section")


def find_pair(root: Path, stem: str) -> tuple[Path, Path]:
    wasm = sorted(root.rglob(stem + ".wasm"))
    abi = sorted(root.rglob(stem + ".abi"))
    if not wasm or not abi:
        raise Fail(f"missing {stem}.wasm/.abi under {root}")
    return wasm[0], abi[0]


def test_contracts(root: Path) -> None:
    for stem, actions in REQUIRED.items():
        wasm_path, abi_path = find_pair(root, stem)
        exports = wasm_exports(wasm_path.read_bytes())
        if "apply" not in exports:
            raise Fail(f"{stem} wasm does not export apply ({exports})")
        abi = load_abi(abi_path)
        by_name = {a["name"]: a["type"] for a in abi.get("actions", [])}
        for action, fields in actions.items():
            if action not in by_name:
                raise Fail(f"{stem} ABI has no action {action}; have {sorted(by_name)}")
            got = struct_fields(abi, by_name[action])
            if got != fields:
                raise Fail(f"{stem}::{action} fields {got} != {fields}")
        for banned in FORBIDDEN.get(stem, []):
            if banned in by_name:
                raise Fail(f"{stem} must not expose {banned}")
        # openpool's tail is Alcor createpool: account is the caller, not an arg here.
        if stem == "flex.token":
            tail = [typ for _, typ in actions["openpool"][1:]]
            if tail != ["extended_asset", "extended_asset", "uint128", "uint32"]:
                raise Fail(f"openpool is not createpool-shaped: {tail}")


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: test_deploy_surface.py <contract-build-dir>", file=sys.stderr)
        return 2
    root = Path(sys.argv[1])
    if not root.is_dir():
        print(f"not a directory: {root}", file=sys.stderr)
        return 2
    try:
        test_wire()
        test_contracts(root)
    except Fail as exc:
        print(f"FAIL {exc}", file=sys.stderr)
        return 1
    print(f"ok {len(REQUIRED)} contracts, wire layout pinned")
    return 0


if __name__ == "__main__":
    sys.exit(main())

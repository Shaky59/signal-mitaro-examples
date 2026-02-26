import argparse
import json
import sys
import time
from pathlib import Path

import serial
from jsonschema import Draft202012Validator


def load_schema(schema_path: Path) -> dict:
    with schema_path.open("r", encoding="utf-8") as f:
        return json.load(f)


def make_validator(schema: dict) -> Draft202012Validator:
    v = Draft202012Validator(schema)
    # ここで schema自体の整合性チェックもできる（任意）
    v.check_schema(schema)
    return v


def now_ms() -> int:
    return int(time.time() * 1000)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", required=True, help="Serial port (e.g. COM5 or /dev/ttyUSB0)")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--schema", default="schemas/event_v1.json")
    ap.add_argument("--strict", action="store_true", help="Exit on first invalid event")
    ap.add_argument("--reject", default="reject.ndjson", help="Reject log path")
    ap.add_argument("--badline", default="badline.log", help="Non-JSON line log path")
    args = ap.parse_args()

    schema_path = Path(args.schema)
    schema = load_schema(schema_path)
    validator = make_validator(schema)

    transport = f"serial:{args.port}@{args.baud}"
    recv_seq = 0

    ser = serial.Serial(args.port, args.baud, timeout=1)

    sys.stderr.write(f"[receiver] open {transport}\n")
    sys.stderr.flush()

    with open(args.reject, "a", encoding="utf-8") as rej, open(args.badline, "a", encoding="utf-8") as bad:
        while True:
            line = ser.readline()
            if not line:
                continue

            # 改行単位（NDJSON）
            try:
                s = line.decode("utf-8", errors="replace").strip()
            except Exception:
                continue

            if not s:
                continue

            recv_seq += 1
            recv_ts = now_ms()

            # JSON parse
            try:
                obj = json.loads(s)
            except json.JSONDecodeError as e:
                bad.write(f"{recv_ts}\t{transport}\tseq={recv_seq}\tJSON_DECODE_ERROR\t{e}\t{s}\n")
                bad.flush()
                if args.strict:
                    raise
                continue

            # Schema validate
            errors = sorted(validator.iter_errors(obj), key=lambda x: x.path)
            if errors:
                # reject.ndjson へ「理由付き」で保存（仕様と実装を一致させるための証跡）
                rec = {
                    "recv_meta": {
                        "recv_ts_ms": recv_ts,
                        "recv_seq": recv_seq,
                        "transport": transport,
                    },
                    "reason": [f"{'/'.join(map(str, e.path))}: {e.message}" for e in errors],
                    "raw": obj,
                }
                rej.write(json.dumps(rec, ensure_ascii=False) + "\n")
                rej.flush()
                if args.strict:
                    sys.stderr.write("[receiver] strict mode: invalid event -> exit\n")
                    sys.exit(2)
                continue

            # OK: 受信メタを付与して stdout に出す（Phase1の最小ゴール）
            out = dict(obj)
            out["recv_meta"] = {
                "recv_ts_ms": recv_ts,
                "recv_seq": recv_seq,
                "transport": transport,
            }
            print(json.dumps(out, ensure_ascii=False), flush=True)


if __name__ == "__main__":
    main()

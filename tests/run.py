#!/usr/bin/env python3
"""Parallel test runner for milang-bits.

Discovers and runs all test_*.mi files across library directories.
Modeled on milang-copilot/tests/run.py.

Usage:
    python tests/run.py -m ./milang
    python tests/run.py -m ./milang -n 4 -t 30
    python tests/run.py -m ./milang --exclude sdl  # skip SDL tests (need display)
"""

import argparse
import collections
import multiprocessing
import os
import subprocess
import sys
import time

DummyResult = collections.namedtuple("DummyResult", ["stdout", "stderr", "returncode"])

parser = argparse.ArgumentParser(description="Run milang-bits tests")
parser.add_argument(
    "-n", "--n-workers", type=int, help="Number of tests to run in parallel"
)
parser.add_argument("-m", "--milang", default="./milang", help="Milang executable")
parser.add_argument(
    "-t", "--timeout", type=int, default=30, help="Timeout per test (seconds)"
)
parser.add_argument(
    "--cc", default=None, help="C compiler to pass to milang (e.g. gcc, clang)"
)
parser.add_argument(
    "--exclude", nargs="*", default=["sdl"], help="Library dirs to exclude (default: sdl)"
)
parser.add_argument(
    "--interpret", action="store_true", help="Run with --interpret flag"
)
parser.add_argument(
    "-v", "--verbose", action="store_true", help="Show each test as it completes"
)


def find_tests(root, exclude):
    """Find all test_*.mi files in */tests/ directories."""
    tests = []
    for entry in sorted(os.listdir(root)):
        if entry in exclude or entry.startswith("."):
            continue
        test_dir = os.path.join(root, entry, "tests")
        if not os.path.isdir(test_dir):
            continue
        for item in sorted(os.listdir(test_dir)):
            if item.startswith("test_") and item.endswith(".mi"):
                tests.append(os.path.join(test_dir, item))
    # Also check bugs/
    for extra in ["bugs"]:
        extra_dir = os.path.join(root, extra)
        if os.path.isdir(extra_dir):
            for item in sorted(os.listdir(extra_dir)):
                if item.endswith(".mi"):
                    tests.append(os.path.join(extra_dir, item))
    return tests


def run(filename, milang, timeout, cc, interpret):
    """Run a single test file, return result dict."""
    cmd = [milang]
    if cc:
        cmd += ["--cc", cc]
    cmd += ["run"]
    if interpret:
        cmd += ["--interpret"]
    cmd += [filename]

    start = time.monotonic()
    try:
        result = subprocess.run(cmd, timeout=timeout, capture_output=True, text=True)
    except subprocess.TimeoutExpired:
        elapsed = time.monotonic() - start
        return dict(
            success=False,
            name=filename,
            summary="TIMEOUT: {} ({:.1f}s)".format(os.path.relpath(filename), elapsed),
            output=DummyResult("", "timed out after {}s".format(timeout), -1),
            elapsed=elapsed,
        )
    elapsed = time.monotonic() - start

    # Check for FAIL in output (milang-bits convention: tests print PASS/FAIL lines)
    has_fail = False
    for line in result.stdout.splitlines():
        if line.startswith("FAIL:") or line.startswith("FAIL "):
            has_fail = True
            break

    if result.returncode != 0:
        return dict(
            success=False,
            name=filename,
            summary="ERROR: {} (exit code {})".format(
                os.path.relpath(filename), result.returncode
            ),
            output=result,
            elapsed=elapsed,
        )

    if has_fail:
        return dict(
            success=False,
            name=filename,
            summary="FAIL: {}".format(os.path.relpath(filename)),
            output=result,
            elapsed=elapsed,
        )

    return dict(
        success=True,
        name=filename,
        summary="PASS: {} ({:.1f}s)".format(os.path.relpath(filename), elapsed),
        output=result,
        elapsed=elapsed,
    )


def main(n_workers, milang, timeout, cc, exclude, interpret, verbose):
    milang = os.path.abspath(milang)
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

    tests = find_tests(root, set(exclude or []))
    if not tests:
        print("No test files found!")
        sys.exit(1)

    print("Running {} tests with {} workers...".format(
        len(tests), n_workers or multiprocessing.cpu_count()
    ))

    start = time.monotonic()

    with multiprocessing.Pool(n_workers) as p:
        thunks = []
        for t in tests:
            thunks.append(
                p.apply_async(run, (t, milang, timeout, cc, interpret))
            )

        results = []
        for thunk in thunks:
            result = thunk.get()
            results.append(result)
            if verbose:
                status = "✓" if result["success"] else "✗"
                print("  {} {}".format(status, result["summary"]))

    total_time = time.monotonic() - start
    passed = sum(1 for r in results if r["success"])
    failed = sum(1 for r in results if not r["success"])
    failures = [r for r in results if not r["success"]]

    if failures:
        print()
        for result in failures:
            print(result["summary"])
            for name in ("stdout", "stderr"):
                value = getattr(result["output"], name, "").strip()
                if not value:
                    continue
                lines = value.splitlines()
                if len(lines) > 20:
                    lines = lines[:10] + ["  ... ({} lines omitted)".format(len(lines) - 20)] + lines[-10:]
                print("  {}:".format(name))
                for line in lines:
                    print("    {}".format(line))
            print()

    print("─" * 50)
    print("{} passed, {} failed ({:.1f}s total)".format(passed, failed, total_time))
    sys.exit(failed)


if __name__ == "__main__":
    main(**vars(parser.parse_args()))

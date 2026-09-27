# Prompt 4 — test driver

## Prompt (verbatim)

```
Write tests/test_extract.cpp that #includes the extractor (declare
extractIPv4 in src/ipv4_extract.h and move main behind #ifndef UNIT_TEST).
For each row of this table assert the bool, outAddress and outPort, print
PASS/FAIL per row and a summary, and exit non-zero on any failure.
Also assert that outputs are reset to 0/-1 when a failing call follows a
successful one. No test framework, no banned functions.
<paste the test table>
```

(Test table: `tests/cases.md`, all 32 rows.)

## Response

See `tests/test_extract.cpp`, `src/ipv4_extract.h`, and the `#ifndef UNIT_TEST`
guard added to `src/ipv4_extract.cpp`.

## Check against the oracle

Every expected (bool, address, port) triple in `test_extract.cpp` was diffed
by hand against `tests/cases.md` cell-by-cell before trusting a single PASS —
a test that copies a wrong expectation would hide the bug it's meant to
catch. No mismatches found.

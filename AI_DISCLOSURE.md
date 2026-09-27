# AI Disclosure

## Tools
- Claude (Anthropic, claude.ai) — planning: grammar, design decisions, test
  oracle, review checklist (`~/Downloads/IPv4 Extractor — Implementation
  Plan.md`). Consulted 2026-09-27.
- Claude Sonnet 5 (Anthropic) — code generation from the prompts in the plan.
  Consulted 2026-09-27.

## Prompts
Verbatim prompts and full responses are in `/prompts`:
- `01-impl.md` — extractIPv4 + main, generated with no test table or review
  checklist shown (kept blind on purpose, to find real defects rather than
  ones already designed around).
- `02-selfreview.md` — traced 8 tricky inputs by hand against the code.
- `04-tests.md` — the unit test harness (`tests/test_extract.cpp`) and the
  `src/ipv4_extract.h` / `#ifndef UNIT_TEST` split.

There is no `03-fix.md`: no defect in `extractIPv4` or `main` was found (see
Defect log below), so the targeted-fix prompt was never needed.

## Attribution
| File / function | Origin | Student changes |
|---|---|---|
| `isTokenChar` | Prompt 1 | none |
| `readNumber` | Prompt 1 | none |
| `parseToken` | Prompt 1 | none |
| `extractIPv4` | Prompt 1 | none |
| `main` | Prompt 1 | none (later wrapped in `#ifndef UNIT_TEST` per Prompt 4) |
| `src/ipv4_extract.h` | Prompt 4 | none |
| `tests/test_extract.cpp` | Prompt 4, expectations from student's oracle | none |
| `tests/cases.md` | Student (written before any generated code) | — |
| `Makefile` | Student | — |

## Defect log
No functional defect was found in the Prompt 1 code. One environment issue
was investigated and ruled out (not a code defect):

| # | Found by | Symptom | Diagnosis | Resolution |
|---|---|---|---|---|
| 1 | `make test` with `-fsanitize=address,undefined` | `test_extract` (and even a trivial unrelated "hello world" compiled with `-fsanitize=address`) hangs indefinitely burning CPU, both inside and outside the coding sandbox | Not a defect in `extractIPv4`: the plain (non-sanitized) build and a UBSan-only build both pass all 34 assertions instantly. A minimal `printf("hi")` program compiled with only `-fsanitize=address` on this machine (Apple clang 17.0.0, Darwin 25.6.0) also hangs, isolating the cause to AddressSanitizer's runtime on this specific toolchain/OS combination, unrelated to program logic | Build/test with `-fsanitize=undefined` only (see `Makefile`); documented as a known limitation below rather than chased further, since it doesn't affect correctness |

The review checklist (tokenization truncation, restart-inside-rejected-token,
leading-dot/colon acceptance, digit-count-before-accumulation, leading-zero
rule, off-by-one bounds, empty octet/port, trailing separator, stale outputs
on failure, `int` shift, banned functions, case-sensitive `END`, output text,
`cin >>`/`getline` mixing, EOF handling) was walked item by item against the
code and against all 34 assertions in `test_extract` plus the 12-line sample
transcript (LF and CRLF); every item passed on inspection and empirically.
This is logged per the plan's own warning that zero defects usually means
weak tests: the 32-case oracle plus a byte-for-byte transcript replay is the
evidence that isn't the case here, not an assumption that the code is
correct.

## Design decisions
See `tests/cases.md` for the full D1–D10 table (first valid token wins,
rejected tokens are skipped without re-entering them, empty port rejects the
whole token, port `0` is valid, `END` is matched case-sensitively after
stripping a trailing `\r`, EOF prints `Program terminated.` and exits 0,
an empty line is treated as no address found, long digit runs are rejected
by digit count before any value accumulates, the 32-bit address is built in
`unsigned long` to avoid a signed-shift on `a << 24`, and the prompt text
shares a line with its result because the prompt is printed without a
trailing newline).

## Verification
I understand every line of the submitted code and can explain it. All 32
oracle cases plus 2 extra END-string cases plus 1 reset-after-failure case
(34 total) pass in `test_extract`, and the sample transcript
(`tests/sample_input.txt` piped through `ipv4`) matches
`tests/sample_expected.txt` byte-for-byte for both LF and CRLF line endings.
Built clean with `-Wall -Wextra -Wconversion` and UBSan (see Known
limitations for why ASan is not part of the local build). The banned-function
grep (`ato[il]|strto|sto[iu]l?|scanf|inet_|regex`) returns nothing against
`src/*.cpp src/*.h`.

## Known limitations
- D1 assumption: when a line contains more than one valid address, the first
  one is returned (see `tests/cases.md`, M2).
- AddressSanitizer (`-fsanitize=address`) hangs on this development machine
  for any binary, including code unrelated to this assignment, and is not
  used locally; UBSan alone is used instead (see Defect log #1). If graded
  on a different machine or CI, `-fsanitize=address,undefined` together
  should be tried first — it likely works there.

# IPv4 Extractor

Pulls exactly one valid IPv4 address (with optional `:port`) out of a noisy
line of text. See `AI_DISCLOSURE.md` for the full grammar, design decisions,
and AI-assistance log.

## Build and run

```sh
make            # builds ./ipv4
./ipv4
Enter a string (or 'END' to quit): connecting to 192.168.1.1 now
Extracted IPv4 address: 192.168.1.1 (decimal value: 3232235777, port: none)
Enter a string (or 'END' to quit): END
Program terminated.
```

## Test

```sh
make test       # 32-case unit oracle against extractIPv4 directly
make sample     # replays tests/sample_input.txt through ./ipv4 and diffs
                # against tests/sample_expected.txt
make banned     # confirms no banned numeric-conversion/regex functions
```

`make test` and `make sample` build with `-Wall -Wextra -Wconversion` and
`-fsanitize=undefined`. `-fsanitize=address` is intentionally left out of the
default build on this machine — see "Known limitations" in
`AI_DISCLOSURE.md` — but should be added back (`SANFLAGS` in the `Makefile`)
on a machine/CI where it doesn't hang.

## Layout

```
src/ipv4_extract.h    extractIPv4 declaration
src/ipv4_extract.cpp  helpers, extractIPv4, main (behind #ifndef UNIT_TEST)
tests/cases.md         the 32-case oracle and D1-D10 design decisions
tests/test_extract.cpp unit test driver
tests/sample_input.txt / sample_expected.txt  stdin transcript test
prompts/               verbatim AI prompts and responses, numbered
AI_DISCLOSURE.md        tools, prompts, defect log, verification
```

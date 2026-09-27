# Prompt 2 — self-review

## Prompt (verbatim)

```
Review your extractIPv4 for these inputs and state the exact return value,
outAddress and outPort for each, tracing the code rather than guessing:
1.2.3.4.5 | .1.2.3.4 | 1.2.3.4: | 1.2.3.4:80. | 00.1.2.3 | 1.2.3.4a:80 |
99999999999999999999.1.1.1 | 255.255.255.255
If any trace disagrees with the spec I gave, fix the code and explain the fix.
```

## Traced response

- `1.2.3.4.5` — one maximal token run (all chars are digits/`.`). After the
  4th octet `4` is read, `i` points at the `.` before `5`; the code after the
  octet loop requires that char to be `:` or end-of-token, and `.` is
  neither, so `parseToken` returns false. No other run exists.
  **return false, outAddress=0, outPort=-1.**
- `.1.2.3.4` — one run starting with `.`. The first `readNumber` call sees a
  non-digit at position 0, reads zero digits, returns false immediately.
  **return false, outAddress=0, outPort=-1.**
- `1.2.3.4:` — after octet 4, `i` is at `:` (last char). The `:` branch
  consumes it and calls `readNumber` with `i == end`; that loop body never
  runs, `count == 0`, returns false. **return false, outAddress=0, outPort=-1.**
- `1.2.3.4:80.` — port `80` parses to value 80 with `i` left at the trailing
  `.`. The final `if (i != end) return false;` catches the leftover char.
  **return false, outAddress=0, outPort=-1.**
- `00.1.2.3` — first octet reads digits `0`,`0` (count=2), value=0, then the
  leading-zero check `count > 1 && s[start]=='0'` is true, so `readNumber`
  returns false for octet 1. **return false, outAddress=0, outPort=-1.**
- `1.2.3.4a:80` — `'a'` is not a token char, so the maximal run is just
  `1.2.3.4`, which parses as a complete, portless address and is accepted on
  its own; the trailing `a:80` is a separate run never reached because the
  first valid token already caused `extractIPv4` to return.
  **return true, outAddress=16909060 (1.2.3.4), outPort=-1.**
- `99999999999999999999.1.1.1` — reading the first octet's digits, `count`
  exceeds `maxDigits` (3) on the 4th `'9'`, so `readNumber` returns false
  before `value` ever accumulates past 3 digits worth (999) — no overflow,
  and the octet fails outright. **return false, outAddress=0, outPort=-1.**
- `255.255.255.255` — each octet reads 3 digits, value=255 <= 255, leading
  digit is `'2'` so the leading-zero check doesn't trigger. All 4 octets
  valid, no `:` follows the 4th, `i == end`. Packed value =
  `255<<24 | 255<<16 | 255<<8 | 255` computed in `unsigned long`, no signed
  shift. **return true, outAddress=4294967295, outPort=-1.**

## Outcome

Every trace matches the oracle in `tests/cases.md`. No disagreement with the
spec was found on this pass, so no code change was made here. (The
`test_extract` run in the checklist step independently exercises the same
inputs plus 24 more and confirms these traces empirically — see the defect
log in `AI_DISCLOSURE.md`.)

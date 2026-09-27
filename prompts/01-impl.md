# Prompt 1 — implementation

Sent to a fresh Claude Sonnet 5 session (no test table or checklist shown).

## Prompt (verbatim)

```
Write a single-file C++17 program (src/ipv4_extract.cpp) that implements exactly:

  bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort);

Spec:
- Token chars are only digits, '.', ':'. Every other char is garbage.
- A candidate token is a MAXIMAL run of token chars. It must match the grammar
  in full or be rejected whole. Never truncate or search inside a run.
- Grammar: octet.octet.octet.octet[:port]
  octet: 1-3 digits, 0-255, no leading zero unless the value is exactly 0.
  port: 1-5 digits, 0-65535, same leading-zero rule.
  If ':' is present the port must be fully valid, else the whole token is rejected.
- Return the FIRST valid token on the line. Rejected tokens are skipped and
  scanning continues after them.
- On success: outAddress = A*2^24 + B*2^16 + C*2^8 + D; outPort = port or -1.
- On failure: outAddress = 0, outPort = -1.
- Forbidden anywhere in the file: atoi, atol, atoll, strtol, strtoul, strtod,
  stoi, stol, stoul, sscanf, scanf numeric conversions, inet_aton, inet_pton,
  inet_addr, std::regex, regex.h. isdigit is allowed. Accumulate digits by hand.

main():
- Loop: print "Enter a string (or 'END' to quit): " (no newline), read a line
  with std::getline, strip one trailing '\r'.
- If the line is exactly "END" (case-sensitive) or stdin hits EOF, print
  "Program terminated." and exit 0.
- On success print:
  Extracted IPv4 address: A.B.C.D (decimal value: N, port: P)
  where P is the port or the word none.
- On failure print:
  Invalid input: no valid IPv4 address found

Structure it as small static helpers (token-char test, bounded number reader,
token validator) plus extractIPv4 and main. Check digit count before value so
long digit runs cannot overflow. Do the 32-bit math in unsigned long.
Comment each helper with its contract. Output only the code.
```

## Response

See `src/ipv4_extract.cpp` (committed untouched from this prompt's output).

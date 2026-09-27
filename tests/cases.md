# Test oracle (32 cases)

Written before seeing any Sonnet code. Decimal = A*16777216 + B*65536 + C*256 + D.
"Invalid" means extractIPv4 returns false, outAddress == 0, outPort == -1.

| # | Input | Expected (address · decimal · port) | Checks |
| --- | --- | --- | --- |
| S1 | `connecting to 192.168.1.1 now` | 192.168.1.1 · 3232235777 · none | Sample |
| S2 | `server=10.0.0.255:8080end` | 10.0.0.255 · 167772415 · 8080 | Sample, port then garbage |
| S3 | `192a168.1.1.1` | 168.1.1.1 · 2818638081 · none | Sample, garbage splits tokens |
| S4 | `192.168.1.1.` | Invalid | Sample, trailing period |
| S5 | `Connection from 192.168.1.1 refused` | 192.168.1.1 · 3232235777 · none | Sample |
| S6 | `192.168.01.1` | Invalid | Sample, leading zero |
| S7 | `1.2.3.4:99999` | Invalid | Sample, port range |
| S8 | `12.34.56` | Invalid | Sample, 3 octets |
| S9 | `no number here` | Invalid | Sample, no token |
| B1 | `0.0.0.0` | 0.0.0.0 · 0 · none | Lower bound, single `0` allowed |
| B2 | `255.255.255.255` | 255.255.255.255 · 4294967295 · none | Upper bound, unsigned math |
| B3 | `255.255.255.255:65535` | 255.255.255.255 · 4294967295 · 65535 | Max port |
| B4 | `1.2.3.4:0` | 1.2.3.4 · 16909060 · 0 | Port zero (D4) |
| B5 | `256.1.1.1` | Invalid | Octet 256 |
| B6 | `1.2.3.4:65536` | Invalid | Port 65536 |
| B7 | `1234.1.1.1` | Invalid | 4-digit octet |
| B8 | `99999999999999999999.1.1.1` | Invalid | Overflow guard (D8) |
| G1 | `1.2.3.4.5` | Invalid | No truncation to a sub-address |
| G2 | `.1.2.3.4` | Invalid | Leading stray period |
| G3 | `:1.2.3.4` | Invalid | Leading stray colon |
| G4 | `1..2.3.4` | Invalid | Empty octet |
| G5 | `00.1.2.3` | Invalid | Double zero |
| G6 | `1.2.3.4:` | Invalid | Empty port (D3) |
| G7 | `1.2.3.4:80:90` | Invalid | Second colon |
| G8 | `1.2.3.4:080` | Invalid | Port leading zero |
| G9 | `1.2.3.4:80.` | Invalid | Stray period after port |
| G10 | `1.2.3.4a:80` | 1.2.3.4 · 16909060 · none | Garbage separates colon from address |
| M1 | `bad 1.2.3 then 10.20.30.40` | 10.20.30.40 · 169090600 · none | Skip rejected token (D2) |
| M2 | `10.0.0.1 and 10.0.0.2` | 10.0.0.1 · 167772161 · none | First valid wins (D1) |
| M3 | `at 12:30:45 host 8.8.8.8:53` | 8.8.8.8 · 134744072 · 53 | Timestamp-shaped noise |
| L1 | *(empty line)* | Invalid | D7 |
| L2 | `end` and ` END  ` (trailing space) | Invalid, loop continues | Only exact `END` quits (D5) |

## Design decisions (D1-D10)

| # | Question | Decision |
| --- | --- | --- |
| D1 | Two valid addresses on one line | Return the first valid token |
| D2 | Rejected token followed by a valid one | Keep scanning; the later one can win |
| D3 | Empty port (`1.2.3.4:`) | Reject the whole token |
| D4 | Port `0` | Valid |
| D5 | `END` detection | Exact match of the whole line after stripping a trailing `\r` |
| D6 | EOF before `END` | Print `Program terminated.` and exit 0 |
| D7 | Empty line | Treated as input with no address -> invalid message |
| D8 | Very long digit runs | Reject on digit count before accumulating further |
| D9 | Integer width for the 32-bit value | Build in `unsigned long` with casts before shifting |
| D10 | Output when a prompt and a result share a line | Prompt printed without newline; result on its own line |

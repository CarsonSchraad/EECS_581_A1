#include "../src/ipv4_extract.h"
#include <cstdio>
#include <string>

struct Case {
    const char* id;
    std::string input;
    bool expectOk;
    unsigned long expectAddr;
    int expectPort;
};

static const Case kCases[] = {
    {"S1", "connecting to 192.168.1.1 now", true, 3232235777UL, -1},
    {"S2", "server=10.0.0.255:8080end", true, 167772415UL, 8080},
    {"S3", "192a168.1.1.1", true, 2818638081UL, -1},
    {"S4", "192.168.1.1.", false, 0UL, -1},
    {"S5", "Connection from 192.168.1.1 refused", true, 3232235777UL, -1},
    {"S6", "192.168.01.1", false, 0UL, -1},
    {"S7", "1.2.3.4:99999", false, 0UL, -1},
    {"S8", "12.34.56", false, 0UL, -1},
    {"S9", "no number here", false, 0UL, -1},
    {"B1", "0.0.0.0", true, 0UL, -1},
    {"B2", "255.255.255.255", true, 4294967295UL, -1},
    {"B3", "255.255.255.255:65535", true, 4294967295UL, 65535},
    {"B4", "1.2.3.4:0", true, 16909060UL, 0},
    {"B5", "256.1.1.1", false, 0UL, -1},
    {"B6", "1.2.3.4:65536", false, 0UL, -1},
    {"B7", "1234.1.1.1", false, 0UL, -1},
    {"B8", "99999999999999999999.1.1.1", false, 0UL, -1},
    {"G1", "1.2.3.4.5", false, 0UL, -1},
    {"G2", ".1.2.3.4", false, 0UL, -1},
    {"G3", ":1.2.3.4", false, 0UL, -1},
    {"G4", "1..2.3.4", false, 0UL, -1},
    {"G5", "00.1.2.3", false, 0UL, -1},
    {"G6", "1.2.3.4:", false, 0UL, -1},
    {"G7", "1.2.3.4:80:90", false, 0UL, -1},
    {"G8", "1.2.3.4:080", false, 0UL, -1},
    {"G9", "1.2.3.4:80.", false, 0UL, -1},
    {"G10", "1.2.3.4a:80", true, 16909060UL, -1},
    {"M1", "bad 1.2.3 then 10.20.30.40", true, 169090600UL, -1},
    {"M2", "10.0.0.1 and 10.0.0.2", true, 167772161UL, -1},
    {"M3", "at 12:30:45 host 8.8.8.8:53", true, 134744072UL, 53},
    {"L1", "", false, 0UL, -1},
    // L2: END-quit detection is exercised by the main-loop transcript test
    // (tests/sample_input.txt); as bare strings neither contains a token
    // char, so extractIPv4 must reject both like any other non-address line.
    {"L2a", "end", false, 0UL, -1},
    {"L2b", " END  ", false, 0UL, -1},
    // E1-E7: added after fuzzing against an independent reference
    // implementation (see AI_DISCLOSURE.md); no bug found, but these close
    // gaps the original 32 cases didn't cover.
    {"E1", "1.2:3.4", false, 0UL, -1},
    {"E2", "1:2.3.4", false, 0UL, -1},
    {"E3", "1.2.3:4", false, 0UL, -1},
    {"E4", "-1.2.3.4", true, 16909060UL, -1},
    {"E5", "1.2.3.256", false, 0UL, -1},
    {"E6", "255255255255", false, 0UL, -1},
    {"E7", "1.2.3.4:1", true, 16909060UL, 1},
};

int main() {
    int total = 0;
    int failed = 0;

    for (const Case& c : kCases) {
        total++;
        unsigned long addr = 0xDEADBEEFUL;
        int port = -99;
        bool ok = extractIPv4(c.input, addr, port);

        bool pass = (ok == c.expectOk) && (addr == c.expectAddr) && (port == c.expectPort);
        std::printf("[%s] %-4s  ", pass ? "PASS" : "FAIL", c.id);
        if (!pass) {
            failed++;
            std::printf("input=\"%s\" got(ok=%d addr=%lu port=%d) want(ok=%d addr=%lu port=%d)",
                        c.input.c_str(), ok, addr, port, c.expectOk, c.expectAddr, c.expectPort);
        }
        std::printf("\n");
    }

    // A failing call that follows a successful one must not leak the prior
    // success into outAddress/outPort.
    {
        total++;
        unsigned long addr = 0;
        int port = -1;
        extractIPv4("192.168.1.1", addr, port);
        bool ok2 = extractIPv4("no number here", addr, port);
        bool pass = (ok2 == false) && (addr == 0UL) && (port == -1);
        std::printf("[%s] RESET input=(success then failure) got(ok=%d addr=%lu port=%d)\n",
                    pass ? "PASS" : "FAIL", ok2, addr, port);
        if (!pass) {
            failed++;
        }
    }

    std::printf("\n%d / %d passed\n", total - failed, total);
    return failed == 0 ? 0 : 1;
}

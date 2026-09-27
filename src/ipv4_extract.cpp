#include "ipv4_extract.h"
#include <iostream>
#include <string>

// A token char is a digit, '.', or ':'. Everything else is garbage that
// splits the line into candidate runs.
static bool isTokenChar(char c) {
    return (c >= '0' && c <= '9') || c == '.' || c == ':';
}

// Reads a run of up to maxDigits decimal digits starting at s[i] (i < end),
// accumulating into out by hand. Advances i past the digits it consumed.
// Fails (returns false) if there are zero digits, more than maxDigits
// digits, the accumulated value exceeds maxValue, or the run has a leading
// zero followed by another digit (i.e. is not exactly "0"). Digit count is
// checked before the value grows, so an arbitrarily long run of digits
// cannot overflow `out`.
static bool readNumber(const std::string& s, size_t& i, size_t end,
                        int maxDigits, long maxValue, long& out) {
    size_t start = i;
    long value = 0;
    int count = 0;
    while (i < end && s[i] >= '0' && s[i] <= '9') {
        count++;
        if (count > maxDigits) {
            return false;
        }
        value = value * 10 + (s[i] - '0');
        i++;
    }
    if (count == 0) {
        return false;
    }
    if (count > 1 && s[start] == '0') {
        return false;
    }
    if (value > maxValue) {
        return false;
    }
    out = value;
    return true;
}

// Validates the token s[begin, end) against octet.octet.octet.octet[:port].
// On success fills addr (packed A*2^24 + B*2^16 + C*2^8 + D) and port
// (-1 if absent) and returns true. On any grammar violation returns false
// and leaves addr/port unspecified; the caller does not use them.
static bool parseToken(const std::string& s, size_t begin, size_t end,
                        unsigned long& addr, int& port) {
    size_t i = begin;
    long octets[4];
    for (int k = 0; k < 4; k++) {
        long value;
        if (!readNumber(s, i, end, 3, 255, value)) {
            return false;
        }
        octets[k] = value;
        if (k < 3) {
            if (i >= end || s[i] != '.') {
                return false;
            }
            i++;
        }
    }

    int parsedPort = -1;
    if (i < end) {
        if (s[i] != ':') {
            return false;
        }
        i++;
        long portValue;
        if (!readNumber(s, i, end, 5, 65535, portValue)) {
            return false;
        }
        parsedPort = static_cast<int>(portValue);
    }

    if (i != end) {
        return false;
    }

    addr = (static_cast<unsigned long>(octets[0]) << 24) |
           (static_cast<unsigned long>(octets[1]) << 16) |
           (static_cast<unsigned long>(octets[2]) << 8) |
           static_cast<unsigned long>(octets[3]);
    port = parsedPort;
    return true;
}

bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort) {
    outAddress = 0;
    outPort = -1;

    size_t n = str.size();
    size_t i = 0;
    while (i < n) {
        if (!isTokenChar(str[i])) {
            i++;
            continue;
        }
        size_t start = i;
        while (i < n && isTokenChar(str[i])) {
            i++;
        }
        // [start, i) is the maximal token run; parseToken checks it whole.
        unsigned long addr;
        int port;
        if (parseToken(str, start, i, addr, port)) {
            outAddress = addr;
            outPort = port;
            return true;
        }
        // Rejected token: i is already past its end, so scanning resumes
        // after it, never inside it.
    }
    return false;
}

#ifndef UNIT_TEST
int main() {
    std::string line;
    while (true) {
        std::cout << "Enter a string (or 'END' to quit): ";
        if (!std::getline(std::cin, line)) {
            std::cout << "Program terminated." << std::endl;
            return 0;
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line == "END") {
            std::cout << "Program terminated." << std::endl;
            return 0;
        }

        unsigned long addr;
        int port;
        if (extractIPv4(line, addr, port)) {
            unsigned long a = (addr >> 24) & 0xFF;
            unsigned long b = (addr >> 16) & 0xFF;
            unsigned long c = (addr >> 8) & 0xFF;
            unsigned long d = addr & 0xFF;
            std::cout << "Extracted IPv4 address: " << a << "." << b << "."
                       << c << "." << d << " (decimal value: " << addr
                       << ", port: ";
            if (port == -1) {
                std::cout << "none";
            } else {
                std::cout << port;
            }
            std::cout << ")" << std::endl;
        } else {
            std::cout << "Invalid input: no valid IPv4 address found" << std::endl;
        }
    }
}
#endif

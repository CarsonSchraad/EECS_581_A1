#ifndef IPV4_EXTRACT_H
#define IPV4_EXTRACT_H

#include <string>

// Scans str for the first maximal run of digits/'.'/':' that fully matches
// octet.octet.octet.octet[:port]. On success returns true and sets
// outAddress = A*2^24 + B*2^16 + C*2^8 + D and outPort (port or -1).
// On failure returns false, outAddress = 0, outPort = -1.
bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort);

#endif

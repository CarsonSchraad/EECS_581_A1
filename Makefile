CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wconversion -O0 -g

# NOTE: -fsanitize=address hangs on this machine's toolchain (Apple clang 17,
# Darwin 25.6.0) even for a trivial "hello world" binary -- see the "Known
# limitations" section of AI_DISCLOSURE.md. UBSan alone works and is what
# these targets use. If you're on a different machine/CI, add `,address`
# back to SANFLAGS to get full ASan+UBSan coverage.
SANFLAGS = -fsanitize=undefined

.PHONY: all test sample banned clean

all: ipv4

ipv4: src/ipv4_extract.cpp src/ipv4_extract.h
	$(CXX) $(CXXFLAGS) $(SANFLAGS) src/ipv4_extract.cpp -o ipv4

test_extract: src/ipv4_extract.cpp src/ipv4_extract.h tests/test_extract.cpp
	$(CXX) $(CXXFLAGS) $(SANFLAGS) -DUNIT_TEST src/ipv4_extract.cpp tests/test_extract.cpp -o test_extract

test: test_extract
	./test_extract

sample: ipv4
	./ipv4 < tests/sample_input.txt | diff - tests/sample_expected.txt && echo "sample transcript matches"

banned:
	! grep -nE 'ato[il]|strto|sto[iu]l?|scanf|inet_|regex' src/*.cpp src/*.h

clean:
	rm -f ipv4 test_extract

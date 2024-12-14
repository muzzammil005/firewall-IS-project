#include "header.h"

using namespace std;


int main() {
    rulefile();  // Load rules from "rule.txt"
    packets();   // Process packets from "sample.txt"
    compare();   // Compare packets with rules and write to "result.dat"

    return 0;
}

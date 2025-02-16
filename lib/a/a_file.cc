#include "a_file.hh"

Tester::Tester() {
    test_ = 0;
}

int& Tester::test() {
    return test_;
}

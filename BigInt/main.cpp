#include "BigInt.h"

#include <cassert>
#include <iostream>

using namespace std::experimental::seminumeric;

void testAdd()
{
    assert(integer(0) + integer(0) == integer(0));
}

int main(int argc, char **argv) {
    testAdd();
}

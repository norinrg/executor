#include "seminumeric"

#include <cassert>
#include <iostream>

using namespace std::experimental::seminumeric;

void doAssert(bool b)
{
    assert(b);
}
/*
void testAdd()
{
    doAssert(integer(  0) + integer(  0) == integer(  0));
    doAssert(integer(100) + integer(100) == integer(200));
    doAssert(integer(200) + integer(200) == integer(400));
    doAssert(integer(300) + integer(300) == integer(600));
    doAssert(integer(400) + integer(400) == integer(800));

    for (int i = 1024; i != 2048; ++i) {
        std::cerr <<  ((i % 100 == 0) ? "*" : ".");
        for (long j = 1<<28 ; j != (1<<28) + 4096; ++j) {
            doAssert(integer(i) + integer(j) == integer(i+j));
            //assert(int(i) + long(j) == long(i+j));
          }
    }

    integer one(1);
    integer minusOne(-1);
    integer zero(0);
    doAssert(one + minusOne == zero);
}
*/

void flipBit(bits& b, int bit)
{
    b[bit].flip();
    std::cerr << "b:" << b.to_string() << "\n";
}

void testBits()
{
    bits b1;
    bits b2 = 10;
    bits b3 = {30, 20, 10};
    bits b4 = {9, 8, 7, 6, 5, 4, 3, 2, 1,  0};

    std::cerr << "b1:" << b1.to_string() << "\n";
    std::cerr << "b2:" << b2.to_string() << "\n";
    std::cerr << "b3:" << b3.to_string() << "\n";
    std::cerr << "b4:" << b4.to_string() << "\n";

    std::cerr << "Flipping bits\n";

    flipBit(b1, 0);
    flipBit(b1, 1);
    flipBit(b1, 2);
    flipBit(b1, 3);
    flipBit(b1, 4);
    flipBit(b1, 5);
    flipBit(b1, 6);
    flipBit(b1, 7);
    flipBit(b1, 0);
    flipBit(b1, 1);
    flipBit(b1, 2);
    flipBit(b1, 3);
    flipBit(b1, 4);
    flipBit(b1, 5);
    flipBit(b1, 6);
    flipBit(b1, 7);
    flipBit(b1, 31);
}

int main(int argc, char **argv)
{
    //testAdd();
    testBits();
}

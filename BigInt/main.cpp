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

void bit_testConstructors()
{
    bits b1;
    assert(b1.to_string() == "00000000");

    bits b2(1);
    assert(b2.to_string() == "00000000000000000000000000000001");

    bits b3(-1);
    assert(b3.to_string() == "11111111111111111111111111111110");

    bits b4{1,  10,  1,  10};
    assert(b4.to_string() ==    "00000000000000000000000000000001"
                                "00000000000000000000000000001010"
                                "00000000000000000000000000000001"
                                "00000000000000000000000000001010");

    bits b5(b1);
    assert(b1.to_string() == "00000000");
    assert(b5.to_string() == "00000000");

    bits b6(std::move(b4));
    assert(b6.to_string() ==    "00000000000000000000000000000001"
                                "00000000000000000000000000001010"
                                "00000000000000000000000000000001"
                                "00000000000000000000000000001010");
    assert(b4.to_string() == "");
}

void bit_testAssignment()
{
    bits b1;
    bits b2;
    b1[9] = true;
    b2 = b1;

    assert(b1.to_string() ==    "00000010"
                                "00000000");
    assert(b1.to_string() == b2.to_string());

    bits b3 = -1;
    b3[9].flip();
    assert(b3.to_string() ==    "11111111"
                                "11111111"
                                "11111101"
                                "11111110");
    b3[33].flip();
    assert(b3.to_string() ==    "11111101"
                                "11111111"
                                "11111111"
                                "11111101"
                                "11111110");

    b1 = 1024;
    assert(b1.to_ulong() == 1024UL);
}

void bit_testConversion()
{
    bits b;
    assert(b.to_ulong() == 0UL);

    b = bits{1024, 10, 22, 44};
    try {
        auto ul = b.to_ulong();
        assert(false);
    }
    catch (std::range_error&) {
    }
    b = bits{1024, 10, };
    assert(b.to_ullong() == 0x000004000000000aULL);
}

void testBits()
{
    bit_testConstructors();
    bit_testAssignment();
    bit_testConversion();

    bits b1;
    bits b2a = 10;
    bits b2b = -10;
    bits b3 = {30, 20, 10};
    bits b4 = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    std::cerr << "b1:" << b1.to_string() << "\n";
    std::cerr << "b2a:" << b2a.to_string() << "\n";
    std::cerr << "b2b:" << b2b.to_string() << "\n";
    std::cerr << "b3:" << b3.to_string() << "\n";
    std::cerr << "b4:" << b4.to_string() << "\n";

    std::cerr << "Flipping bits\n";

    assert(b1.to_string()         ==  "00000000");
    assert(b1.flip(0).to_string() ==  "00000001");
    assert(b1.flip(1).to_string() ==  "00000011");
    assert(b1.flip(2).to_string() ==  "00000111");
    assert(b1.flip(3).to_string() ==  "00001111");
    assert(b1.flip(4).to_string() ==  "00011111");
    assert(b1.flip(5).to_string() ==  "00111111");
    assert(b1.flip(6).to_string() ==  "01111111");
    assert(b1.flip(7).to_string() ==  "11111111");

    assert(b1.flip(20).to_string() ==  "000100000000000011111111");
    std::cerr << "b1:" << b1.to_string() << "\n";
}

int main(int argc, char **argv)
{
    //testAdd();
    testBits();
}

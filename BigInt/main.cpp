#include "seminumeric"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

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
    assert(b2.to_string() == "00000001");

    bits b3(-1);
    assert(b3.to_string() == "11111111");

    bits b4{1,  10,  1,  10};
    assert(b4.to_string() ==    "00000001"
                                "00000000000000000000000000001010"
                                "00000000000000000000000000000001"
                                "00000000000000000000000000001010");

    bits b5(b1);
    assert(b1.to_string() == "00000000");
    assert(b5.to_string() == "00000000");

    bits b6(std::move(b4));
    assert(b6.to_string() ==    "00000001"
                                "00000000000000000000000000001010"
                                "00000000000000000000000000000001"
                                "00000000000000000000000000001010");
    // nothing to assert for b4
    // assert(b4.to_string() == "00000000");
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
    assert(b3.to_string() ==    "11111101"
                                "11111111");
    b3[33].flip();
    assert(b3.to_string() ==    "11111101"
                                "11111111"
                                "11111111"
                                "11111101"
                                "11111111");

    b1 = 1024;
    assert(b1.to_ullong() == 1024UL);
}

void bit_testConversion()
{
    bits b;
    assert(b.to_ullong() == 0UL);

    b = bits{1024, 10, 22, 44};
    try {
        auto ul = b.to_ullong();
        assert(false);
    }
    catch (std::range_error&) {
    }
    b = bits{1024, 10, };
    assert(b.to_ullong() == 0x000004000000000aULL);
}

template <typename IntType>
void assertLeftShift(const bits& b,  IntType i, size_t shift)
{
    std::string result;
    bool sign = i < 0;
    if (sign) {
        i = -i;
    }
    bool turn = false;
    while (i != 0) {
        if (i % 2 == 1) {
            char c = '1';
            if (sign && turn) {
                c = '0';
            }
            turn = true;
            result = c + result;
        } else {
            char c = '0';
            if (sign && turn) {
                c = '1';
            }
            result = c + result;
        }
        i = i / 2;
    }
    result +=  std::string(shift, '0');
    while ((result.size() % CHAR_BIT) != 0) {
        result = (sign ? '1' : '0') + result;
    }
    std::string s = b.to_string();
    assert(s == result);
}

void bit_testShiftOperators()
{
    for (int shift = 0; shift != 1240; ++shift) {
        bits b1 = char(1);
        b1 <<= shift;
        assertLeftShift(b1, 1u, shift);
    }

    for (int shift = 0; shift != 2000; ++shift) {
        auto val = 0x04030201U;
        bits b1 = val;
        b1 <<= shift;
        assertLeftShift(b1, val, shift);
    }

    for (int shift = 8; shift != 1240; ++shift) {
        bits b1 = char(-1);
        b1 <<= shift;
        assertLeftShift(b1, -1, shift);
    }

    for (int shift = 0; shift != 2000; ++shift) {
        auto val = 0x04030201U;
        bits b1 = val;
        b1 <<= shift;
        assertLeftShift(b1, val, shift);
    }
}

void bit_testOperators()
{
    bits b1 = 0xffffu;
    bits b2 = 0xff00u;
    b1 &=  b2;
    assert(b1.to_ullong() == 0xff00u);

    bits b3 = ~b1;
    assert(b3.to_ullong() == 0xffu);

    b1 |= b3;
    assert(b1.to_ullong() == 0xffffu);

    b1 ^= b3;
    assert(b1.to_ullong() == 0xff00u);

    bit_testShiftOperators();
}

void testBits()
{
    bit_testConstructors();
    bit_testAssignment();
    bit_testConversion();
    bit_testOperators();

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

namespace {

template <class Ty>
char bitChar(Ty rhs, unsigned bit, typename std::enable_if<std::is_integral<Ty>::value>::type* = nullptr) noexcept
{
    if ((rhs & (1 << bit)) == (1 << bit)) {
        return '1';
    }
    return '0';
}

template <class Ty>
std::string to_bitstring(Ty rhs, typename std::enable_if<std::is_integral<Ty>::value>::type* = nullptr) noexcept                            // integral types only
{
    std::string result;
    for (int i = 0; i != (sizeof(Ty) * CHAR_BIT); ++i) {
        result = bitChar(rhs, i) + result;
    }
    char bit = '0';
    if (rhs < 0) {
        bit= '1';
    }
    while (!result.empty() && result[0] == bit) {
        result.erase(result.begin());
    }

    while (result.size() == 0 || result.size() % CHAR_BIT != 0) {
        result = bit + result;
    }

    return std::move(result);
}

std::string make_bit_pattern_string(int size)
{
    std::string result;
    for (; size != 0; --size) {
        result += "10";
    }

    while (result.size() == 0 || result.size() %  CHAR_BIT != 0) {
        result = '0' + result;
    }
    return result;
}

void test_bitstring(std::string s)
{
    REQUIRE(bits(s).to_string() == s);
    REQUIRE(bits(s).to_string().c_str() == s);
}

bits shrink(bits&& b)
{
    b.shrink_to_fit();
    return std::move(b);
}

bits& shrink(bits& b)
{
    b.shrink_to_fit();
    return b;
}

}

TEST_CASE("Test Bits-Constructors", "[bits] [constructor]")
{
    /// default constructor
    REQUIRE(bits(     ).to_string() == to_bitstring(    0));

    // some in type constructors
    REQUIRE(shrink(bits(    0)).to_string() == to_bitstring(    0));
    REQUIRE(shrink(bits(  100)).to_string() == to_bitstring(  100));
    REQUIRE(shrink(bits(-   3)).to_string() == to_bitstring(-   3));
    REQUIRE(shrink(bits(-1024)).to_string() == to_bitstring(-1024));

    // initializer list
    REQUIRE(shrink(bits({0xff, 0xff, 0xff})).to_string() == "111111110000000000000000000000001111111100000000000000000000000011111111");

    // bitstrings
    test_bitstring("111111110000000000000000000000001111111100000000000000000000000011111111");
    for (int i = 0; i != 2000; ++i) {
        test_bitstring(make_bit_pattern_string(i));
    }

    // copy constructor
    bits b1(make_bit_pattern_string(5000));
    bits b2( b1 );
    REQUIRE(b1 == b2);

    // move constructor
    bits b3(std::move(b2));
    REQUIRE(b1 == b3);
}

TEST_CASE("Test Bits Assignment", "[bits] [assignment]")
{
    bits b1;

    b1 =    0; REQUIRE(b1 ==    0);
    b1 =  100; REQUIRE(b1 ==  100);
    b1 =   -3; REQUIRE(b1 ==   -3);
    b1 = 1024; REQUIRE(b1 == 1024);

    std::string s(make_bit_pattern_string(5000));

    bits b2(s);
    b1 = b2;            REQUIRE(b1 == bits(s));
    b2 = std::move(b2); REQUIRE(b1 == bits(s));
}

TEST_CASE("Swapping Bits swaps them", "[bits] [swap]")
{
    bits b1a(make_bit_pattern_string(5000));
    bits b2a(make_bit_pattern_string(6000));
    bits b1b(b1a);
    bits b2b(b2a);

    REQUIRE(b1a != b2a);
    REQUIRE(b2b != b1b);
    REQUIRE(b1a == b1b);
    REQUIRE(b2a == b2b);

    swap(b1a, b2a);
    REQUIRE(b1a == b2b);
    REQUIRE(b2a == b1b);
}

TEST_CASE("Converting to ulong", "[bits] [convert]")
{
    REQUIRE(bits(     ).to_ulong() ==   0UL);
    REQUIRE(bits(    0).to_ulong() ==   0UL);
    REQUIRE(bits(  100).to_ulong() == 100UL);
    REQUIRE(bits(-   3).to_ulong() ==    -3);
    REQUIRE(bits(-1024).to_ulong() == -1024);

    // initializer list
    REQUIRE(bits({0x12aabbff}).to_ulong() == 0x12aabbffUL);
    REQUIRE_THROWS(bits({0x12aabbff, 0x12aabbff, 0x12aabbff, 0x12aabbff}).to_ulong());
}

TEST_CASE("Converting to ullong", "[bits] [convert]")
{
    REQUIRE(bits(     ).to_ullong() ==   0ULL);
    REQUIRE(bits(    0).to_ullong() ==   0ULL);
    REQUIRE(bits(  100).to_ullong() == 100ULL);
    REQUIRE(shrink(bits(-   3)).to_ullong() == static_cast<unsigned char>(-3));
    REQUIRE(bits(-1024).to_ulong()  ==  -1024);

    // initializer list
    REQUIRE(bits({0x12aabbff}).to_ullong() == 0x12aabbffUL);
    REQUIRE_THROWS(bits({0x12aabbff, 0x12aabbff, 0x12aabbff, 0x12aabbff}).to_ullong());
}

TEST_CASE("Converting to string", "[bits] [convert]")
{
    REQUIRE(bits(     ).to_string() == "00000000");
    REQUIRE(shrink(bits(    0)).to_string() == "00000000");
    REQUIRE(shrink(bits(  100)).to_string() == "01100100");
    REQUIRE(shrink(bits(-   3)).to_string() == "11111101");
    REQUIRE(shrink(bits(-1024)).to_string() == "1111110000000000");
}

TEST_CASE("Operator &=", "[bits] [operator]")
{
    bits original ("10101010");
    bits base     (original);
    bits inverted ("01010101");
    bits doubled  ("1010101010101010");
    bits dInverted("0101010101010101");

    SECTION("base &= base")
    {
        REQUIRE((base &= base) == original);
    }
    SECTION("base &= inverted")
    {
        REQUIRE((base &= inverted) == 0);
    }
    SECTION("base &= doubled")
    {
        REQUIRE((base &= doubled) == original);
    }
    SECTION("base &= dInverted")
    {
        REQUIRE((base &= dInverted) == 0);
    }
}

TEST_CASE("Operator |=", "[bits] [operator]")
{
    bits original ("10101010");
    bits base     (original);
    bits inverted ("01010101");
    bits doubled  ("1010101010101010");
    bits dInverted("0101010101010101");
    bits all      ("11111111");
    bits invertedPlusAll("0101010111111111");

    SECTION("base |= base")
    {
        REQUIRE((base |= base) == original);
    }
    SECTION("base |= inverted")
    {
        REQUIRE((base |= inverted) == all);
    }
    SECTION("base |= doubled")
    {
        REQUIRE((base |= doubled) == doubled);
    }
    SECTION("base |= dInverted")
    {
        REQUIRE((base |= dInverted) == invertedPlusAll);
    }
}

TEST_CASE("Operator ^=", "[bits] [operator]")
{
    bits original ("10101010");
    bits base     (original);
    bits inverted ("01010101");
    bits doubled  ("1010101010101010");
    bits dInverted("0101010101010101");
    bits all      ("11111111");
    bits invertedPlusAll("0101010111111111");
    bits originalPlusNull("1010101000000000");

    SECTION("base ^= base")
    {
        REQUIRE((base ^= base) == 0);
    }
    SECTION("base ^= inverted")
    {
        REQUIRE((base ^= inverted) == all);
    }
    SECTION("base ^= doubled")
    {
        REQUIRE((base ^= doubled) == originalPlusNull);
    }
    SECTION("base ^= dInverted")
    {
        REQUIRE((base ^= dInverted) == invertedPlusAll);
    }
}

TEST_CASE("Operator ~", "[bits] [operator]")
{
    bits base    ("10101010");
    bits inverted("01010101");
    bits all     ("11111111");

    SECTION("~base")
    {
        REQUIRE(~base == inverted);
    }
    SECTION("~all")
    {
        REQUIRE(~all ==  0);
    }
}

TEST_CASE("Operator <<= (left shift)", "[bits] [operator] [shift]")
{
    bits b(1);

    SECTION("Left-Shifting by zero is a nop")
    {
        REQUIRE((b <<=  0) == 1);
        REQUIRE(b == 1);
    }
    SECTION("Left-Shifting by 1 doubles")
    {
        REQUIRE((b <<= 1) == 2);
        REQUIRE(b == 2);
    }
    SECTION("Left-Shifting by 10 adds 10 (binary) zeroes")
    {
        REQUIRE((b <<= 10) == bits("1" + std::string(10,  '0')));
        REQUIRE(b == bits("1" + std::string(10,  '0')));
    }
    SECTION("Left-Shifting by 10000 adds 10000 (binary) zeroes")
    {
        REQUIRE((b <<= 10000) == bits("1" + std::string(10000,  '0')));
        REQUIRE(b == bits("1" + std::string(10000,  '0')));
    }
}

TEST_CASE("Operator >>= (right shift)", "[bits] [operator] [shift]")
{
    bits b(0xffffffff);

    SECTION("Right-Shifting by zero is a nop")
    {
        REQUIRE((b >>= 0) == b);
    }
    SECTION("Right-Shifting 1 by 1 makes zero")
    {
        b = 1;
        REQUIRE((b >>= 1) == 0);
    }
    SECTION("Right-Shifting strange number by 1 makes an even stranger number")
    {
        b = bits(static_cast<unsigned short>(0x4411));
        REQUIRE((b >>= 1) == 0x2208);
    }
    SECTION("Right-Shifting by 1 makes -7f")
    {
        REQUIRE((b >>= 1) == 0x7fffffff);
    }
    SECTION("Right-Shifting by 2 makes -3f")
    {
        REQUIRE((b >>= 2) == 0x3fffffff);
    }
    SECTION("Right-Shifting by 3 makes -1f")
    {
        REQUIRE((b >>= 3) == 0x1fffffff);
    }
    SECTION("Right-Shifting by 4 makes -0f")
    {
        REQUIRE((b >>= 4) == 0x0fffffff);
    }
    SECTION("Right-Shifting by 5 makes -07")
    {
        REQUIRE((b >>= 5) == 0x07ffffff);
    }
    SECTION("Right-Shifting by 6 makes -03")
    {
        REQUIRE((b >>= 6) == 0x03ffffff);
    }
    SECTION("Right-Shifting by 7 makes -01")
    {
        REQUIRE((b >>= 7) == 0x01ffffff);
    }
    SECTION("Right-Shifting by 8 makes -00")
    {
        REQUIRE((b >>= 8) == 0x00ffffff);
    }
    SECTION("Right-Shifting by 9 makes -007")
    {
        REQUIRE((b >>= 9) == 0x007fffff);
    }
    SECTION("Right-Shifting by 10 shifts by 10")
    {
        b = bits{0xffffffffU, 0xffffffffU, 0xffffffffU, 0xffffffffU};
        REQUIRE((b >>= 10) == bits({0x003fffffU, 0xffffffffU, 0xffffffffU, 0xffffffffU}));
    }
    SECTION("Right-Shifting by 10 shifts by 10")
    {
        b = bits{"1101010100011111010010001111110010010101010001011110"
                 "0010100100010011111100001010010010010010010111110001"};
        REQUIRE((b >>= 34) == bits("0000000000000000000000000000000000110101010001111101"
                                   "0010001111110010010101010001011110001010010001001111"));
    }
    SECTION("Right-Shifting by exactly available bits makes 0")
    {
        REQUIRE((b >>= 32) == 0);
    }
    SECTION("Right-Shifting by more than available bits makes 0")
    {
        REQUIRE((b >>= 33) == 0);
    }
}

TEST_CASE("set", "[bits] [modifier] [set]")
{
    bits b0(0x0);
    bits bf(0xff);

    b0 = shrink(b0);

    SECTION("Set sets every bit in the source")
    {
        REQUIRE(b0.set() == bits(0xffU));
    }
    SECTION("Set sets every bit even in longer sources")
    {
        bf = bits{0x10101010, 0x10101010, 0x10101010, 0x10101010, 0x10101010,
                 0x10101010, 0x10101010, 0x10101010, 0x10101010, 0x10101010};
        REQUIRE(bf.set() == bits({0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                                  0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}));
    }
    SECTION("Set(pos) sets the given bit")
    {
        REQUIRE(b0.set(0) == 1);
    }
    SECTION("Set(pos, true) sets the given bit")
    {
        REQUIRE(b0.set(1, true) == 0x02);
    }
    SECTION("Set(pos, false) resets the given bit")
    {
        REQUIRE(bf.set(3, false) == 0xf7);
    }
    SECTION("Set(pos) can be used to set/reset only virtually existing bits")
    {
        REQUIRE(b0.set(30) == 0x40000000);
    }
}

TEST_CASE("reset", "[bits] [modifier] [reset]")
{
    bits b0(0x0);
    bits bf(0xff);

    b0 = shrink(b0);
    bf = shrink(bf);

    SECTION("Reset() sets every bit in the sequence to 0")
    {
        REQUIRE(bf.reset() == 0);
    }
    SECTION("Reset resets every bit even in longer sources")
    {
        bf = bits{0x10101010, 0x10101010, 0x10101010, 0x10101010, 0x10101010,
            0x10101010, 0x10101010, 0x10101010, 0x10101010, 0x10101010};
        REQUIRE(bf.reset() == 0);
    }
    SECTION("Reset(pos) resets the given bit")
    {
        REQUIRE(bf.reset(0) == 0xfe);
    }
    SECTION("Reset(pos) can be used to set only virtually existing bits")
    {
        //  i.e. nothing changed
        REQUIRE(bf.reset(30) == 0xff);
    }
    SECTION("Flip() toggles every bit in the sequence")
    {
        REQUIRE(b0.flip() == 0xff);
        REQUIRE(bf.flip() == 0x00);
    }
}

TEST_CASE("flip", "[bits] [modifier] [flip]")
{
    bits b0(0x0);
    bits bf(0xff);

    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(0) == 0xfe);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(1) == 0b11111101);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(2) == 0b11111011);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(3) == 0b11110111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(4) == 0b11101111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(5) == 0b11011111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(6) == 0b10111111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(7) == 0b01111111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(8) == 0b000111111111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(9) == 0b001011111111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(10) == 0b010011111111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(11) == 0b100011111111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(12) == 0b0001000011111111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(13) == 0b0010000011111111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(14) == 0b0100000011111111);
    }
    SECTION("flip(pos) toggles the given bit")
    {
        REQUIRE(bf.flip(15) == 0b1000000011111111);
    }
}

TEST_CASE("operator[]", "[bits] [modifier] [operator]")
{
    const bits constBits(0b010101010101010101010101);
    bits nonconstBits(constBits);

    SECTION("operator[] gives access to the bits")
    {
        REQUIRE(constBits[0]);
        REQUIRE(nonconstBits[0]);
    }
}

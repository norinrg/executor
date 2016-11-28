/*
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

// from https://isocpp.org/files/papers/n4038.html

#include <algorithm>
#include <climits>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace std { namespace experimental { namespace seminumeric {

// constructors
inline bits::bits() noexcept
    : bits((byte)(0))
{}

template <class Ty>
inline bits::bits(Ty rhs, typename enable_if<is_integral<Ty>::value>::type*) noexcept
    : data_{initVector(rhs)}
{
    data_.shrink();
}

bits::bits(std::initializer_list<uint_least32_t> list)
    : data_{initVector(move(list))}
{
    data_.shrink();
}

template <class CharT, class Traits, class Alloc>
bits::bits(const basic_string<CharT, Traits, Alloc>& str,
    typename basic_string<CharT, Traits, Alloc>::size_type pos,
    typename basic_string<CharT, Traits, Alloc>::size_type count,
    CharT zero,
    CharT one)
    : data_{false}
{
    if (count == std::basic_string<CharT>::npos) {
        count = str.size() - pos;
    }
    data_ = makeVector(&*str.begin()+pos, count, zero, one);
    data_.shrink();
}

template <class CharT>
bits::bits(const CharT* ptr,
    typename basic_string<CharT>::size_type count,
    CharT zero,
    CharT one)
{
    if (count == std::basic_string<CharT>::npos) {
        count = strlen(ptr);
    }
    data_ = makeVector(ptr, count, zero, one);
    data_.shrink();
}

/*
explicit bits::bits(const integer& val);
explicit bits::bits(integer&& val);
*/

bits::bits(const bits& rhs)
    : data_(rhs.data_)
{}

bits::bits(bits&& rhs) noexcept
    : data_(move(rhs.data_))
{}

// assign and swap
template <class Ty>
bits& bits::operator=(Ty rhs) // integral types only
{
    data_ = initVector(rhs);

    data_.shrink();
    return *this;
}

/*
bits& bits::operator=(const integer& rhs);
bits& bits::operator=(integer&& rhs);
*/

bits& bits::operator=(const bits& rhs)
{
    auto that(rhs);
    swap(that);
    return *this;
}

bits& bits::operator=(bits&& rhs)
{
    swap(rhs);
    return *this;
}

void bits::swap(bits& rhs) noexcept
{
    data_.data.swap(rhs.data_.data);
    std::swap(data_.complement, rhs.data_.complement);
}

// conversions
unsigned long bits::to_ulong() const
{
    if (data_.data.size() > sizeof (unsigned long)) {
        throw range_error("out of range");
    }
    unsigned long result = 0UL;
    size_t shift = 0;
    for (auto c : data_.data) {
        unsigned long b = c;
        result += b << shift;
        shift += CHAR_BIT;
    }

    return result;
}

unsigned long long bits::to_ullong() const
{
    if (data_.data.size() > sizeof (unsigned long long)) {
        throw range_error("out of range");
    }
    unsigned long long result = 0ULL;
    size_t shift = 0;
    for (auto c : data_.data) {
        unsigned long long b = c;
        result += b << shift;
        shift += CHAR_BIT;
    }

    return result;
}

template <class CharT, class Traits, class Alloc>
std::basic_string<CharT, Traits, Alloc> bits::to_string(CharT zero, CharT one) const
{
    size_t pos = data_.data.size();
    std::basic_string<CharT, Traits, Alloc> result(pos * CHAR_BIT, zero);

    for (auto c : data_.data) {
        for (int i = 0; i != CHAR_BIT; ++i) {
            if (c & (1 << i)) {
                result[pos*CHAR_BIT-(i+1)] = one;
            }
        }

        --pos;
    }

    if (data_.data.empty()) {
        result = std::string(CHAR_BIT, data_.complement ? one : zero);
    }

    return result;
}

// logical operations
bits& bits::operator&=(const bits& rhs)
{
    return operate(rhs, bit_and<byte>());
}

bits& bits::operator|=(const bits& rhs)
{
    return operate(rhs, bit_or<byte>());
}

bits& bits::operator^=(const bits& rhs)
{
    return operate(rhs, bit_xor<byte>());
}

bits bits::operator~() const
{
    bits result(*this);
    transform(begin(result.data_.data), end(result.data_.data), begin(result.data_.data),
        [](auto b1) {
            return ~b1;
        }
    );
    result.data_.shrink();

    return result;
}

bits& bits::operator<<=(size_t rhs)
{
    auto byteShift = (rhs+CHAR_BIT-1)/CHAR_BIT;
    auto bitShift  = rhs%CHAR_BIT;
    auto growSize  = byteShift;

    grow(data_, max(data_.data.size()+growSize, size_t(2)));

    auto b1Origin = rbegin(data_.data)+byteShift;
    auto e1Origin = rend(data_.data);

    auto b2Origin = b1Origin-1;
    auto bDest = rbegin(data_.data);

    transform(b1Origin, e1Origin, b2Origin, bDest,
        [bitShift](auto b1, auto b2) {
            if (bitShift == 0) {
                return b1;
            }
            byte result = (b1 >> ((CHAR_BIT - bitShift)%CHAR_BIT)) | (b2 << bitShift);
            return result;
        }
    );
    if (bitShift != 0) {
        --byteShift;
    }

    data_.data[byteShift] = data_.data[0] << bitShift;
    fill(begin(data_.data), begin(data_.data)+byteShift, 0);
    data_.shrink();

    return *this;
}


bits& bits::operator>>=(size_t rhs)
{
    data_ >>= rhs;
    return *this;
}

/*
bits& bits::operator<<(size_t rhs) const;
bits& bits::operator>>(size_t rhs) const;

// element access and modification
bits& bits::set() noexcept;
bits& bits::set(size_t pos, bool val = true);
bits& bits::reset() noexcept;
bits& bits::reset(size_t pos);
bits& bits::flip() noexcept;
*/

bits& bits::flip(size_t pos)
{
    reference ref = make_existing_reference(data_, pos);
    ref.flip();
    return *this;
}

/*bool bits::operator[](size_t pos) const;
*/

bits::reference bits::operator[](size_t pos)
{
    return make_existing_reference(data_, pos);
}

/*
bool bits::test(size_t pos) const noexcept;
bool bits::all() const noexcept;
bool bits::any() const noexcept;
bool bits::none() const noexcept;
size_t bits::count() const noexcept;
size_t bits::count_not_set() const noexcept;
*/
// comparison
bool bits::operator==(const bits& rhs) const noexcept
{
    return data_ == rhs.data_;
}

bool bits::operator!=(const bits& rhs) const noexcept
{
    return !operator==(rhs);
}

/*
// capacity
size_t bits::size() const noexcept;
size_t bits::capacity() const noexcept;
void bits::reserve(size_t bit_count);
void bits::shrink_to_fit();
*/

// class bits::reference
bits::reference& bits::reference::operator=(bool val) noexcept
{
    uc ^= (-int(val) ^ uc) & (1 << bit);
    return *this;
}

bits::reference& bits::reference::operator=(const reference& rhs) noexcept
{
    (*this) = (bool)rhs;
    return *this;
}

bool bits::reference::operator~() const noexcept
{
    return (bool)(*this);
}

bits::reference::operator bool() const noexcept
{
    return (uc >> bit) & 1;
}

bits::reference& bits::reference::flip() noexcept
{
    uc ^= 1 << bit;
    return *this;
}

bits::reference::reference(byte& uc, int bit)
    : uc(uc)
    , bit(bit)
{}

template <typename Ty>
inline void bits::addValue(Data& data, Ty rhs)
{
    auto size = sizeof rhs;
    auto b = static_cast<byte*>(static_cast<void*>(&rhs));
    auto e = b + size;

    // little endian
    copy(b, e, back_inserter(data.data));
}

template <typename CharT>
inline bits::Data bits::makeVector(const CharT* str, size_t count, CharT zero, CharT one)
{
    Data result{false};
    if (count == 0) {
        return move(result);
    }
    do {
        reference ref(make_existing_reference(result, --count));
        if (*str != zero && *str != one) {
            // throw?
        }
        ref = *str == one;
        ++str;
    } while (count != 0);

    return move(result);
}

template <typename Ty>
inline bits::Data bits::initVector(Ty rhs, typename enable_if<is_integral<Ty>::value>::type*)
{
    Data result{rhs < 0};

    result.data.reserve(sizeof rhs);
    addValue(result, rhs);

    return move(result);
}

inline bits::Data bits::initVector(std::initializer_list<uint_least32_t> list)
{
    Data result{false};

    result.data.reserve(list.size() * sizeof(uint_least32_t));
    for_each(rbegin(list), rend(list), [&result](auto val) {
        addValue(result, val);
    });

    return move(result);
}

inline void bits::grow(Data& data, size_t size)
{
    size_t oldSize = data.data.size();
    if (oldSize < size) {
        data.data.insert(end(data.data), size-oldSize, data.highByte());
    }
}

inline bits::reference bits::make_existing_reference(Data& data, size_t pos)
{
    auto byteNr = pos / CHAR_BIT;
    grow(data, byteNr+1);
    return reference(data.data[byteNr], pos % CHAR_BIT);
}

template <class Op>
inline bits& bits::operate(const bits& rhs, Op op)
{
    grow(data_, rhs.data_.data.size());
    transform(begin(data_.data), begin(data_.data)+data_.data.size(), begin(rhs.data_.data), begin(data_.data),
        [op](auto b1, auto b2) {
            return op(b1, b2);
        }
    );

    byte highByte = rhs.data_.highByte();
    transform(begin(data_.data)+data_.data.size(), end(data_.data), begin(data_.data)+data_.data.size(),
        [op, highByte](auto b) {
            return op(b, highByte);
        }
    );
    data_.shrink();

    return *this;
}

inline void bits::Data::shrink()
{
    byte sign = highByte();
    while (!data.empty()) {
        if (data.back() != sign) {
            break;
        }
        data.pop_back();
    }
    if (data.empty()) {
        data.push_back(sign);
    }
    if (complement && (data.back() >> (CHAR_BIT-1)) != sign >> (CHAR_BIT-1)) {
        data.push_back(sign);
    }
}

inline bits::byte bits::Data::highByte() const
{
    return complement ? -1 : 0;
}

inline bool bits::Data::operator==(const Data& rhs) const
{
    return complement == rhs.complement
        && data == rhs.data;
}

inline void bits::Data::operator>>=(size_t rhs)
{
    size_t bitShift = rhs % CHAR_BIT;
    size_t byteShift = rhs / CHAR_BIT;

    if (byteShift < data.size()) {
        auto b1Origin = begin(data)+byteShift;
        auto e1Origin = end(data)-1;

        auto b2Origin = b1Origin+1;
        auto bDest = begin(data);

        transform(b1Origin, e1Origin, b2Origin, bDest,
                [bitShift](auto b1, auto b2) {
                    if (bitShift == 0) {
                        return b1;
                    }
                    byte result = (b1 >> bitShift) | (b2 << (CHAR_BIT - bitShift));
                    return result;
                }
        );
        *(bDest+(e1Origin-b1Origin)) = (*e1Origin >> bitShift);
        fill(end(data)-byteShift, end(data), highByte());
    } else {
        data = {highByte()};
    }
    shrink();
}

/*
    auto byteShift = (rhs+CHAR_BIT-1)/CHAR_BIT;
    auto bitShift  = rhs%CHAR_BIT;
    auto growSize  = byteShift;

    grow(data_, max(data_.data.size()+growSize, size_t(2)));

    auto b1Origin = rbegin(data_.data)+byteShift;
    auto e1Origin = rend(data_.data);

    auto b2Origin = b1Origin-1;
    auto bDest = rbegin(data_.data);

    transform(b1Origin, e1Origin, b2Origin, bDest,
        [bitShift](auto b1, auto b2) {
            if (bitShift == 0) {
                return b1;
            }
            byte result = (b1 >> ((CHAR_BIT - bitShift)%CHAR_BIT)) | (b2 << bitShift);
            return result;
        }
    );
    if (bitShift != 0) {
        --byteShift;
    }

    data_.data[byteShift] = data_.data[0] << bitShift;
    fill(begin(data_.data), begin(data_.data)+byteShift, 0);
    data_.shrink();

    return *this;
*/

}}}

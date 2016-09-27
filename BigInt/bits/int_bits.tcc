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

namespace std { namespace experimental { namespace seminumeric {
/*
static void grow(vector<byte>& data, size_t size)
{
    size_t oldSize = data.size();
    for (; oldSize < size; ++oldSize) {
        data.push_back(-1);
    }
}
*/
// constructors
inline bits::bits() noexcept
    : bits((byte)(0))
{}

template <class Ty>
inline bits::bits(Ty rhs) noexcept
    : is_negative_(rhs < 0)
    , data_(initVector(rhs))
{}

bits::bits(std::initializer_list<uint_least32_t> list)
    : is_negative_(false)
    , data_(initVector(move(list)))
{}

/*
template <class CharT, class Traits, class Alloc>
explicit bits::bits(const basic_string<CharT, Traits, Alloc>& str,
    typename basic_string<CharT, Traits, Alloc>::size_t pos = 0,
    typename basic_string<CharT, Traits, Alloc>::size_t count = std::basic_string<CharT>::npos,
    CharT zero = CharT('0'),
    CharT one = CharT('1'))
{}

template <class CharT>
explicit bits::bits(const CharT *ptr,
    typename basic_string<CharT>::size_t count = std::basic_string<CharT>::npos,
    CharT zero = CharT('0'),
    CharT one = CharT('1'));

explicit bits::bits(const integer& val);
explicit bits::bits(integer&& val);
*/

bits::bits(const bits& rhs)
    : is_negative_(rhs.is_negative_)
    , data_(rhs.data_)
{}

bits::bits(bits&& rhs) noexcept
    : is_negative_(rhs.is_negative_)
    , data_(move(rhs.data_))
{}

// assign and swap
template <class Ty>
bits& bits::operator=(Ty rhs) // integral types only
{
    is_negative_ = rhs < 0;
    data_ = initVector(rhs);

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
    data_.swap(rhs.data_);
    std::swap(is_negative_, rhs.is_negative_);
}

/*
// conversions
unsigned long bits::to_ulong() const;
unsigned long long bits::to_ullong() const;
*/

template <class CharT, class Traits, class Alloc>
std::basic_string<CharT, Traits, Alloc> bits::to_string(CharT zero, CharT one) const
{
    size_t pos = data_.size();
    std::basic_string<CharT, Traits, Alloc> result(pos * CHAR_BIT, zero);

    for (auto c : data_) {
        for (int i = 0; i != CHAR_BIT; ++i) {
            if (c & (1 << i)) {
                result[pos*CHAR_BIT-(i+1)] = one;
            }
        }
        --pos;
    }

    return result;
}

/*
// logical operations
bits& bits::operator&=(const bits& rhs);
bits& bits::operator|=(const bits& rhs);
bits& bits::operator^=(const bits& rhs);
bits bits::operator~() const;

bits& bits::operator<<=(size_t rhs);
bits& bits::operator>>=(size_t rhs);
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
    reference ref = make_existing_reference(pos);
    ref.flip();
    return *this;
}

/*bool bits::operator[](size_t pos) const;
*/

bits::reference bits::operator[](size_t pos)
{
    return make_existing_reference(pos);
}

/*
bool bits::test(size_t pos) const noexcept;
bool bits::all() const noexcept;
bool bits::any() const noexcept;
bool bits::none() const noexcept;
size_t bits::count() const noexcept;
size_t bits::count_not_set() const noexcept;

// comparison
bool bits::operator==(const bits& rhs) const noexcept;
bool bits::operator!=(const bits& rhs) const noexcept;

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
inline void bits::addValue(vector<byte>& data, Ty rhs)
{
    if (rhs < 0) {
        rhs = ~(-rhs);
    }
    auto size = sizeof rhs;
    auto b = static_cast<byte*>(static_cast<void*>(&rhs));
    auto e = b + size;

    // little endian
    copy(b, e, back_inserter(data));
}

template <typename Ty>
inline vector<bits::byte> bits::initVector(Ty rhs, typename enable_if<is_integral<Ty>::value>::type*)
{
    vector<byte> result;

    result.reserve(sizeof rhs);
    addValue(result, rhs);

    return move(result);
}

inline vector<bits::byte> bits::initVector(std::initializer_list<uint_least32_t> list)
{
    vector<byte> result;

    result.reserve(list.size() * sizeof(uint_least32_t));
    for_each(rbegin(list), rend(list), [&result](auto val) {
        addValue(result, val);
    });

    return move(result);
}

inline void bits::grow(size_t size)
{
    size_t oldSize = data_.size();
    for (; oldSize < size; ++oldSize) {
        data_.push_back(is_negative_ ? -1 : 0);
    }
}

inline bits::reference bits::make_existing_reference(size_t pos)
{
    auto byteNr = pos / CHAR_BIT;
    grow(byteNr+1);
    return reference(data_[byteNr], pos % CHAR_BIT);
}

}}}

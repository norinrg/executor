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

namespace std { namespace experimental { namespace seminumeric {

unsigned char onesComplement(unsigned char c)
{
    return ~c;
}

template <typename Ty>
void addValue(vector<unsigned char>& data, Ty rhs)
{
    auto size = sizeof rhs;
    using uchar = unsigned char;
    uchar* b = static_cast<uchar*>(static_cast<void*>(&rhs));
    uchar* e = b + size;

    // little endian
    for (; b != e; ++b) {
        data.push_back(onesComplement(*b));
    }
}

template <typename Ty>
static vector<unsigned char> initVector(Ty rhs, typename enable_if<is_integral<Ty>::value>::type* = 0)
{
    vector<unsigned char> result;

    result.reserve(sizeof rhs);
    addValue(result, rhs);

    return result;
}

static vector<unsigned char> initVector(std::initializer_list<uint_least32_t> list)
{
    vector<unsigned char> result;

    result.reserve(list.size() * sizeof(uint_least32_t));
    for_each(rbegin(list), rend(list), [&result](auto val) {
        addValue(result, val);
    });

    return result;
}

// constructors
inline bits::bits() noexcept
    : bits((unsigned char)(0))
{}

template <class Ty>
inline bits::bits(Ty rhs) noexcept
    : data_(initVector(rhs))
{}

bits::bits(std::initializer_list<uint_least32_t> list)
    : data_(initVector(move(list)))
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
    return *this;
}

/*
bits& bits::operator=(const integer& rhs);
bits& bits::operator=(integer&& rhs);
*/

bits& bits::operator=(const bits& rhs)
{
    auto data = rhs.data_;
    data.swap(data_);
    return *this;
}

bits& bits::operator=(bits&& rhs)
{
    data_.swap(rhs.data_);
    return *this;
}

void bits::swap(bits& rhs) noexcept
{
    data_.swap(rhs.data_);
}

/*
// conversions
unsigned long bits::to_ulong() const;
unsigned long long bits::to_ullong() const;
template <class CharT = char, class Traits = std::char_traits<CharT>, class Alloc = std::allocator<CharT> >
    std::basic_string<CharT, Traits, Alloc> bits::to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const;

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
bits& bits::flip(size_t pos);
bool bits::operator[](size_t pos) const;
reference bits::operator[](size_t pos);
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

// class bits::reference
bits::reference& bits::reference::operator=(bool val) noexcept;
bits::reference& bits::reference::operator=(const reference& rhs) noexcept;
bool bits::reference::operator~() const noexcept;
operator bool() const noexcept;
bits::reference& bits::reference::flip() noexcept;
*/
}}}

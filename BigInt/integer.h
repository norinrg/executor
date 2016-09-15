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

#ifndef BIGINT_H
#define BIGINT_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace std { namespace experimental { namespace seminumeric {


/* class integer */

class integer;
class integer_data_proxy;

void swap(integer& lhs, integer& rhs) noexcept;

// comparisons
bool operator==(const integer& lhs, const integer& rhs) noexcept;
bool operator!=(const integer& lhs, const integer& rhs) noexcept;
bool operator<(const integer& lhs, const integer& rhs) noexcept;
bool operator<=(const integer& lhs, const integer& rhs) noexcept;
bool operator>(const integer& lhs, const integer& rhs) noexcept;
bool operator>=(const integer& lhs, const integer& rhs) noexcept;

// arithmetic operations
integer operator+(const integer& lhs, const integer& rhs); //!
integer operator-(const integer& lhs, const integer& rhs); //!
integer operator*(const integer& lhs, const integer& rhs); //!
integer operator/(const integer& lhs, const integer& rhs); //!
integer operator%(const integer& lhs, const integer& rhs); //!

std::pair<integer, integer> div(const integer& lhs, const integer& rhs);

integer abs(const integer& val);

integer operator<<(const integer& lhs, size_t rhs);
integer operator>>(const integer& lhs, size_t rhs);

// numeric operations
integer sqr(const integer& val);
integer sqrt(const integer& val);
integer pow(const integer& val, const integer& exp);
integer mod(const integer& lhs, const integer& rhs);
integer mulmod(const integer& lhs, const integer& rhs, const integer& m);
integer powmod(const integer& lhs, const integer& rhs, const integer& m);

integer gcd(const integer& a, const integer& b);
integer lcm(const integer& a, const integer& b);


// conversions
std::string to_string(const integer& val, int radix = 10);

// I/O operations
template <class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(
    std::basic_ostream<CharT, Traits>& str, const integer& val);
template <class CharT, class Traits>
std::basic_istream<CharT, Traits>& operator>>(
    std::basic_istream<CharT, Traits>& str, integer& val);

/* class bits */
class bits;

void swap(bits& lhs, bits& rhs) noexcept;

// logical operations
bits operator&(const bits& lhs, const bits& rhs);
bits operator|(const bits& lhs, const bits& rhs);
bits operator^(const bits& lhs, const bits& rhs);

// I/O operations
template <class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(
std::basic_ostream<CharT, Traits>& str, const bits& val);
template <class CharT, class Traits>
std::basic_istream<CharT, Traits>& operator>>(
std::basic_istream<CharT, Traits>& str, bits& val);

class integer_data_proxy {

    friend class integer;
    using buffer_type = vector<unsigned char>;

public:
    // type names
    typedef buffer_type::value_type data_type;                          // char
    typedef short arithmetic_type;                    // short
    typedef unsigned short uarithmetic_type;
    typedef buffer_type::iterator iterator;
    typedef buffer_type::const_iterator const_iterator;
    typedef buffer_type::reverse_iterator reverse_iterator;
    typedef buffer_type::const_reverse_iterator const_reverse_iterator;

    // constructors
    integer_data_proxy(const integer_data_proxy& rhs) = delete;
    integer_data_proxy(integer_data_proxy&& rhs);

    // assign
    integer_data_proxy& operator=(const integer_data_proxy& rhs) = delete;
    integer_data_proxy& operator=(integer_data_proxy&& rhs) = delete;

    // iterators
    iterator begin() noexcept;
    iterator end() noexcept;
    reverse_iterator rbegin() noexcept;
    reverse_iterator rend() noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;

    // element access
    data_type operator[](size_t pos) const;
    data_type& operator[](size_t pos);

    // capacity
    size_t size() const noexcept;
    size_t capacity() const noexcept;
    void reserve(size_t digits);
    void shrink_to_fit();

private:
    static constexpr uarithmetic_type base = uarithmetic_type(data_type(-1)) + 1;

    template <class Ty>
    integer_data_proxy(Ty rhs) noexcept
      : neg_(rhs < 0)
      , default_(neg_ ? data_type(-1) : data_type(0))
    {
        data_type* data = static_cast<data_type*>(&rhs);
        data_.reserve(sizeof(Ty) / sizeof(data_type));
        for (int i = 0; i != sizeof(rhs); ++i) {
            data_.push_back(rhs % base);
        }
        bool localNeg = neg_;
        if (localNeg) {
            rhs = -rhs;
        }
        data_.reserve(sizeof(Ty) / sizeof(data_type));
        while (rhs != 0) {
            data_.push_back(rhs % base);
            rhs /=  base;
        }
        if (localNeg) {
            negate();
            neg_ = true;
            default_ = data_type(-1);
        }
        normalize();
    }

    integer_data_proxy(const integer_data_proxy& rhs, int len)
      : neg_(rhs.neg_)
      , data_(rhs.data_)
      , default_(rhs.default_)
    {
    }

    // arithmetic operations
    integer_data_proxy& operator+=(const integer_data_proxy& rhs);
    integer_data_proxy& operator-=(const integer_data_proxy& rhs);
    integer_data_proxy& operator*=(const integer_data_proxy& rhs);
    integer_data_proxy& operator/=(const integer_data_proxy& rhs);
    integer_data_proxy& operator%=(const integer_data_proxy& rhs);

    int compare(const integer_data_proxy& rhs) const noexcept;
    integer_data_proxy& negate() noexcept;

    void enlarge(int len);
    void normalize();

private:
    bool neg_;
    buffer_type data_;
    data_type default_;
};

class integer {
public:

    // constructors
    integer() noexcept : data_(0)
    {}

    template <class Ty>
    integer(Ty rhs) noexcept : data_(rhs)
    {}

    integer(std::initializer_list<uint_least32_t> init);

    template <class CharT, class Traits, class Alloc>
    explicit integer(const std::basic_string<CharT, Traits, Alloc>& str);

    explicit integer(const bits& rhs);
    explicit integer(bits&& rhs);

    integer(const integer& rhs) : data_(rhs.data_, int())
    {}

    integer(integer&& rhs) noexcept : data_(move(rhs.data_))
    {}

    // assign and swap
    template <class Ty>
    integer& operator=(Ty rhs);                         // arithmetic types only
    integer& operator=(const bits& rhs);
    integer& operator=(bits&& rhs);
    integer& operator=(const integer& rhs);
    integer& operator=(integer&& rhs);
    void swap(integer& rhs) noexcept;

    // conversions
    explicit operator long long() const;
    explicit operator unsigned long long() const;
    explicit operator long double() const noexcept;
    explicit operator bool() const noexcept;

    // comparisons
    int compare(const integer& rhs) const noexcept
    {
        return data_.compare(rhs.data_);
    }

    // arithmetic operations
    integer& operator+=(const integer& rhs);

    integer& operator-=(const integer& rhs);
    integer& operator*=(const integer& rhs);
    integer& operator/=(const integer& rhs);
    integer& operator%=(const integer& rhs);

    integer& operator++();
    integer operator++(int);
    integer& operator--();
    integer operator--(int);

    integer div(const integer& rhs);

    integer& abs() noexcept;
    integer& negate() noexcept;
    integer operator+() const noexcept;
    integer operator-() const noexcept;

    integer& operator<<=(size_t rhs);
    integer& operator>>=(size_t rhs);

    // numeric operations
    integer& sqr();
    integer& sqrt();
    integer& pow(const integer& exp);
    integer& mod(const integer& rhs);
    integer& mulmod(const integer& rhs, const integer& m);
    integer& powmod(const integer& exp, const integer& m);

    // observers
    bool is_zero() const noexcept;
    bool is_odd() const noexcept;

    // accessors
    integer_data_proxy get_data_proxy();

    // capacity
    size_t size() const noexcept;
    size_t capacity() const noexcept;
    void reserve(size_t digits);
    void shrink_to_fit();

private:
    integer_data_proxy data_;
};

class bits {
public:

    class reference;

    // constructors
    bits() noexcept;

    template <class Ty>
    bits(Ty rhs) noexcept;                              // integral types only

    bits(std::initializer_list<uint_least32_t> list);

    template <class CharT, class Traits, class Alloc>
    explicit bits(const basic_string<CharT, Traits, Alloc>& str,
        typename basic_string<CharT, Traits, Alloc>::size_t pos = 0,
        typename basic_string<CharT, Traits, Alloc>::size_t count = std::basic_string<CharT>::npos,
        CharT zero = CharT('0'),
        CharT one = CharT('1'));
    template <class CharT>
    explicit bits(const CharT *ptr,
        typename basic_string<CharT>::size_t count = std::basic_string<CharT>::npos,
        CharT zero = CharT('0'),
        CharT one = CharT('1'));

    explicit bits(const integer& val);
    explicit bits(integer&& val);

    bits(const bits& rhs);
    bits(bits&& rhs) noexcept;

    // assign and swap
    template <class Ty>
    bits& operator=(Ty rhs);                            // integral types only
    bits& operator=(const integer& rhs);
    bits& operator=(integer&& rhs);
    bits& operator=(const bits& rhs);
    bits& operator=(bits&& rhs);
    void swap(bits& rhs) noexcept;

    // conversions
    unsigned long to_ulong() const;
    unsigned long long to_ullong() const;
    template <class CharT = char, class Traits = std::char_traits<CharT>, class Alloc = std::allocator<CharT> >
    std::basic_string<CharT, Traits, Alloc> to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const;

    // logical operations
    bits& operator&=(const bits& rhs);
    bits& operator|=(const bits& rhs);
    bits& operator^=(const bits& rhs);
    bits operator~() const;

    bits& operator<<=(size_t rhs);
    bits& operator>>=(size_t rhs);
    bits& operator<<(size_t rhs) const;
    bits& operator>>(size_t rhs) const;

    // element access and modification
    bits& set() noexcept;
    bits& set(size_t pos, bool val = true);
    bits& reset() noexcept;
    bits& reset(size_t pos);
    bits& flip() noexcept;
    bits& flip(size_t pos);
    bool operator[](size_t pos) const;
    reference operator[](size_t pos);
    bool test(size_t pos) const noexcept;
    bool all() const noexcept;
    bool any() const noexcept;
    bool none() const noexcept;
    size_t count() const noexcept;
    size_t count_not_set() const noexcept;

    // comparison
    bool operator==(const bits& rhs) const noexcept;
    bool operator!=(const bits& rhs) const noexcept;

    // capacity
    size_t size() const noexcept;
    size_t capacity() const noexcept;
    void reserve(size_t bit_count);
    void shrink_to_fit();
};

} /* namespace seminumeric */
} /* namespace experimental */

template <class Ty> class numeric_limits;
template <> class numeric_limits<experimental::seminumeric::integer>;

template <class Ty> class hash;
template <> class hash<experimental::seminumeric::integer>;
template <> class hash<experimental::seminumeric::bits>;

}

#endif

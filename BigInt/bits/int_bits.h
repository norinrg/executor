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

#ifndef INT_BITS_H
#define INT_BITS_H

namespace std { namespace experimental { namespace seminumeric {

class bits {
public:

    class reference;

    // constructors
    bits() noexcept;

    template <class Ty>
    bits(Ty rhs) noexcept;                            // integral types only

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
    bits& operator=(Ty rhs); // integral types only
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

    class reference {
    public:
        reference& operator=(bool val) noexcept;
        reference& operator=(const reference& rhs) noexcept;
        bool operator~() const noexcept;
        operator bool() const noexcept;
        reference& flip() noexcept;

    // new
    private:
        friend class bits;

        reference(unsigned char& uc,  int bit);

    private:
        unsigned char& uc;
        int bit;
    };

//  new
private:
    using byte = unsigned char;

    template <typename Ty>
    static void addValue(vector<byte>& data, Ty rhs);

    template <typename Ty>
    static vector<byte> initVector(Ty rhs, typename enable_if<is_integral<Ty>::value>::type* = 0);
    static vector<byte> initVector(std::initializer_list<uint_least32_t> list);

    void grow(size_t size);
    reference make_existing_reference(size_t pos);

    void shrink();

private:
    bool is_negative_ = false;
    vector<byte> data_;
};

}}}

#include "int_bits.tcc"

#endif

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

#include "BigInt.h"

namespace std { namespace experimental { namespace seminumeric {

bool operator==(const integer& lhs, const integer& rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

bool operator!=(const integer& lhs, const integer& rhs) noexcept
{
    return !(lhs == rhs);
}

bool operator<(const integer& lhs, const integer& rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

bool operator<=(const integer& lhs, const integer& rhs) noexcept
{
    return !(rhs < lhs);
}

bool operator>(const integer& lhs, const integer& rhs) noexcept
{
    return rhs < lhs;
}

bool operator>=(const integer& lhs, const integer& rhs) noexcept
{
    return !(lhs < rhs);
}

integer operator+(const integer& lhs, const integer& rhs)
{
    return integer(lhs) += rhs;
}
/*
integer operator-(const integer& lhs, const integer& rhs)
{
}

integer operator*(const integer& lhs, const integer& rhs)
{
}

integer operator/(const integer& lhs, const integer& rhs)
{
}

integer operator%(const integer& lhs, const integer& rhs)
{
}
*/

void integer_data_proxy::enlarge(int len)
{
    data_.reserve(len);
    data_type subst = neg_ ? -1 : 0;
    for (int i = data_.size(); i < len; ++i) {
        data_.push_back(subst);
    }
}

void integer_data_proxy::normalize()
{
    while (!data_.empty()) {
        if (data_.back() != default_) {
            break;
        }
        data_.pop_back();
    }
}

integer_data_proxy& integer_data_proxy::operator+=(const integer_data_proxy& rhs)
{
    size_t len = std::max(data_.size(),  rhs.data_.size()) + 1;

    uarithmetic_type sum = 0;
    for (int i = 0; i != len; ++i) {
        sum += (*this)[i] + rhs[i];
        if (i == data_.size()) {
            data_.push_back(sum % base);
        } else {
            data_[i] = sum % base;
        }
        sum /= base;
    }

    neg_ = sum != 0;
    default_ = neg_ ? data_type(-1) : data_type(0);
    normalize();

    return *this;
}

/*
integer_data_proxy& integer_data_proxy::operator-=(const integer_data_proxy& rhs)
{
}

integer_data_proxy& integer_data_proxy::operator*=(const integer_data_proxy& rhs)
{
}

integer_data_proxy& integer_data_proxy::operator/=(const integer_data_proxy& rhs)
{
}

integer_data_proxy& integer_data_proxy::operator%=(const integer_data_proxy& rhs)
{
}
*/

int integer_data_proxy::compare(const integer_data_proxy& rhs) const noexcept
{
    if (neg_) {
        if (rhs.neg_) {
        } else {
            //  lhs < rhs
            return -1;
        }
        // both negative
        size_t lSize = data_.size();
        size_t rSize = rhs.data_.size();
        if (lSize < rSize) {
            return 1;
          }
        if (rSize < lSize) {
            return -1;
          }
        for (auto&& lIter = data_.rbegin(), rIter = rhs.data_.rbegin(); lIter !=  data_.rend(); ++lIter, ++rIter) {
            if (*lIter < *rIter) {
                return 1;
              }
            if (*rIter < *lIter) {
                return -1;
              }
          }
        return 0;
    } else {
        if (rhs.neg_) {
            //  lhs > rhs
            return 1;
        }
        //  both positive:
        size_t lSize = data_.size();
        size_t rSize = rhs.data_.size();
        if (lSize < rSize) {
            return -1;
        }
        if (rSize < lSize) {
            return 1;
        }
        for (auto&& lIter = data_.rbegin(), rIter = rhs.data_.rbegin(); lIter !=  data_.rend(); ++lIter, ++rIter) {
            if (*lIter < *rIter) {
                return -1;
            }
            if (*rIter < *lIter) {
                return 1;
            }
        }
        return 0;
    }
}

integer_data_proxy& integer_data_proxy::negate() noexcept
{
    neg_ = !neg_;
    for (auto& val : data_) {
        val = ~val;
    }
    return *this += 1;
}

integer_data_proxy::data_type integer_data_proxy::operator[](size_t pos) const
{
    return (const_cast<integer_data_proxy&>(*this))[pos];
}

integer_data_proxy::data_type& integer_data_proxy::operator[](size_t pos)
{
    if (pos < data_.size()) {
        return data_[pos];
    }

    return default_;
}

integer& integer::operator+=(const integer& rhs)
{
    data_ +=  rhs.data_;
    return *this;
}


}}}

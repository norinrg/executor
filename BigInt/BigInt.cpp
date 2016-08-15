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

integer_data_proxy& integer_data_proxy::operator+=(const integer_data_proxy& rhs)
{
    integer_data_proxy prox(rhs,  int());
    if (prox.neg_) {
        prox.negate();
    }

    // !!!
    data_ +=  rhs.data_;
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
integer_data_proxy& integer_data_proxy::negate() noexcept
{
    neg_ = !neg_;
    for (auto& val : data_) {
        val = ~val;
    }
    return *this += 1;
}

}}}

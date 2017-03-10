/*
 * ip_address.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: sergey.fedorov
 */

#include <tip/iri/ip_address.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace tip {
namespace iri {

inline namespace v2 {

::std::ostream&
operator << (::std::ostream& os, ipv4_address const& val)
{
    ::std::ostream::sentry s (os);
    if (s) {
        os  << ((val.data & 0xff000000) >> 24) << '.'
            << ((val.data & 0x00ff0000) >> 16) << '.'
            << ((val.data & 0x0000ff00) >> 8) << '.'
            <<  (val.data & 0x000000ff);
    }
    return os;
}

::std::string
to_string(ipv4_address const& val)
{
    ::std::ostringstream os;
    os << val;
    return os.str();
}

struct zero_run : ::std::pair< ::std::uint16_t const*, ::std::uint16_t const* > {
    using pair::pair;
    using iterator = pair::first_type;
    zero_run(iterator i) : pair{i, i} {}

    ::std::ptrdiff_t
    size() const
    { return second - first; }
};

::std::ostream&
operator << (::std::ostream& os, ipv6_address const& val)
{
    ::std::ostream::sentry s (os);
    if (s) {
        auto begin = val.data;
        auto end = val.data + 8;

        // Find longest run of zeroes
        zero_run max{ end };
        zero_run current{ end };

        for (auto p = begin; p != end; ++p) {
            if (*p != 0) {
                if (current.size() > max.size())
                    max = current;
                current = {end};
            } else {
                if (current.first == end)
                    current = zero_run{p};
                ++current.second;
            }
        }
        if (current.size() > max.size())
            max = current;

        if (max.size() > 1) {
            if (max.size() == 5 && max.first == val.data
                    && val.data[5] == 0xffff) {
                // IPv4 Mapped
                ipv4_address::repr v4 = (val.data[6] << 16) | val.data[7];
                os << "::ffff:" << ipv4_address{ v4 };
            } else {
                auto p = begin;
                for (; p != max.first; ++p) {
                    if (p != begin)
                        os << ":";
                    os << ::std::hex << *p;
                }
                os << "::";
                p = max.second;
                for (; p != end; ++p) {
                    if (p != max.second)
                        os << ":";
                    os << ::std::hex << *p;
                }
            }
        } else {
            for (auto p = begin; p != end; ++p) {
                if (p != val.data)
                    os << ":";
                os << ::std::hex << *p;
            }
        }
    }
    return os;
}

::std::string
to_string(ipv6_address const& val)
{
    ::std::ostringstream os;
    os << val;
    return os.str();
}

ipv6_address::ipv6_address(char const* str)
    : data{0}
{
//    using element = ipv6_address::element;
//
//    element head[ ipv6_address::size ] {0};
//    ::std::size_t head_size{0};
//    element tail[ ipv6_address::size ] {0};
//    ::std::size_t tail_size{0};
//
//    ::std::int32_t  curr_hextet{0};
//    // track classifications to find out if the number contains decimal digits only
//    char_type       curr_chars{ char_type::none };
//    //char            prev_char;
//
//    ::std::size_t   hextet_cnt{0};
//
//    auto p = str;
//    while (*p != 0) {
//        auto cls = char_classification::classify(*p);
//        if (any(cls & char_type::xdigit)) {
//            if (curr_chars == char_type::none) {
//                curr_chars = cls;
//            } else {
//                curr_chars = curr_chars & cls;
//            }
//            curr_hextet <<= 8;
//            curr_hextet += detail::digit_value(*p);
//        } else {
//            if (*p == ':') {
//
//            } else if (*p == '.') {
//                // previous hextet was actually the first decimal octet of IPv4 address
//            } else {
//
//            }
//        }
//    }
}


} /* namespace v2 */


}  /* namespace iri */
}  /* namespace tip */

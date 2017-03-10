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


}  /* namespace iri */
}  /* namespace tip */

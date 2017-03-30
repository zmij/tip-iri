/*
 * host.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: zmij
 */

#include <tip/iri/host.hpp>
#include <tip/iri/detail/iri_part.hpp>
#include <tip/iri/detail/escape_string.hpp>

#include <iostream>
#include <sstream>

namespace tip {
namespace iri {
inline namespace v2 {

namespace {

struct lookup_visitor : boost::static_visitor<> {
    ::std::ostream& os;

    lookup_visitor(::std::ostream& o) : os{o} {}

    void
    operator()(::std::string const& reg_name) const
    {
        using escaper = char_escaper< iri_part, iri_part::reg_name >;
        escaper::escape(os, reg_name.begin(), reg_name.end());
    }
    void
    operator()(ipv4_address const& ipv4) const
    {
        os << ipv4;
    }
    void
    operator()(ipv6_address const& ipv6) const
    {
        os << ipv6;
    }
};

struct out_visitor : boost::static_visitor<> {
    ::std::ostream& os;

    out_visitor(::std::ostream& o) : os{o} {}

    void
    operator()(::std::string const& reg_name) const
    {
        os << reg_name;
    }
    void
    operator()(ipv4_address const& ipv4) const
    {
        os << ipv4;
    }
    void
    operator()(ipv6_address const& ipv6) const
    {
        os << '[' << ipv6 << ']';
    }
};

} /* namespace  */

::std::string
host::lookup_string() const
{
    ::std::ostringstream os;
    ::boost::apply_visitor( lookup_visitor{os}, data );
    return os.str();
}

::std::ostream&
operator <<(::std::ostream& os, host const& val)
{
    ::std::ostream::sentry s(os);
    if (s) {
        ::boost::apply_visitor( out_visitor{os}, val.data );
    }
    return os;
}

::std::string
to_string(host const& val)
{
    ::std::ostringstream os;
    os << val;
    return os.str();
}

::std::ostream&
operator <<(::std::ostream& os, host::type const& val)
{
    ::std::ostream::sentry s(os);
    if (s) {
        switch (val) {
            case host::type::ipv4:
                os << "ipv4";
                break;
            case host::type::ipv6:
                os << "ipv6";
                break;
            default:
                os << "reg_name";
                break;
        }
    }
    return os;
}


} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */

/*
 * iri.cpp
 *
 *  Created on: Aug 17, 2015
 *      Author: zmij
 */

#include <tip/iri.hpp>

#include <pushkin/parsers/char_classes.hpp>

#include <tip/iri/detail/escape_string.hpp>

#include <tip/iri/parsers/authority_parser.hpp>
#include <tip/iri/parsers/path_parser.hpp>
#include <tip/iri/parsers/query_parser.hpp>
#include <iostream>

namespace tip {
namespace iri {

inline namespace v2 {

constexpr ipv6_address::repr ipv6_address::empty_addr;

//----------------------------------------------------------------------------
// Userinfo implementation
//----------------------------------------------------------------------------
::std::ostream&
operator <<(::std::ostream& os, userinfo const& val)
{
    ::std::ostream::sentry s(os);
    if (s) {
        if (!val.empty()) {
            using escaper = char_escaper< iri_part, iri_part::user >;
            escaper::escape(os, val.user.begin(), val.user.end());
            os << val.user;
            if (!val.password.empty()) {
                os << ':';
                escaper::escape(os, val.password.begin(), val.password.end());
            }
        }
    }
    return os;
}


//----------------------------------------------------------------------------
// Host implementation
//----------------------------------------------------------------------------
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
        os << '[' << ipv6 << ']';
    }
};

struct empty_check_visitor : ::boost::static_visitor<bool> {

    template < typename T >
    bool
    operator()(T const& addr_variant) const
    { return addr_variant.empty(); }


};

} /* namespace  */

::std::string
host::lookup_string() const
{
    ::std::ostringstream os;
    ::boost::apply_visitor( lookup_visitor{os}, data );
    return os.str();
}

bool
host::empty() const
{
    return ::boost::apply_visitor(empty_check_visitor{}, data);
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


//----------------------------------------------------------------------------
// Path implementation
//----------------------------------------------------------------------------
path
path::parse(::std::string const& s)
{
    parsers::absolute_path_parser<path, iri_part::path_segment> parser;
    parser.parse(s.begin(), s.end());
    if (parser.failed())
        throw ::std::runtime_error{ "IRI: Path parsing failed" };
    return parser.value();
}

::std::ostream&
operator <<(::std::ostream& os, path const& val)
{
    using escaper = char_escaper< iri_part, iri_part::path >;
    ::std::ostream::sentry s(os);
    if (s) {
        if (val.is_rooted())
            os << '/';
        for (auto p = val.begin(); p != val.end(); ++p) {
            if (p != val.begin())
                os << '/';
            escaper::escape(os, p->begin(), p->end());
        }
    }
    return os;
}

::std::string
to_string(path const& val)
{
    ::std::ostringstream os;
    os << val;
    return os.str();
}

//----------------------------------------------------------------------------
//  Query implementation
//----------------------------------------------------------------------------
query
query::parse(::std::string const& s)
{
    parsers::parser<iri_part::query> parser;

    parser.parse(s.begin(), s.end());
    if (parser.failed())
        throw ::std::runtime_error{ "IRI: Query parsing failed" };
    return parser.value();
}

::std::ostream&
operator <<(::std::ostream& os, query const& val)
{
    using escaper = char_escaper< iri_part, iri_part::query >;
    ::std::ostream::sentry s(os);
    if (s) {
        if (!val.empty()) {
            os << '?';
            for (auto p = val.begin(); p != val.end(); ++p) {
                if (p != val.begin())
                    os << '&';
                escaper::escape(os, p->first.begin(), p->first.end());
                os << '=';
                escaper::escape(os, p->second.begin(), p->second.end());
            }
        }
    }
    return os;
}

::std::string
to_string(query const& val)
{
    ::std::ostringstream os;
    os << val;
    return os.str();
}

} /* namespace v2 */

//path
//path::parse(std::string const& s)
//{
//    namespace qi = boost::spirit::qi;
//    typedef std::string::const_iterator string_iterator;
//    typedef grammar::parse::ipath_grammar< string_iterator > ipath_grammar;
//
//    string_iterator f = s.begin();
//    string_iterator l = s.end();
//
//    path p;
//    if (!qi::parse(f, l, ipath_grammar(), p) || f != l) {
//        throw std::runtime_error("Invalid path");
//    }
//    return p;
//}
//
//iri
//parse_iri(std::string const& s)
//{
//    namespace qi = boost::spirit::qi;
//    typedef std::string::const_iterator string_iterator;
//    typedef grammar::parse::iri_grammar< string_iterator > iri_grammar;
//
//    string_iterator f = s.begin();
//    string_iterator l = s.end();
//
//    iri res;
//    if (!qi::parse(f, l, iri_grammar(), res) || f != l) {
//        throw std::runtime_error("Invalid IRI");
//    }
//
//    return res;
//}
//
//std::ostream&
//operator << (std::ostream& out, path const& val)
//{
//    std::ostream::sentry s(out);
//    if (s) {
//        if (val.is_rooted()) {
//            out << "/";
//        }
//        path::const_iterator f = val.begin();
//        path::const_iterator l = val.end();
//        for (path::const_iterator p = f; p != l; ++p) {
//            if (p != f)
//                out << "/";
//            out << *p;
//        }
//    }
//    return out;
//}
//
//std::ostream&
//operator << (std::ostream& out, host const& val)
//{
//    std::ostream::sentry s(out);
//    if (s) {
//        out << static_cast<std::string const&>(val);
//    }
//    return out;
//}
//
//
//std::ostream&
//operator << (std::ostream& out, authority const& val)
//{
//    std::ostream::sentry s(out);
//    if (s) {
//        if (!val.userinfo.empty()) {
//            out << val.userinfo << "@";
//        }
//        out << val.host;
//        if (!val.port.empty()) {
//            out << ":" << val.port;
//        }
//    }
//    return out;
//}
//
//std::ostream&
//operator << (std::ostream& out, basic_iri<query> const& val)
//{
//    std::ostream::sentry s(out);
//    if (s) {
//        if (!val.scheme.empty()) {
//            out << val.scheme << ":";
//        }
//        if (!val.authority.empty()) {
//            out << "//" << val.authority;
//        }
//        if (!val.path.empty()) {
//            out << val.path;
//        }
//        if (!val.query.empty()) {
//            out << '?' << val.query;
//        }
//        if (!val.fragment.empty()) {
//            out << '#' << val.fragment;
//        }
//    }
//    return out;
//}


}  // namespace iri
}  // namespace tip

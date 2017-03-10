/*
 * iri_part.hpp
 *
 *  Created on: Mar 9, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_DETAIL_IRI_PART_HPP_
#define TIP_IRI_DETAIL_IRI_PART_HPP_

#include <tip/iri/detail/char_classes.hpp>
#include <tip/iri/detail/parser_state_base.hpp>

namespace tip {
namespace iri {

inline namespace v2 {

enum class iri_part {
    none            = 0x0000,
    schema          = 0x0001,
    user_info       = schema        * 2,
    reg_name        = user_info     * 2,        // Registered name (domain)
    ipv4_address    = reg_name      * 2,        // Literal IPv4 address
    ipv6_address    = ipv4_address  * 2,        // Literal IPv6 address
    ip_future       = ipv6_address  * 2,        // IP future literal
    ip_literal      = ipv6_address | ip_future,
    host            = reg_name | ipv4_address | ip_literal,
    port            = ip_future     * 2,
    authority       = user_info | host | port,
    path            = port          * 2,
    query_param     = path          * 2,
    query_value     = query_param   * 2,
    query           = query_param | query_value,
    fragment        = query_value   * 2
};

inline constexpr iri_part
operator | (iri_part lhs, iri_part rhs)
{
    using int_type = ::std::underlying_type<iri_part>::type;
    return static_cast<iri_part>(static_cast<int_type>(lhs) | static_cast<int_type>(rhs));
}

inline constexpr iri_part
operator & (iri_part lhs, iri_part rhs)
{
    using int_type = ::std::underlying_type<iri_part>::type;
    return static_cast<iri_part>(static_cast<int_type>(lhs) & static_cast<int_type>(rhs));
}

inline constexpr iri_part
operator ^ (iri_part lhs, iri_part rhs)
{
    using int_type = ::std::underlying_type<iri_part>::type;
    return static_cast<iri_part>(static_cast<int_type>(lhs) ^ static_cast<int_type>(rhs));
}

inline constexpr bool
operator !(iri_part p)
{
    return p == iri_part::none;
}

inline constexpr bool
any(iri_part p)
{
    return p != iri_part::none;
}

template < typename Charset, iri_part P >
using iri_part_class = character_class< Charset, iri_part, P, iri_part::none >;

namespace char_classes {

using schema_chars          = join< alnum_chars, char_sequence<'+', '-', '.'> >::type;

using schema_delim          = char_sequence<':', '/', '/'>;

using ipv4_chars            = join<digits, char_constant<'.'> >::type;
using ipv6_chars            = join<hex_digits, char_constant<':'>, ipv4_chars>::type;

constexpr char sub_delim_str[] {"!$&'()*+,;="};
constexpr char gen_delim_str[] {":/?#[]@"};
constexpr char unreserved_str[] {"-._~"};

using sub_delim_chars       = make_char_sequence< sub_delim_str >;
using gen_delim_chars       = make_char_sequence< gen_delim_str >;
using reserved_chars        = unique_sort< join< sub_delim_chars, gen_delim_chars >::type >::type;

// alnum | ucschar | -._~
using unreserved_chars      = unique_sort< join< alnum_chars, make_char_sequence< unreserved_str > >::type >::type;

// unreserved | pct encoded | sub_delims | :@
using ip_chars              = unique_sort< join< unreserved_chars, sub_delim_chars,
                                                char_sequence<':', '@'>>::type >::type;

using fragment_chars        = unique_sort< join< ip_chars, char_sequence< '/', '?' > >::type >::type;


using schema_class      = iri_part_class< schema_chars,     iri_part::schema >;
using ipv4_class        = iri_part_class< ipv4_chars,       iri_part::ipv4_address >;
using ipv6_class        = iri_part_class< ipv6_chars,       iri_part::ipv6_address >;

using fragment_class    = iri_part_class< fragment_chars,   iri_part::fragment >;

} /* namespace char_classes */


//
template < iri_part P >
using part_constant = ::std::integral_constant<iri_part, P>;
template < iri_part ... Parts >
using part_sequence = ::std::integer_sequence<iri_part, Parts...>;
//
//template < typename T, iri_part P >
//struct transition {};
//
//template < typename ... T >
//struct transitions {};
//

namespace detail {

template < iri_part P >
struct part_traits_base {
    using type  = part_constant<P>;
    using next  = part_sequence<>;
    using sub   = part_sequence<>;
    using chars = ::psst::meta::char_sequence<>;
};

} /* namespace detail */


template < iri_part P >
struct part_traits : detail::part_traits_base<P> {};

namespace detail {

template < typename Final, iri_part P >
struct parser_base : parser_state_base< Final > {
    using traits_type   = part_traits<P>;
    using type          = typename traits_type::type;
};

} /* namespace detail */

template < iri_part P >
struct parser : detail::parser_base< parser<P>, P> {};

//
//template <>
//struct part_traits<iri_part::schema> {
//    using type  = part_constant< iri_part::schema >;
//    using next  = part_sequence< iri_part::user_info, iri_part::host, iri_part::path >;
//    using chars = char_classes::schema_chars;
//};
//
///**
// *
// */
//template <>
//struct part_traits<iri_part::none> {
//    using type  = part_constant< iri_part::none >;
//    using next  = part_sequence<
//                    iri_part::schema,
//                    iri_part::user_info,
//                    iri_part::host,
//                    iri_part::path >;
//};
//
//template < iri_part P >
//struct parser {
//    using traits_type   = part_traits<P>;
//    using type          = typename traits_type::type;
//
//    template < typename InputIterator >
//    void
//    operator()(InputIterator& begin, InputIterator end)
//    {
//
//    }
//};

} /* namespace v2 */


} /* namespace iri */
} /* namespace tip */




#endif /* TIP_IRI_DETAIL_IRI_PART_HPP_ */

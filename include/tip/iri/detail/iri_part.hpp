/*
 * iri_part.hpp
 *
 *  Created on: Mar 9, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_DETAIL_IRI_PART_HPP_
#define TIP_IRI_DETAIL_IRI_PART_HPP_

#include <tip/iri/detail/char_class.hpp>
#include <tip/iri/detail/char_classes.hpp>

namespace tip {
namespace iri {

inline namespace v2 {

enum class iri_part {
    none            = 0x0000,
    schema          = 0x0001,
    user_info       = schema            * 2,
    reg_name        = user_info         * 2,        // Registered name (domain)
    ipv4_address    = reg_name          * 2,        // Literal IPv4 address
    ipv6_address    = ipv4_address      * 2,        // Literal IPv6 address
    ip_future       = ipv6_address      * 2,        // IP future literal
    ip_literal      = ipv6_address | ip_future,
    host            = reg_name | ipv4_address | ip_literal,
    port            = ip_future         * 2,
    authority       = user_info | host | port,
    path_nc_segment = port              * 2,
    path_nc         = path_nc_segment   * 2,
    path_segment    = path_nc           * 2,
    path            = path_segment      * 2,
    query_param     = path              * 2,
    query_value     = query_param       * 2,
    query_delim     = query_value       * 2,
    query           = query_param | query_value | query_delim,
    fragment        = query_delim       * 2
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

constexpr char sub_delim_str[]  {"!$&'()*+,;="};
constexpr char gen_delim_str[]  {":/?#[]@"};
constexpr char unreserved_str[] {"-._~"};

constexpr char query_key_str[]  {"!|$'()*+,;:@/?"};

using sub_delim_chars       = make_char_sequence< sub_delim_str >;
using gen_delim_chars       = make_char_sequence< gen_delim_str >;
using reserved_chars        = unique_sort< join< sub_delim_chars, gen_delim_chars >::type >::type;

// alnum | ucschar | -._~
using iunreserved_chars     = unique_sort< join<
                                alnum_chars,
                                make_char_sequence< unreserved_str > >::type >::type;

//// unreserved | pct encoded | sub_delims | :@
using ipchars               = unique_sort< join< iunreserved_chars, sub_delim_chars,
                                                char_sequence<':', '@'>>::type >::type;
using ipchars_nc            = unique_sort< join< iunreserved_chars, sub_delim_chars,
                                                char_sequence<'@'>>::type >::type;

using reg_name_chars        = unique_sort< join< iunreserved_chars, sub_delim_chars
                                    >::type >::type;

using fragment_chars        = unique_sort< join< ipchars, char_sequence< '/', '?' > >::type >::type;

using path_nc_chars         = unique_sort< join< ipchars_nc, char_sequence<'/'> >::type >::type;
using path_chars            = unique_sort< join< ipchars, char_sequence<'/'> >::type >::type;

using query_key_chars       = unique_sort< join< iunreserved_chars,
                                    make_char_sequence< query_key_str > >::type >::type;
using query_val_chars       = unique_sort< join< query_key_chars, char_sequence<'='> >::type >::type;
using query_delim_chars     = char_sequence<'&'>;

using schema_class      = iri_part_class< schema_chars,     iri_part::schema        >;
using ipv4_class        = iri_part_class< ipv4_chars,       iri_part::ipv4_address  >;
using ipv6_class        = iri_part_class< ipv6_chars,       iri_part::ipv6_address  >;
using reg_name_class    = iri_part_class< reg_name_chars,   iri_part::reg_name      >;
using path_seg_nc_class = iri_part_class< ipchars_nc,       iri_part::path_nc_segment >;
using path_nc_class     = iri_part_class< path_nc_chars,    iri_part::path_nc       >;
using path_seg_class    = iri_part_class< ipchars,          iri_part::path_segment  >;
using path_class        = iri_part_class< path_chars,       iri_part::path          >;
using query_key_class   = iri_part_class< query_key_chars,  iri_part::query_param   >;
using query_val_class   = iri_part_class< query_val_chars,  iri_part::query_value   >;
using query_delim_class = iri_part_class< query_delim_chars,iri_part::query_delim   >;

using fragment_class    = iri_part_class< fragment_chars,   iri_part::fragment      >;

using iri_parts_table_base = character_class_table<128, iri_part,
        schema_class,
        ipv4_class,
        ipv6_class,
        reg_name_class,
        path_seg_nc_class,
        path_nc_class,
        path_seg_class,
        path_class,
        query_key_class,
        query_val_class,
        query_delim_class
    >;

} /* namespace char_classes */

struct iri_char_classification : char_classes::iri_parts_table_base {};

template <>
struct char_classification_traits<iri_part> {
    using type = iri_char_classification;
};
//
template < iri_part P >
using part_constant = ::std::integral_constant<iri_part, P>;
template < iri_part ... Parts >
using part_sequence = ::std::integer_sequence<iri_part, Parts...>;
//
} /* namespace v2 */


} /* namespace iri */
} /* namespace tip */




#endif /* TIP_IRI_DETAIL_IRI_PART_HPP_ */

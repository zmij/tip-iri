/*
 * char_class.hpp
 *
 *  Created on: Mar 10, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_DETAIL_CHAR_CLASS_HPP_
#define TIP_IRI_DETAIL_CHAR_CLASS_HPP_

#include <tip/iri/detail/char_classes.hpp>
#include <tip/iri/detail/parser_state_base.hpp>

namespace tip {
namespace iri {

inline namespace v2 {

enum class char_type {
    none        = 0x00,
    horz_space  = 0x01,
    vert_space  = horz_space    * 2,
    whitespace  = horz_space | vert_space,
    alpha       = vert_space    * 2,
    digit       = alpha         * 2,
    xdigit      = digit         * 2,
    octdigit    = xdigit        * 2,
    punct       = octdigit      * 2,
    arithmetic  = punct         * 2,
    logic       = arithmetic    * 2,
    utf_2bytes  = logic         * 2,
    utf_3bytes  = utf_2bytes    * 2,
    utf_4bytes  = utf_3bytes    * 2,
    utf_header  = utf_2bytes | utf_3bytes | utf_4bytes,
    utf_cont    = utf_4bytes    * 2
};

template < char_type T >
using char_type_constant = ::std::integral_constant<char_type, T>;

inline constexpr char_type
operator |(char_type lhs, char_type rhs)
{
    using int_type = ::std::underlying_type<char_type>::type;
    return static_cast<char_type>(static_cast<int_type>(lhs)
            | static_cast<int_type>(rhs));
}


inline constexpr char_type
operator &(char_type lhs, char_type rhs)
{
    using int_type = ::std::underlying_type<char_type>::type;
    return static_cast<char_type>(static_cast<int_type>(lhs)
            & static_cast<int_type>(rhs));
}

inline constexpr char_type
operator ^(char_type lhs, char_type rhs)
{
    using int_type = ::std::underlying_type<char_type>::type;
    return static_cast<char_type>(static_cast<int_type>(lhs)
            ^ static_cast<int_type>(rhs));
}

inline constexpr bool
operator !(char_type p)
{
    return p == char_type::none;
}

inline constexpr bool
any(char_type p)
{
    return p != char_type::none;
}

template < typename Charset, char_type C >
using char_classificator = character_class<Charset, char_type, C, char_type::none>;

namespace char_classes {

using horz_ws_class     = char_classificator< horizontal_space,     char_type::horz_space  >;
using vert_ws_class     = char_classificator< vertical_space,       char_type::vert_space  >;
using alpha_class       = char_classificator< alpha_chars,          char_type::alpha       >;
using digits_class      = char_classificator< digits,               char_type::digit       >;
using xdigits_class     = char_classificator< hex_digits,           char_type::xdigit      >;
using oct_digits_class  = char_classificator< oct_digits,           char_type::octdigit    >;
using punct_class       = char_classificator< punctuation_chars,    char_type::punct       >;
using arithmetic_class  = char_classificator< arithmetic_ops,       char_type::arithmetic  >;
using logic_class       = char_classificator< logic_ops,            char_type::logic       >;

using char_types_table_base = character_class_table< 128, char_type,
            horz_ws_class,
            vert_ws_class,
            alpha_class,
            digits_class,
            xdigits_class,
            oct_digits_class,
            punct_class,
            arithmetic_class,
            logic_class
        >;
} /* namespace char_classes */

enum class utf8_masks {
    utfc_byte   = 0x80, // UTF-8 continuation byte                  10xxxxxx
    utfc_mask   = 0xc0, // mask for checking a continuation byte    11000000
    utf2_bytes  = 0xc0, // UTF-8 first byte for 2-byte sequence     110xxxxx
    utf2_mask   = 0xe0, // mask for checking 2-byte header          11100000
    utf3_bytes  = 0xe0, // UTF-8 first byte for 3-byte sequence     1110xxxx
    utf3_mask   = 0xf0, // mask for checking 3-byte header          11110000
    utf4_bytes  = 0xf0, // UTF-8 first byte for 4-byte sequence     11110xxx
    utf4_mask   = 0xf8  // mask for checking 4-byte header          11111000
};

inline constexpr utf8_masks
operator |(utf8_masks lhs, utf8_masks rhs)
{
    using int_type = ::std::underlying_type<utf8_masks>::type;
    return static_cast<utf8_masks>(static_cast<int_type>(lhs)
            | static_cast<int_type>(rhs));
}

inline constexpr utf8_masks
operator &(utf8_masks lhs, utf8_masks rhs)
{
    using int_type = ::std::underlying_type<utf8_masks>::type;
    return static_cast<utf8_masks>(static_cast<int_type>(lhs)
            & static_cast<int_type>(rhs));
}

inline constexpr utf8_masks
operator &(unsigned char lhs, utf8_masks rhs)
{
    using int_type = ::std::underlying_type<utf8_masks>::type;
    return static_cast<utf8_masks>(lhs & static_cast<int_type>(rhs));
}

inline constexpr utf8_masks
operator ^(utf8_masks lhs, utf8_masks rhs)
{
    using int_type = ::std::underlying_type<utf8_masks>::type;
    return static_cast<utf8_masks>(static_cast<int_type>(lhs)
            ^ static_cast<int_type>(rhs));
}

inline constexpr bool
operator !(utf8_masks p)
{
    return p == static_cast<utf8_masks>(0);
}

inline constexpr bool
any(utf8_masks p)
{
    return p != static_cast<utf8_masks>(0);
}


struct char_classification : char_classes::char_types_table_base {
    using char_type_classification = char_classes::char_types_table_base;
    static constexpr char_type
    classify(char c)
    {
        unsigned char uc = c;

        if (uc > 127) {
            if ((uc & utf8_masks::utf4_mask) == utf8_masks::utf4_bytes) {
                return char_type::utf_4bytes;
            } else if ((uc & utf8_masks::utf3_mask) == utf8_masks::utf3_bytes) {
                return char_type::utf_3bytes;
            } else if ((uc & utf8_masks::utf2_mask) == utf8_masks::utf2_bytes) {
                return char_type::utf_2bytes;
            } else if ((uc & utf8_masks::utfc_mask) == utf8_masks::utfc_byte) {
                return char_type::utf_cont;
            }

            return char_type::none;
        }
        return char_type_classification::classify(c);
    }
};

inline constexpr ::std::size_t
codepoint_size(char_type c) {
    switch (c) {
        case char_type::utf_2bytes:
            return 2;
        case char_type::utf_3bytes:
            return 3;
        case char_type::utf_4bytes:
            return 4;
        default:
            return 1;
    }
}

inline ::std::size_t
codepoint_count(char const* str)
{
    auto p = str;
    ::std::size_t sz{0};
    while (*p != 0) {
        auto c = char_classification::classify(*p);
        if (any(c & char_type::utf_header)) {
            // skip continuation bytes
            auto cont = codepoint_size(c);
            auto e = p + cont;
            ++p;
            ++sz;
            while (p != e && p != 0) {
                if (char_classification::classify(*p) != char_type::utf_cont) {
                    throw ::std::runtime_error{
                        "Invalid utf-8 codepoint at char " + ::std::to_string(p - str)
                        + " symbol " + ::std::to_string(sz)};
                }
                ++p;
                --cont;
            }
            if (cont != 0) {
                throw ::std::runtime_error{
                    "Invalid utf-8 sequence at char " + ::std::to_string(p - str)
                        + " symbol " + ::std::to_string(sz)};
            }
        } else {
            ++sz;
            ++p;
        }
    }
    return sz;
}

namespace detail {

constexpr ::std::int32_t
digit_value(char c)
{
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else if ('A' <= c && c <= 'F') {
        return c - 'A' + 10;
    } else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

template < ::std::size_t Base >
struct int_parser_chars;

template <>
struct int_parser_chars< 10 > : char_type_constant< char_type::digit > {};
template <>
struct int_parser_chars< 16 > : char_type_constant< char_type::xdigit > {};
template <>
struct int_parser_chars< 8 > : char_type_constant< char_type::octdigit > {};

} /* namespace detail */

template < typename T, ::std::size_t Base = 10, ::std::size_t MaxDigits = 0 >
struct uint_parser : detail::parser_state_base< uint_parser<T, Base, MaxDigits> > {
    static_assert( ::std::is_integral<T>::value, "Uint parser instantiated for a non-integral type" );

    static constexpr ::std::size_t base         = Base;
    static constexpr ::std::size_t max_digits   = MaxDigits;

    using base_type     = detail::parser_state_base< uint_parser<T, Base, MaxDigits> >;
    using parser_state  = detail::parser_state;
    using char_type     = detail::int_parser_chars<base>;
    using value_type    = T;

    using base_type::want_more;
    using base_type::start;
    using base_type::finish;
    using base_type::fail;

    constexpr uint_parser()
        : value_{0}, digits_{0} {}

    parser_state
    feed_char(char c)
    {
        if (want_more()) {
            auto cls = char_classification::classify(c);
            if (any(cls & char_type::value)) {
                start();
                value_ *= base;
                value_ += detail::digit_value(c);
                ++digits_;
                if (max_digits > 0 && digits_ >= max_digits) {
                    finish();
                }
            } else {
                if (digits_ > 0) {
                    return finish();
                }
                return fail();
            }
        }
        return state;
    }

    value_type
    value() const
    { return value_; }

    ::std::size_t
    digits() const
    { return digits_; }

    void
    clear()
    {
        value_ = 0;
        digits_ = 0;
        this->reset();
    }
private:
    using base_type::reset;
    using base_type::state;

    value_type      value_;
    ::std::size_t   digits_;
};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_DETAIL_CHAR_CLASS_HPP_ */

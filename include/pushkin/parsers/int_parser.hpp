/*
 * int_parser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef PUSHKIN_PARSERS_INT_PARSER_HPP_
#define PUSHKIN_PARSERS_INT_PARSER_HPP_

#include <pushkin/parsers/parser_state_base.hpp>
#include <pushkin/parsers/char_class.hpp>

namespace psst {
namespace parsers {

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

template < typename T, ::std::size_t Base = 10,
        ::std::size_t MaxDigits = 0, ::std::size_t MinDigits = 1 >
struct uint_parser : parser_state_base< uint_parser<T, Base, MaxDigits, MinDigits> > {
    static_assert( ::std::is_integral<T>::value, "Uint parser instantiated for a non-integral type" );

    static constexpr ::std::size_t base         = Base;
    static constexpr ::std::size_t max_digits   = MaxDigits;
    static constexpr ::std::size_t min_digits   = MinDigits;

    using base_type     = parser_state_base< uint_parser<T, Base, MaxDigits, MinDigits> >;
    using char_type     = detail::int_parser_chars<base>;
    using value_type    = T;

    using base_type::want_more;
    using base_type::start;
    using base_type::fail;

    constexpr uint_parser()
        : value_{0}, digits_{0} {}

    feed_result
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
                return consumed(true);
            } else {
                if (digits_ >= min_digits) {
                    finish();
                } else {
                    fail();
                }
            }
        }
        return consumed(false);
    }

    parser_state
    finish()
    {
        if (want_more()) {
            if (digits_ < min_digits) {
                return fail();
            }
            state = base_type::finish();
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
    using base_type::consumed;

    value_type      value_;
    ::std::size_t   digits_;
};


} /* namespace parsers */
} /* namespace psst */



#endif /* PUSHKIN_PARSERS_INT_PARSER_HPP_ */

/*
 * literal_parser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_LITERAL_PARSER_HPP_
#define TIP_IRI_PARSERS_LITERAL_PARSER_HPP_

#include <tip/iri/parsers/parser_state_base.hpp>
#include <pushkin/meta/char_sequence.hpp>

namespace tip {
namespace iri {
inline namespace v2 {

struct ignore_value{};

namespace detail {

template < typename Final, typename T >
struct literal_parser_impl;

template < typename Final, char ... Chars >
struct literal_parser_impl<Final, ::psst::meta::char_sequence_literal<Chars...>>
    : detail::parser_state_base<Final> {

    using value_type        = ignore_value;
    using string_literal    = ::psst::meta::char_sequence_literal<Chars...>;
    using base_type         = detail::parser_state_base<Final>;
    using parser_state      = detail::parser_state;
    using feed_result       = detail::feed_result;

    constexpr literal_parser_impl() : current_{ string_literal::static_begin() } {}

    using base_type::want_more;
    using base_type::start;
    using base_type::fail;

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            start();
            if (c != *current_) {
                return fail(false);
            }
            if (++current_ == string_literal::static_end()) {
                finish();
            }
            return consumed(true);
        }
        return consumed(false);
    }

    parser_state
    finish()
    {
        if (want_more()) {
            if (current_ != string_literal::static_end())
                return fail();
            base_type::finish();
        }
        return state;
    }

    void
    clear()
    {
        current_ = string_literal::static_begin();
        base_type::reset();
    }

    value_type
    value() const
    {
        return value_type{};
    }
private:
    using base_type::consumed;
    using base_type::state;
    char const* current_;
};

template < typename Final, char Char >
struct literal_parser_impl<Final, ::psst::meta::char_sequence_literal<Char>>
    : detail::parser_state_base<Final> {

    using value_type        = ignore_value;
    using base_type         = detail::parser_state_base<Final>;
    using parser_state      = detail::parser_state;
    using feed_result       = detail::feed_result;

    static constexpr char check_value = Char;

    using base_type::want_more;
    using base_type::start;
    using base_type::fail;

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            if (c != check_value)
                return fail(false);
            return base_type::finish(true);
        }
        return consumed(false);
    }

    parser_state
    finish()
    {
        if (want_more())
            return fail();
        return state;
    }

    void
    clear()
    {
        base_type::reset();
    }

    value_type
    value() const
    {
        return value_type{};
    }
private:
    using base_type::consumed;
    using base_type::state;
};

} /* namespace detail */


template < char ... Chars >
struct literal_parser
    : detail::literal_parser_impl<
              literal_parser<Chars...>, ::psst::meta::char_sequence_literal<Chars...>> {};

template < char const* Str >
struct literal_str_parser
        : detail::literal_parser_impl<
              literal_str_parser<Str>, ::psst::meta::make_char_literal_s<Str>> {};



} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_PARSERS_LITERAL_PARSER_HPP_ */

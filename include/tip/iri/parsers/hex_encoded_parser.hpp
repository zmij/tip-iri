/*
 * hex_encoded_parser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_HEX_ENCODED_PARSER_HPP_
#define TIP_IRI_PARSERS_HEX_ENCODED_PARSER_HPP_

#include <tip/iri/parsers/int_parser.hpp>
#include <tip/iri/parsers/literal_parser.hpp>
#include <tip/iri/parsers/composite_parsers.hpp>

namespace tip {
namespace iri {
inline namespace v2 {

/**
 * A parser for decoding percent-encoded sequences. The percent sign
 * must be consumed by the owner parser.
 */
struct pct_encoded_parser
        : detail::parser_state_base<pct_encoded_parser> {
    using value_type    = unsigned char;
    using base_type     = detail::parser_state_base<pct_encoded_parser>;
    using parser_state  = detail::parser_state;
    using feed_result   = detail::feed_result;

    using hex_parser    = uint_parser<value_type, 16, 2, 2>;

    constexpr pct_encoded_parser() : hex_{} {}

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            start();
            auto res = hex_.feed_char(c);
            state = res.first;
            return consumed(res.second);
        }
        return consumed(false);
    }

    parser_state
    finish()
    {
        if (want_more()) {
            state = hex_.finish();
        }
        return state;
    }

    void
    clear()
    {
        hex_.clear();
        base_type::reset();
    }
    value_type
    value() const
    { return hex_.value(); }
private:
    hex_parser      hex_;
};

/**
 *
 */
enum class hex_range {
    none,
    ucschar,
    iprivate
};

namespace detail {

template < hex_range >
struct hex_range_check {
    static bool
    check(::std::uint32_t v)
    {
        return false;
    }
};



template <>
struct hex_range_check<hex_range::ucschar> {
    static bool
    check(::std::uint32_t v)
    {
        auto res = (   0xa0 <= v && v <=  0xd7ff)
            || ( 0xf900 <= v && v <=  0xfdcf)
            || ( 0xfdf0 <= v && v <=  0xffef)
            || (0x10000 <= v && v <= 0x1fffd)
            || (0x20000 <= v && v <= 0x2fffd)
            || (0x30000 <= v && v <= 0x3fffd)
            || (0x40000 <= v && v <= 0x4fffd)
            || (0x50000 <= v && v <= 0x5fffd)
            || (0x60000 <= v && v <= 0x6fffd)
            || (0x70000 <= v && v <= 0x7fffd)
            || (0x80000 <= v && v <= 0x8fffd)
            || (0x90000 <= v && v <= 0x9fffd)
            || (0xa0000 <= v && v <= 0xafffd)
            || (0xb0000 <= v && v <= 0xbfffd)
            || (0xc0000 <= v && v <= 0xcfffd)
            || (0xd0000 <= v && v <= 0xdfffd)
            || (0xe1000 <= v && v <= 0xefffd);
        if (!res)
            return res;
        // Check the UTF-8 sequence
        v = ::boost::endian::native_to_big(v);
        char* p = reinterpret_cast<char*>(&v);
        auto e = p + sizeof(v);

        for (; p != e && *p == 0; ++p); // Fast forward zero bytes
        auto cls = char_classification::classify(*p);
        if (!(cls & char_type::utf_header)) {
            return false;
        }
        auto sz = codepoint_size(cls);
        if (p + sz != e) {
            // Invalid UTF-8 sequence size
            return false;
        }

        ++p;
        for (; p != e; ++p) {
            if (char_classification::classify(*p) != char_type::utf_cont) {
                return false;
            }
        }
        return true;
    }
};

template <>
struct hex_range_check<hex_range::iprivate> {
    static constexpr bool
    check(::std::uint32_t v)
    {
        return (( 0xe000 <= v) && (v <=   0xf8ff))
           || (( 0xf0000 <= v) && (v <=  0xffffd))
           || ((0x100000 <= v) && (v <= 0x10fffd));
    }
};

} /* namespace detail */

/**
 * A parser for decoding hex-encoded sequences (%xaaaaaaa). %x must be consumed
 * by the owner parser
 */
template < hex_range R >
struct hex_encoded_parser
        : detail::parser_state_base<hex_encoded_parser<R>> {
    using value_type    = ::std::string;
    using base_type     = detail::parser_state_base<hex_encoded_parser<R>>;
    using parser_state  = detail::parser_state;
    using feed_result   = detail::feed_result;
    using hex_parser    = uint_parser<::std::uint32_t, 16, 8, 2>;
    using check_type    = detail::hex_range_check<R>;

    using base_type::want_more;
    using base_type::start;
    using base_type::fail;

    constexpr hex_encoded_parser() : hex_{} {}

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            auto res = hex_.feed_char(c);
            if (res.first == parser_state::done) {
                return base_type::finish(res.second);
            }
            state = res.first;
            return consumed(res.second);
        }
        return consumed(false);
    }

    parser_state
    finish()
    {
        if (want_more()) {
            // Check the value
            auto res = hex_.finish();
            if (hex_.failed())
                return fail();
            if (!check_value())
                return fail();
            return base_type::finish();
        }
        return state;
    }

    void
    clear()
    {
        hex_.clear();
        base_type::reset();
    }

    value_type
    value() const
    {
        value_type str;
        auto v = ::boost::endian::native_to_big(hex_.value());
        char* p = reinterpret_cast<char*>(&v);
        auto e = p + sizeof(v);

        for (; p != e && *p == 0; ++p); // Fast forward zero bytes

        str.reserve(e - p);
        for (; p != e; ++p) {
            str.push_back(*p);
        }

        return str;
    }
private:
    bool
    check_value()
    {
        return check_type::check(hex_.value());
    }

    using base_type::consumed;
    using base_type::state;

    hex_parser      hex_;
};


template < typename SeqParser, char ... Escape >
struct escape_sequence_parser
    : detail::parser_state_base<escape_sequence_parser<SeqParser, Escape...> > {

    using value_parser  = SeqParser;
    using value_type    = typename value_parser::value_type;
    using base_type     = detail::parser_state_base<escape_sequence_parser<SeqParser, Escape...>>;
    using parser_state  = detail::parser_state;
    using feed_result   = detail::feed_result;

    using parser_seq    = detail::sequental_parser<
                                literal_parser<Escape...>,
                                value_parser
                            >;

    using base_type::want_more;
    using base_type::fail;

    feed_result
    feed_char(char c)
    {
        bool consumed = false;
        if (want_more()) {
            auto res = parser_.feed_char(c);
            if (detail::failed(res.first))
                return fail(res.second);
            if (detail::done(res.first))
                return base_type::finish(res.second);
            consumed = res.second;
        }
        return base_type::consumed(consumed);
    }
    parser_state
    finish()
    {
        if (want_more()) {
            auto res = parser_.finish();
            if (detail::failed(res))
                return fail();
        }
        return base_type::finish();
    }

    value_type
    value() const
    {
        auto v = parser_.value();
        return ::std::get<1>(v);
    }

    void
    clear()
    {
        parser_.clear();
        base_type::reset();
    }
private:
    parser_seq  parser_;
};

using pct_encoded_sequence_parser = escape_sequence_parser<pct_encoded_parser, '%'>;
using hex_encoded_ucs_parser
        = escape_sequence_parser<hex_encoded_parser<hex_range::ucschar>, '%', 'x'>;
using hex_encoded_private_parser
        = escape_sequence_parser<hex_encoded_parser<hex_range::iprivate>, '%', 'x'>;

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */




#endif /* TIP_IRI_PARSERS_HEX_ENCODED_PARSER_HPP_ */

/*
 * host_parser.hpp
 *
 *  Created on: Mar 11, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_AUTHORITY_PARSER_HPP_
#define TIP_IRI_PARSERS_AUTHORITY_PARSER_HPP_

#include <tip/iri/host.hpp>
#include <tip/iri/parsers/ip_address_parsers.hpp>
#include <tip/iri/parsers/reg_name_parser.hpp>

#include <numeric>

namespace tip {
namespace iri {
inline namespace v2 {

template<>
struct parser<iri_part::schema>
        : detail::parser_base<parser<iri_part::schema>, iri_part::schema> {

    using value_type        = ::std::string;
    using base_type         = detail::parser_base<parser<iri_part::schema>, iri_part::schema>;
    using parser_state      = detail::parser_state;
    using feed_result       = detail::feed_result;

    parser() {}

    feed_result
    feed_char(char c)
    {
        bool consumed = false;
        if (want_more()) {
            auto cls = char_classification::classify(c);
            if (empty()) {
                if (!(cls & char_type::alpha))
                    return fail(false);
                start();
                val_.push_back(c);
                consumed = true;
            } else {
                auto icls = iri_char_classification::classify(c);
                if (any(icls & iri_part::schema)) {
                    val_.push_back(c);
                    consumed = true;
                } else {
                    finish();
                }
            }
        }
        return base_type::consumed(consumed);
    }

    parser_state
    finish()
    {
        if (want_more()) {
            if (val_.empty())
                return fail();
            base_type::finish();
        }
        return state;
    }

    void
    clear()
    {
        val_.clear();
        base_type::reset();
    }

    value_type const&
    value() const
    {
        return val_;
    }
private:
    value_type val_;
};

template <>
struct parser<iri_part::host>
    : detail::parser_base<parser<iri_part::host>, iri_part::host> {

    using value_type        = v2::host;

    using base_type         = detail::parser_base<parser<iri_part::host>, iri_part::host>;
    using parser_state      = detail::parser_state;
    using feed_result       = detail::feed_result;

    using sub_parsers       = detail::alternatives_parser<
                                parser<iri_part::ipv4_address>,
                                parser<iri_part::ip_literal>,
                                parser<iri_part::reg_name>
                              >;

    parser() {}

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            auto res = parsers_.feed_char(c);
            if (parsers_.finished())
                finish();
            return consumed(res.second);
        }
        return consumed(false);
    }
    parser_state
    finish()
    {
        if (want_more()) {
            parsers_.finish();
            if (parsers_.failed())
                return fail();
            base_type::finish();
        }
        return state;
    }

    void
    clear()
    {
        parsers_.clear();
        base_type::reset();
    }

    value_type
    value() const
    {
        return value_type{ parsers_.value() };
    }
private:
    sub_parsers     parsers_;
};

template <>
struct parser<iri_part::port>
        : detail::parser_base<parser<iri_part::port>, iri_part::port> {

    using value_type    = ::std::uint16_t;

    using base_type = detail::parser_base<parser<iri_part::port>, iri_part::port>;
    using parser_state      = detail::parser_state;
    using feed_result       = detail::feed_result;

    constexpr parser() : port_{} {}

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            start();
            auto res = port_.feed_char(c);
            if (port_.failed())
                return fail(res.second);
            if (port_.done()) {
                finish();
                if (failed())
                    return consumed(false);
            }
            return consumed(res.second);
        }
        return consumed(false);
    }

    parser_state
    finish()
    {
        if (want_more()) {
            port_.finish();
            if (port_.failed())
                return fail();
            if ( port_.value() > ::std::numeric_limits<value_type>::max())
                return fail();
            base_type::finish();
        }
        return state;
    }

    void
    clear()
    {
        port_.clear();
        base_type::reset();
    }

    value_type
    value() const
    {
        return port_.value();
    }
private:
    using int_parser = uint_parser<::std::uint32_t, 10, 5>;

    int_parser port_;
};

template<>
struct parser<iri_part::authority>
        : detail::parser_base<parser<iri_part::authority>, iri_part::authority> {

    using host_parser   = parser< iri_part::host >;
    using port_parser   = parser< iri_part::port >;
    using value_type    = ::std::tuple<host_parser::value_type, port_parser::value_type>;

    using base_type = detail::parser_base<parser<iri_part::authority>, iri_part::authority>;
    using parser_state      = detail::parser_state;
    using feed_result       = detail::feed_result;

    parser() {}

    feed_result
    feed_char(char c)
    {
        bool consumed = false;
        if (want_more()) {
            start();
            if (host_.want_more()) {
                auto res = host_.feed_char(c);
                if (!res.second) {
                    if (host_.failed()) {
                        return fail(false);
                    }
                    if (c == ':') {
                        host_.finish();
                        // proceed with reading the port
                        return base_type::consumed(true);
                    } else {
                        finish();
                    }
                }
                consumed = res.second;
            } else {
                if (c == ':' && port_.empty()) {
                    return base_type::consumed(true);
                }
                auto res = port_.feed_char(c);
                if (port_.failed())
                    return fail(false);
                if (port_.done())
                    finish();
                consumed = res.second;
            }
        }
        return base_type::consumed(consumed);
    }

    parser_state
    finish()
    {
        if (want_more()) {
            host_.finish();
            if (host_.empty() || host_.failed())
                return fail();
            port_.finish();
            base_type::finish();
        }
        return state;
    }

    value_type
    value() const
    {
        port_parser::value_type p{0};
        if (!port_.empty() && !port_.failed())
            p = port_.value();
        return ::std::make_tuple( host_.value(), p );
    }

    void
    clear()
    {
        host_.clear();
        port_.clear();
        base_type::reset();
    }
private:
    host_parser     host_;
    port_parser     port_;
};

inline ::std::ostream&
operator <<(::std::ostream& os, ::std::tuple< host, ::std::uint16_t > const& val)
{
    ::std::ostream::sentry s(os);
    if (s) {
        os << ::std::get<0>(val) << ":" << std::get<1>(val);
    }
    return os;
}


} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */


#endif /* TIP_IRI_DETAIL_AUTHORITY_PARSER_HPP_ */

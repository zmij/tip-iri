/*
 * repetition_parser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_REPETITION_PARSER_HPP_
#define TIP_IRI_PARSERS_REPETITION_PARSER_HPP_

#include <tip/iri/parsers/parser_state_base.hpp>
#include <string>
#include <boost/variant.hpp>

namespace tip {
namespace iri {
inline namespace v2 {
namespace detail {

template < typename Output, typename Input >
struct appender;

template <>
struct appender< ::std::string, char > {
    static void
    append(::std::string& s, char c)
    {
        s.push_back(c);
    }
};

template <>
struct appender< ::std::string, unsigned char > {
    static void
    append(::std::string& s, unsigned char c)
    {
        s.push_back(c);
    }
};

template <>
struct appender< ::std::string, ::std::string > {
    static void
    append(::std::string& s, ::std::string const& v)
    {
        s += v;
    }
};

template < typename T, typename ... U >
struct appender < T, ::boost::variant<U...> > {
    struct append_visitor : ::boost::static_visitor<> {
        T& val;

        append_visitor(T& v) : val{v} {}

        template < typename V >
        void
        operator()(V const& v) const
        {
            appender< T, V >::append(val, v);
        }
    };

    static void
    append(T& val, ::boost::variant< U... > const& var)
    {
        ::boost::apply_visitor(append_visitor{val}, var);
    }
};


template < typename Parser, typename OutputType, ::std::size_t MinRep = 1, ::std::size_t MaxRep = 0 >
struct repetition_parser
    : detail::parser_state_base<repetition_parser<Parser, OutputType>> {

    using base_type         = detail::parser_state_base<repetition_parser<Parser, OutputType>>;
    using value_type        = OutputType;
    using parser_type       = Parser;
    using parser_value_type = typename parser_type::value_type;
    using append_type       = appender<value_type, parser_value_type>;

    static constexpr ::std::size_t min_repetitions  = MinRep;
    static constexpr ::std::size_t max_repetitions  = MaxRep;

    static constexpr bool allow_empty   = min_repetitions == 0;
    static constexpr bool unbounded     = max_repetitions == 0;

    static_assert(unbounded || max_repetitions >= min_repetitions,
            "Max repetitions must be at least min repetitions or zero");

    using base_type::want_more;

    repetition_parser() : parser_{}, val_{}, rep_count_{} {}

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            auto res = parser_.feed_char(c);
            if (detail::failed(res.first)) {
                finish();
                return base_type::consumed(res.second);
            }
            if (detail::done(res.first)) {
                ++rep_count_;
                append_type::append(val_, parser_.value());
                parser_.clear();
                if (!unbounded && rep_count_ >= max_repetitions) {
                    finish();
                    return base_type::consumed(res.second);
                }

                if (!res.second) {
                    return feed_char(c);
                }
            }
            return base_type::consumed(res.second);
        }
        return base_type::consumed(false);
    }

    parser_state
    finish()
    {
        if (want_more()) {
            if (!parser_.empty()) {
                auto res = parser_.finish();
                if (!detail::failed(res)) {
                    ++rep_count_;
                    append_type::append(val_, parser_.value());
                }
                parser_.clear();
            }
            if (!allow_empty && rep_count_ < min_repetitions)
                return fail();
            base_type::finish();
        }
        return state;
    }

    void
    clear()
    {
        parser_.clear();
        val_ = value_type{};
        rep_count_ = 0;
        base_type::reset();
    }

    value_type const&
    value() const
    { return val_; }
private:
    using base_type::fail;
    using base_type::state;

    parser_type     parser_;
    value_type      val_;
    ::std::size_t   rep_count_;
};

} /* namespace detail */
} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_PARSERS_REPETITION_PARSER_HPP_ */

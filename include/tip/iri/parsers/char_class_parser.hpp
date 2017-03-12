/*
 * char_class_parser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_CHAR_CLASS_PARSER_HPP_
#define TIP_IRI_PARSERS_CHAR_CLASS_PARSER_HPP_

#include <tip/iri/detail/char_class.hpp>
#include <tip/iri/parsers/parser_state_base.hpp>

namespace tip {
namespace iri {
inline namespace v2 {

template < typename T, T Filter >
struct char_class_parser
        : detail::parser_state_base<char_class_parser< T, Filter >> {

    using classificator = typename char_classification_traits<T>::type;
    static constexpr T filter = Filter;
    using base_type         = detail::parser_state_base<char_class_parser< T, Filter >>;
    using parser_state      = detail::parser_state;
    using feed_result       = detail::feed_result;
    using value_type        = char;

    constexpr char_class_parser() : val_{0} {}

    using base_type::want_more;

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            auto cls = classificator::classify(c);
            if (any(cls & filter)) {
                val_ = c;
                return base_type::finish(true);
            }
            return base_type::fail(false);
        }
        return base_type::consumed(false);
    }
    parser_state
    finish()
    {
        if (want_more()) {
            return fail();
        }
        return state;
    }

    void
    clear()
    {
        val_ = 0;
        base_type::reset();
    }

    value_type
    value() const
    { return val_; }
private:
    using base_type::fail;
    using base_type::state;

    char    val_;
};

template < typename T, T Filter >
struct char_class_sequence_parser
        : detail::parser_state_base<char_class_sequence_parser<T, Filter>> {

    using classificator = typename char_classification_traits<T>::type;
    static constexpr T filter = Filter;
    using base_type         = detail::parser_state_base<char_class_sequence_parser<T, Filter>>;
    using parser_state      = detail::parser_state;
    using feed_result       = detail::feed_result;
    using value_type = ::std::string;

    using base_type::want_more;

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            auto cls = classificator::classify(c);
            if (any(cls & filter)) {
                val_.push_back(c);
                return base_type::consumed(true);
            } else {
                return base_type::finish(false);
            }
        }
        return base_type::consumed(false);
    }
    parser_state
    finish()
    {
        if (want_more()) {
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
    { return val_; }
private:
    using base_type::fail;
    using base_type::state;

    value_type  val_;
};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_PARSERS_CHAR_CLASS_PARSER_HPP_ */

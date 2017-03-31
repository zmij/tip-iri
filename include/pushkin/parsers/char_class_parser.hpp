/*
 * char_class_parser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef PUSHKIN_PARSERS_CHAR_CLASS_PARSER_HPP_
#define PUSHKIN_PARSERS_CHAR_CLASS_PARSER_HPP_

#include <pushkin/parsers/char_class.hpp>
#include <pushkin/parsers/parser_state_base.hpp>

namespace psst {
namespace parsers {

template < typename T, T Filter >
struct char_class_parser
        : parser_state_base<char_class_parser< T, Filter >> {

    using classificator = typename char_classification_traits<T>::type;
    static constexpr T filter = Filter;
    using base_type         = parser_state_base<char_class_parser< T, Filter >>;
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

template < typename T, T Filter, bool AllowEmpty = false >
struct char_class_sequence_parser
        : parser_state_base<char_class_sequence_parser<T, Filter>> {

    using classificator = typename char_classification_traits<T>::type;
    using base_type         = parser_state_base<char_class_sequence_parser<T, Filter>>;
    using value_type        = ::std::string;
    static constexpr T filter           = Filter;
    static constexpr bool allow_empty   = AllowEmpty;

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
            if (!allow_empty && val_.empty())
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
    { return val_; }
private:
    using base_type::fail;
    using base_type::state;

    value_type  val_;
};

} /* namespace parsers */
} /* namespace psst */



#endif /* PUSHKIN_PARSERS_CHAR_CLASS_PARSER_HPP_ */

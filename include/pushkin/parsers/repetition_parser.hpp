/*
 * repetition_parser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef PUSHKIN_PARSERS_REPETITION_PARSER_HPP_
#define PUSHKIN_PARSERS_REPETITION_PARSER_HPP_

#include <pushkin/parsers/parser_state_base.hpp>
#include <pushkin/parsers/detail/bool_ops.hpp>
#include <pushkin/parsers/detail/append.hpp>
#include <string>
#include <boost/variant.hpp>

namespace psst {
namespace parsers {

template < typename Parser, typename OutputType,
        ::std::size_t MinRep = 1, ::std::size_t MaxRep = 0 >
struct repetition_parser
    : parser_state_base<repetition_parser<Parser, OutputType, MinRep, MaxRep>> {

    using base_type         = parser_state_base<repetition_parser<Parser, OutputType, MinRep, MaxRep>>;
    using value_type        = OutputType;
    using parser_type       = Parser;
    using parser_value_type = typename parser_type::value_type;

    static constexpr ::std::size_t min_repetitions  = MinRep;
    static constexpr ::std::size_t max_repetitions  = MaxRep;

    static constexpr bool allow_empty   = min_repetitions == 0;
    static constexpr bool unbounded     = max_repetitions == 0;

    static_assert(unbounded || max_repetitions >= min_repetitions,
            "Max repetitions must be at least min repetitions or zero");

    using base_type::want_more;

    repetition_parser() : init_value_{}, parser_{}, val_{}, rep_count_{} {}
    repetition_parser(value_type&& val)
        : init_value_{ ::std::move(val) }, parser_{},
          val_{ init_value_ }, rep_count_{} {}

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            this->start();
            auto res = parser_.feed_char(c);
            if (parsers::failed(res.first)) {
                finish();
                return base_type::consumed(res.second);
            }
            if (parsers::done(res.first)) {
                ++rep_count_;
                append(val_, parser_.value());
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
                if (!parsers::failed(res)) {
                    ++rep_count_;
                    append(val_, parser_.value());
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
        val_ = init_value_;
        rep_count_ = 0;
        base_type::reset();
    }

    value_type const&
    value() const
    { return val_; }
private:
    using base_type::fail;
    using base_type::state;

    value_type const    init_value_;

    parser_type         parser_;
    value_type          val_;
    ::std::size_t       rep_count_;
};

template < typename Head, typename Tail, typename OutputType,
        ::std::size_t MinRep = 1, ::std::size_t MaxRep = 0 >
struct repeat_tail_parser
    : parser_state_base<repeat_tail_parser<Head, Tail, OutputType, MinRep, MaxRep>> {

    using base_type         = parser_state_base<repeat_tail_parser<Head, Tail, OutputType, MinRep, MaxRep>>;
    using value_type        = OutputType;
    using head_parser_type  = Head;
    using tail_parser_type  = Tail;

    static constexpr ::std::size_t min_repetitions  = MinRep;
    static constexpr ::std::size_t max_repetitions  = MaxRep;

    static constexpr bool allow_empty   = min_repetitions == 0;
    static constexpr bool unbounded     = max_repetitions == 0;

    static_assert(unbounded || max_repetitions >= min_repetitions,
            "Max repetitions must be at least min repetitions or zero");

    using base_type::want_more;

    repeat_tail_parser() : init_value_{}, head_{}, tail_{}, val_{}, rep_count_{0} {}
    repeat_tail_parser(value_type&& val)
        : init_value_{ ::std::move(val) },
          head_{}, tail_{},
          val_{ init_value_ }, rep_count_{0} {}

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            this->start();
            feed_result res{ parser_state::empty, false };
            if (head_.want_more()) {
                res = head_.feed_char(c);
                if (parsers::failed(res.first)) {
                    finish();
                    return base_type::fail(res.second);
                }
                if (parsers::done(res.first)) {
                    append(val_, head_.value());
                }
            }
            if (!res.second) {
                res = tail_.feed_char(c);
                if (parsers::failed(res.first)) {
                    finish();
                    return base_type::consumed(res.second);
                }
                if (parsers::done(res.first)) {
                    ++rep_count_;
                    append(val_, tail_.value());
                    tail_.clear();
                    if (!unbounded && rep_count_ >= max_repetitions) {
                        finish();
                        return base_type::consumed(res.second);
                    }

                    if (!res.second) {
                        return feed_char(c);
                    }
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
            if (!head_.done()) {
                head_.finish();
                if (head_.failed())
                    return fail();
                append(val_, head_.value());
            }
            if (!tail_.empty()) {
                auto res = tail_.finish();
                if (!parsers::failed(res)) {
                    ++rep_count_;
                    append(val_, tail_.value());
                }
                tail_.clear();
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
        head_.clear();
        tail_.clear();
        val_ = init_value_;
        rep_count_ = 0;
        base_type::reset();
    }

    value_type const&
    value() const
    { return val_; }
private:
    using base_type::fail;
    using base_type::state;

    value_type const    init_value_;

    head_parser_type    head_;
    tail_parser_type    tail_;
    value_type          val_;
    ::std::size_t       rep_count_;
};

} /* namespace parsers */
} /* namespace psst */



#endif /* PUSHKIN_PARSERS_REPETITION_PARSER_HPP_ */

/*
 * parser_state_base.hpp
 *
 *  Created on: Mar 10, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_DETAIL_PARSER_STATE_BASE_HPP_
#define TIP_IRI_DETAIL_PARSER_STATE_BASE_HPP_

#include <tuple>
#include <boost/variant.hpp>

namespace tip {
namespace iri {

inline namespace v2 {
namespace detail {

enum class parser_state {
    empty,
    in_progress,
    done,
    failed
};

constexpr bool
empty(parser_state s)
{ return s == parser_state::empty; }

constexpr bool
done(parser_state s)
{ return s == parser_state::done; }

constexpr bool
failed(parser_state s)
{ return s == parser_state::failed; }

constexpr bool
finished(parser_state s)
{ return s >= parser_state::done; }

constexpr bool
want_more(parser_state s)
{ return s < parser_state::done; }

// A pair of current state and flag if the char was consumed
using feed_result       = ::std::pair< parser_state, bool >;

template < typename Final >
struct parser_state_base {
    using static_final_type = Final;

    constexpr parser_state_base()
        : state{parser_state::empty}, consumed_{0} {}

    constexpr parser_state
    status() const
    { return state; }

    constexpr bool
    empty() const
    { return detail::empty(state); }

    constexpr bool
    done() const
    { return detail::done(state); }

    constexpr bool
    failed() const
    { return detail::failed(state); }

    constexpr bool
    finished() const
    { return detail::finished(state); }

    constexpr bool
    want_more() const
    { return detail::want_more(state); }

    constexpr ::std::size_t
    consumed_count() const
    { return consumed_; }

    template < typename InputIterator >
    InputIterator
    parse(InputIterator begin, InputIterator end)
    {
        auto p = begin;
        for (; p != end && want_more(); ++p) {
            _rebind().feed_char(*p);
        }
        if (!done())
            _rebind().finish();
        if (failed())
            return begin;
        return p;
    }
protected:
    feed_result
    consumed(bool did_consume)
    {
        if (did_consume) {
            ++consumed_;
        }
        return { state, did_consume };
    }

    parser_state
    start()
    {
        if (state == parser_state::empty)
            state = parser_state::in_progress;
        return state;
    }
    parser_state
    finish()
    {
        if (state < parser_state::done)
            state = parser_state::done;
        return state;
    }
    feed_result
    finish(bool did_consume)
    {
        finish();
        return consumed(did_consume);
    }


    parser_state
    fail()
    {
        if (state < parser_state::done)
            state = parser_state::failed;
        return state;
    }
    feed_result
    fail(bool did_consume)
    {
        fail();
        return consumed(did_consume);
    }

    void
    reset()
    {
        state = parser_state::empty;
    }

    parser_state    state;
    ::std::size_t   consumed_;
private:
    static_final_type&
    _rebind()
    { return static_cast<static_final_type&>(*this); }
    static_final_type const&
    _rebind() const
    { return static_cast<static_final_type const&>(*this); }
};

inline constexpr bool
conjunction(bool lhs, bool rhs)
{
    return lhs && rhs;
}

template < typename ... T >
constexpr bool
conjunction(bool v, T ... vals)
{
    if (!v)
        return v;
    return v && conjunction(vals...);
}

inline constexpr bool
disjunction(bool lhs, bool rhs)
{
    return lhs || rhs;
}

template < typename ... T >
constexpr bool
disjunction(bool v, T ... vals)
{
    if (v)
        return v;
    return v || disjunction(vals...);
}

constexpr parser_state
best_state(parser_state s)
{
    return s;
}

template < typename ... T >
constexpr parser_state
best_state(parser_state lhs, T... vals)
{
    auto rhs = best_state(vals...);

    if (lhs == parser_state::in_progress)
        return lhs;
    if (rhs == parser_state::in_progress)
        return rhs;
    return lhs < rhs ? lhs : rhs;
}

constexpr feed_result
join_results(feed_result const& r)
{
    return r;
}

template < typename ... T >
constexpr feed_result
join_results(feed_result const& lhs, T ... vals)
{
    auto rhs = join_results(vals...);

    rhs.second = rhs.second || lhs.second;
    rhs.first  = best_state(lhs.first, rhs.first);
    return rhs;
}

template < typename Parser >
parser_state
finish_parser(Parser& p)
{
    return p.finish();
}

template < typename Parser >
bool
clear_parser(Parser& p)
{
    p.clear();
    return true;
}

template < ::std::size_t Index >
struct value_of_parser {
    static constexpr ::std::size_t parser_index = Index;

    template < typename ParserTuple >
    auto
    operator()(ParserTuple const& parsers) const
    {
        return ::std::get<parser_index>(parsers).value();
    }
};

template < ::std::size_t Index >
struct state_of_parser {
    static constexpr ::std::size_t parser_index = Index;

    template < typename ParserTuple >
    parser_state
    operator()(ParserTuple const& parsers) const
    {
        return ::std::get<parser_index>(parsers).status();
    }
};

template < ::std::size_t Index >
struct parser_feeder {
    static constexpr ::std::size_t parser_index = Index;

    template < typename ParserTuple >
    feed_result
    operator()(ParserTuple& parsers, char c) const
    {
        return ::std::get<parser_index>(parsers).feed_char(c);
    }
};


template < typename ... Parsers >
struct composite_parser_base {
    using parsers_tuple = ::std::tuple<Parsers...>;
    static constexpr ::std::size_t size = sizeof ... (Parsers);
    using indexes       = ::std::make_index_sequence< size >;

    void
    clear()
    { clear(indexes{}); }

    parser_state
    finish()
    {
        return finish(indexes{});
    }


protected:
    template < ::std::size_t ... Indexes >
    constexpr bool
    all_done(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return conjunction( ::std::get<Indexes>(parsers_).done() ... );
    }
    template < ::std::size_t ... Indexes >
    constexpr bool
    any_done(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return disjunction( ::std::get<Indexes>(parsers_).done() ... );
    }
    template < ::std::size_t ... Indexes >
    constexpr bool
    all_failed(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return conjunction( ::std::get<Indexes>(parsers_).failed() ... );
    }
    template < ::std::size_t ... Indexes >
    constexpr bool
    any_failed(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return disjunction( ::std::get<Indexes>(parsers_).failed() ... );
    }
    template < ::std::size_t ... Indexes >
    constexpr bool
    all_finished(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return conjunction( ::std::get<Indexes>(parsers_).finished() ... );
    }
    template < ::std::size_t ... Indexes >
    constexpr bool
    any_finished(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return disjunction( ::std::get<Indexes>(parsers_).finished() ... );
    }
    template < ::std::size_t ... Indexes >
    bool
    clear(::std::integer_sequence<::std::size_t, Indexes...> const&)
    {
        return conjunction( clear_parser(::std::get<Indexes>(parsers_)) ... );
    }

    template < ::std::size_t ... Indexes >
    parser_state
    finish(::std::integer_sequence<::std::size_t, Indexes...> const&)
    {
        return best_state( finish_parser( ::std::get< Indexes >(parsers_) )... );
    }

    parser_state
    state_of(::std::size_t idx) const
    {
        if (idx >= size)
            return parser_state::failed;
        return parser_status_table(indexes{})[idx]( parsers_ );
    }

protected:
    parsers_tuple   parsers_;
protected:
    //------------------------------------------------------------------------
    using get_status_func   = ::std::function< parser_state(parsers_tuple const&) >;
    using get_status_table  = ::std::array< get_status_func, size >;

    template < ::std::size_t ... Indexes >
    static get_status_table const&
    parser_status_table(::std::integer_sequence<::std::size_t, Indexes...>)
    {
        static get_status_table _table{{
            state_of_parser<Indexes>{}...
        }};
        return _table;
    }
};

/**
 * Template for feeding chars to a set of alternative parsers until all are
 * done or all fail.
 */
template < typename ... Alternatives >
struct alternatives_parser : composite_parser_base<Alternatives...> {
    using base_type         = composite_parser_base<Alternatives...>;
    using parsers_tuple     = typename base_type::parsers_tuple;
    using indexes           = typename base_type::indexes;
    using value_type        = ::boost::variant< typename Alternatives::value_type ... >;

    static constexpr ::std::size_t size = sizeof ... (Alternatives);
    using parser_counter= ::std::array<::std::size_t, size>;

    alternatives_parser()
        : counts_{{0}} {}

    constexpr bool
    empty() const
    { return empty(indexes{}); }

    constexpr bool
    done() const
    { return base_type::all_done(indexes{}); }

    constexpr bool
    failed() const
    { return base_type::all_failed(indexes{}); }

    constexpr bool
    finished() const
    { return base_type::all_finished(indexes{}); }

    constexpr bool
    want_more() const
    { return want_more(indexes{}); }

    feed_result
    feed_char(char c)
    {
        return feed_char(c, indexes{});
    }

    value_type
    value() const
    {
        auto max_p = counts_.end();
        ::std::size_t max_c{0};
        for (::std::size_t idx = 0; idx < size; ++idx) {
            auto state = base_type::state_of(idx);
            if (!detail::failed(state)) {
                if (counts_[idx] > max_c) {
                    max_c = counts_[idx];
                    max_p = counts_.begin() + idx;
                }
            }
        }

        if (max_p != counts_.end()) {
            return value_of( max_p - counts_.begin() );
        }
        return value_type{};
    }

    void
    clear()
    {
        base_type::clear();
        counts_ = {{0}};
    }
private:
    template < ::std::size_t ... Indexes >
    constexpr bool
    empty(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return conjunction( ::std::get<Indexes>(parsers_).empty() ... );
    }
    template < ::std::size_t ... Indexes >
    constexpr bool
    want_more(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return disjunction( ::std::get<Indexes>(parsers_).want_more() ... );
    }

    template < ::std::size_t ... Indexes >
    feed_result
    feed_char(char c, ::std::integer_sequence<::std::size_t, Indexes...> const&)
    {
        return join_results( feed_char( ::std::get< Indexes >(parsers_), c, Indexes )... );
    }

    template < typename Parser >
    feed_result
    feed_char(Parser& p, char c, ::std::size_t p_idx)
    {
        if (p.want_more()) {
            auto res = p.feed_char(c);
            if (res.second) {
                // Increment char count for parser
                ++counts_[p_idx];
            }
            return res;
        }
        return {p.status(), false};
    }

    value_type
    value_of(::std::size_t idx) const
    {
        if (idx >= size)
            return value_type{};
        return parser_value_extractors(indexes{})[idx](parsers_);
    }
private:
    using base_type::parsers_;
    parser_counter  counts_;
private:
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    using get_value_func    = ::std::function< value_type(parsers_tuple const&) >;
    using get_value_table   = ::std::array< get_value_func, size >;

    template < ::std::size_t ... Indexes >
    static get_value_table const&
    parser_value_extractors(::std::integer_sequence<::std::size_t, Indexes...>)
    {
        static get_value_table _table{{
            value_of_parser<Indexes>{}...
        }};
        return _table;
    }
};

/**
 * Build a sequence of parsers. Will proceed to the next parser when current
 * is done. If a parser in the chain fails, will fail.
 */
template < typename ... Parsers >
struct sequental_parser : composite_parser_base<Parsers...> {
    using base_type         = composite_parser_base<Parsers...>;
    using parsers_tuple     = typename base_type::parsers_tuple;
    using indexes           = typename base_type::indexes;

    static constexpr ::std::size_t size = sizeof ... (Parsers);
    using value_type    = ::std::tuple< typename Parsers::value_type ... >;

    sequental_parser() : current_{0}, failed_{false} {}

    bool
    empty() const
    {
        return current_ == 0 && detail::empty(state_of(current_));
    }

    constexpr bool
    done() const
    {
        return current_ == size;
    }

    bool
    failed() const
    {
        return failed_ || detail::failed(current_state());
    }

    bool
    finished() const
    {
        return done() || failed();
    }

    bool
    want_more() const
    {
        return detail::want_more(current_state());
    }

    feed_result
    feed_char(char c)
    {
        if (want_more()) {
            auto res = parser_feeders(indexes{})[current_](parsers_, c);
            if (detail::done(res.first)) {
                ++current_;
                if (!res.second)
                    return feed_char(c);
            }
            if (detail::failed(res.first)) {
                failed_ = true;
            }
            return { current_state(), res.second };
        }
        return { current_state(), false };
    }

    parser_state
    finish()
    {
        if (want_more()) {
            base_type::finish();
            if (!base_type::all_done(indexes{}))
                failed_ = true;
        }
        return current_state();
    }

    void
    clear()
    {
        base_type::clear();
        current_ = 0;
        failed_  = false;
    }

    value_type
    value() const
    {
        return value( indexes{} );
    }
private:
    using base_type::state_of;

    parser_state
    current_state() const
    {
        if (failed_)
            return parser_state::failed;
        if (current_ < size)
            return state_of(current_);
        return parser_state::done;
    }

    template < ::std::size_t ... Indexes >
    value_type
    value(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return ::std::make_tuple(::std::get<Indexes>(parsers_).value() ... );
    }

private:
    using base_type::parsers_;
    ::std::size_t   current_;
    bool            failed_;
private:
    //------------------------------------------------------------------------
    using feed_char_func    = ::std::function< feed_result(parsers_tuple&, char) >;
    using feed_table        = ::std::array< feed_char_func, size >;

    template < ::std::size_t ... Indexes >
    static feed_table const&
    parser_feeders(::std::integer_sequence<::std::size_t, Indexes...>)
    {
        static feed_table _table {{
            parser_feeder<Indexes>{}...
        }};
        return _table;
    }

};

} /* namespace detal */
} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */




#endif /* TIP_IRI_DETAIL_PARSER_STATE_BASE_HPP_ */

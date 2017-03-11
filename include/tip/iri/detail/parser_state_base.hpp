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
        : state{parser_state::empty} {}

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
    parser_state
    fail()
    {
        if (state < parser_state::done)
            state = parser_state::failed;
        return state;
    }

    void
    reset()
    {
        state = parser_state::empty;
    }

    parser_state    state;
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

/**
 * Template for feeding chars to a set of alternative parsers until all are
 * done or all fail.
 */
template < typename ... Alternatives >
struct alternatives_parser {
    using parsers_tuple = ::std::tuple<Alternatives...>;
    static constexpr ::std::size_t size = sizeof ... (Alternatives);
    using indexes       = ::std::make_index_sequence< size >;
    using value_type    = ::boost::variant< typename Alternatives::value_type ... >;
    using parser_counter= ::std::array<::std::size_t, size>;

    alternatives_parser()
        : parsers_{}, counts_{{0}} {}

    constexpr bool
    empty() const
    { return empty(indexes{}); }

    constexpr bool
    done() const
    { return done(indexes{}); }

    constexpr bool
    failed() const
    { return failed(indexes{}); }

    constexpr bool
    finished() const
    { return finished(indexes{}); }

    constexpr bool
    want_more() const
    { return want_more(indexes{}); }

    feed_result
    feed_char(char c)
    {
        return feed_char(c, indexes{});
    }

    parser_state
    finish()
    {
        return finish(indexes{});
    }

    void
    clear()
    { clear(indexes{}); }

    value_type
    value() const
    {
        auto max_p = counts_.end();
        ::std::size_t max_c{0};
        for (::std::size_t idx = 0; idx < size; ++idx) {
            auto state = state_of(idx);
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
private:
    template < ::std::size_t ... Indexes >
    constexpr bool
    empty(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return conjunction( ::std::get<Indexes>(parsers_).empty() ... );
    }
    template < ::std::size_t ... Indexes >
    constexpr bool
    done(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return conjunction( ::std::get<Indexes>(parsers_).done() ... );
    }
    template < ::std::size_t ... Indexes >
    constexpr bool
    failed(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return conjunction( ::std::get<Indexes>(parsers_).failed() ... );
    }
    template < ::std::size_t ... Indexes >
    constexpr bool
    finished(::std::integer_sequence<::std::size_t, Indexes...> const&) const
    {
        return conjunction( ::std::get<Indexes>(parsers_).finished() ... );
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

    template < ::std::size_t ... Indexes >
    parser_state
    finish(::std::integer_sequence<::std::size_t, Indexes...> const&)
    {
        return best_state( finish( ::std::get< Indexes >(parsers_) )... );
    }

    template < typename Parser >
    parser_state
    finish(Parser& p)
    {
        return p.finish();
    }

    template < ::std::size_t ... Indexes >
    bool
    clear(::std::integer_sequence<::std::size_t, Indexes...> const&)
    {
        return conjunction( clear(::std::get<Indexes>(parsers_)) ... );
    }

    template < typename Parser >
    bool
    clear(Parser& p)
    {
        p.clear();
        return true;
    }

    parser_state
    state_of(::std::size_t idx) const
    {
        if (idx >= size)
            return parser_state::failed;
        return parser_status_table(indexes{})[idx]( parsers_ );
    }

    value_type
    value_of(::std::size_t idx) const
    {
        if (idx >= size)
            return value_type{};
        return parser_value_extractors(indexes{})[idx](parsers_);
    }
private:
    parsers_tuple   parsers_;
    parser_counter  counts_;
private:
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    template < ::std::size_t Index >
    struct value_of_parser {
        static constexpr ::std::size_t parser_index = Index;

        value_type
        operator()(parsers_tuple const& parsers) const
        {
            return ::std::get<parser_index>(parsers).value();
        }
    };
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
    //------------------------------------------------------------------------
    template < ::std::size_t Index >
    struct state_of_parser {
        static constexpr ::std::size_t parser_index = Index;

        parser_state
        operator()(parsers_tuple const& parsers) const
        {
            return ::std::get<parser_index>(parsers).status();
        }
    };

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

} /* namespace detal */
} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */




#endif /* TIP_IRI_DETAIL_PARSER_STATE_BASE_HPP_ */

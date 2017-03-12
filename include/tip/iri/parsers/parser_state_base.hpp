/*
 * parser_state_base.hpp
 *
 *  Created on: Mar 10, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_DETAIL_PARSER_STATE_BASE_HPP_
#define TIP_IRI_DETAIL_PARSER_STATE_BASE_HPP_

#include <tuple>
#include <iterator>

namespace tip {
namespace iri {

inline namespace v2 {

struct ignore_value{};

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

struct parse_end;

template < typename Final >
struct parser_state_base :
            ::std::iterator<::std::output_iterator_tag, void, void, void, void>{
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

    //@{
    /** @name Output iterator interface */
    constexpr bool
    operator == (parse_end const&) const
    {
        return finished();
    }
    constexpr bool
    operator != (parse_end const&) const
    {
        return want_more();
    }
    parser_state_base&
    operator = (char c)
    {
        _rebind().feed_char(c);
        return _rebind();
    }

    parser_state_base&
    operator * ()
    { return *this; }

    static_final_type&
    operator ++ ()
    { return _rebind(); }
    static_final_type&
    operator ++ (int)
    { return _rebind(); }

    constexpr
    operator bool () const
    {
        return want_more();
    }
    //@}

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

struct parse_end {
    template < typename Final >
    constexpr bool
    operator == (parser_state_base<Final> const& rhs) const
    {
        return rhs.finished();
    }
    template < typename Final >
    constexpr bool
    operator != (parser_state_base<Final> const& rhs) const
    {
        return rhs.want_more();
    }

};


} /* namespace detal */

static constexpr detail::parse_end parse_end{};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */




#endif /* TIP_IRI_DETAIL_PARSER_STATE_BASE_HPP_ */

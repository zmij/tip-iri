/*
 * parser_state_base.hpp
 *
 *  Created on: Mar 10, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_DETAIL_PARSER_STATE_BASE_HPP_
#define TIP_IRI_DETAIL_PARSER_STATE_BASE_HPP_

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


template < typename Final >
struct parser_state_base {
    using static_final_type = Final;

    constexpr parser_state_base()
        : state{parser_state::empty} {}

    constexpr bool
    empty() const
    { return state == parser_state::empty; }

    constexpr bool
    done() const
    { return state == parser_state::done; }

    constexpr bool
    failed() const
    { return state == parser_state::failed; }

    constexpr bool
    want_more() const
    { return state < parser_state::done; }

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
        if (state == parser_state::in_progress)
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

} /* namespace detal */
} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */




#endif /* TIP_IRI_DETAIL_PARSER_STATE_BASE_HPP_ */

/*
 * bool_ops.hpp
 *
 *  Created on: Mar 29, 2017
 *      Author: zmij
 */

#ifndef PUSHKIN_PARSERS_BOOL_OPS_HPP_
#define PUSHKIN_PARSERS_BOOL_OPS_HPP_

namespace psst {
namespace parsers {
namespace detail {

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


} /* namespace detail */
} /* namespace parsers */
} /* namespace psst */

#endif /* PUSHKIN_PARSERS_BOOL_OPS_HPP_ */

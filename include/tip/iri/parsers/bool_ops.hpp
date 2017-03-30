/*
 * bool_ops.hpp
 *
 *  Created on: Mar 29, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_BOOL_OPS_HPP_
#define TIP_IRI_PARSERS_BOOL_OPS_HPP_

namespace tip {
namespace iri {
inline namespace v2 {
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
} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */

#endif /* TIP_IRI_PARSERS_BOOL_OPS_HPP_ */

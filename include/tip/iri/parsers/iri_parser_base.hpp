/*
 * iri_parser_base.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_IRI_PARSER_BASE_HPP_
#define TIP_IRI_PARSERS_IRI_PARSER_BASE_HPP_

#include <tip/iri/detail/iri_part.hpp>
#include <tip/iri/parsers/parser_state_base.hpp>

namespace tip {
namespace iri {
inline namespace v2 {

namespace detail {

template < iri_part P >
struct part_traits_base {
    using type  = part_constant<P>;
    using next  = part_sequence<>;
    using sub   = part_sequence<>;
    using chars = ::psst::meta::char_sequence<>;
};


} /* namespace detail */


template < iri_part P >
struct part_traits : detail::part_traits_base<P> {};

namespace detail {

template < typename Final, iri_part P >
struct parser_base : parser_state_base< Final > {
    using traits_type   = part_traits<P>;
    using type          = typename traits_type::type;
};

} /* namespace detail */

template < iri_part P >
struct parser : detail::parser_base< parser<P>, P> {};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_PARSERS_IRI_PARSER_BASE_HPP_ */

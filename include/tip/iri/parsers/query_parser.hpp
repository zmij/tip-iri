/*
 * query_parser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_QUERY_PARSER_HPP_
#define TIP_IRI_PARSERS_QUERY_PARSER_HPP_

#include <tip/iri/query.hpp>
#include <tip/iri/parsers/iri_parser_base.hpp>
#include <tip/iri/parsers/escaped_sequence_parser.hpp>

namespace tip {
namespace iri {
inline namespace v2 {

template<>
struct parser<iri_part::query_param>
    : escaped_sequence_parser<::std::string, iri_part::query_param> {};

template<>
struct parser<iri_part::query_value>
    : escaped_sequence_parser<::std::string, iri_part::query_value, true> {};

namespace detail {

template <>
struct appender<query, ::std::tuple< ::std::string, ::std::string > > {
    static void
    append(query& q, ::std::tuple< ::std::string, ::std::string > const& param)
    {
        q.emplace(::std::get<0>(param), ::std::get<1>(param));
    }
};

} /* namespace detail */

template <>
struct parser<iri_part::query>
    : detail::repeat_tail_parser<
          detail::sequental_parser<
              literal_parser<'?'>,
              parser< iri_part::query_param >,
              literal_parser<'='>,
              parser< iri_part::query_value >
          >,
          detail::sequental_parser<
              literal_parser<'&'>,
              parser< iri_part::query_param >,
              literal_parser<'='>,
              parser< iri_part::query_value >
          >,
          query, 0 > {};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_PARSERS_QUERY_PARSER_HPP_ */

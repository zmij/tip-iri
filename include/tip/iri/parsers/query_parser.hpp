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
namespace parsers {

template<>
struct parser<iri_part::query_param>
    : escaped_sequence_parser<::std::string, iri_part::query_param> {};

template<>
struct parser<iri_part::query_value>
    : escaped_sequence_parser<::std::string, iri_part::query_value, true> {};

template <>
struct parser<iri_part::query>
    : ::psst::parsers::repeat_tail_parser<
          ::psst::parsers::sequental_parser<
              ::psst::parsers::literal_parser<'?'>,
              parser< iri_part::query_param >,
              ::psst::parsers::literal_parser<'='>,
              parser< iri_part::query_value >
          >,
          ::psst::parsers::sequental_parser<
              ::psst::parsers::literal_parser<'&'>,
              parser< iri_part::query_param >,
              ::psst::parsers::literal_parser<'='>,
              parser< iri_part::query_value >
          >,
          query, 0 > {};

} /* namespace parsers */
} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */

namespace psst {
namespace parsers {
namespace detail {

template <>
struct appender<::tip::iri::query, ::std::tuple< ::std::string, ::std::string > > {
    static void
    append(::tip::iri::query& q, ::std::tuple< ::std::string, ::std::string > const& param)
    {
        q.emplace(::std::get<0>(param), ::std::get<1>(param));
    }
};

} /* namespace detail */
} /* namespace parsers */
} /* namespace psst */


#endif /* TIP_IRI_PARSERS_QUERY_PARSER_HPP_ */

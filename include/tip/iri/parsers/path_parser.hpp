/*
 * path_parser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_PATH_PARSER_HPP_
#define TIP_IRI_PARSERS_PATH_PARSER_HPP_

#include <tip/iri/detail/char_class.hpp>
#include <tip/iri/detail/iri_part.hpp>
#include <tip/iri/parsers/hex_encoded_parser.hpp>
#include <tip/iri/parsers/composite_parsers.hpp>
#include <tip/iri/parsers/repetition_parser.hpp>
#include <tip/iri/parsers/char_class_parser.hpp>

namespace tip {
namespace iri {
inline namespace v2 {

template <>
struct parser<iri_part::path_segment>
        : detail::repetition_parser<
              detail::alternatives_parser<
                  char_class_parser<iri_part, iri_part::path_segment>,
                  pct_encoded_sequence_parser,
                  hex_encoded_ucs_parser
              >,
              ::std::string
          > {};

template <>
struct parser<iri_part::path_nc_segment>
        : detail::repetition_parser<
              detail::alternatives_parser<
                  char_class_parser<iri_part, iri_part::path_nc_segment>,
                  pct_encoded_sequence_parser,
                  hex_encoded_ucs_parser
              >,
              ::std::string
          > {};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_PARSERS_PATH_PARSER_HPP_ */

/*
 * escaped_sequence_parser.hpp
 *
 *  Created on: Mar 30, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_ESCAPED_SEQUENCE_PARSER_HPP_
#define TIP_IRI_PARSERS_ESCAPED_SEQUENCE_PARSER_HPP_

#include <pushkin/parsers/char_class.hpp>
#include <pushkin/parsers/composite_parsers.hpp>
#include <pushkin/parsers/repetition_parser.hpp>
#include <pushkin/parsers/char_class_parser.hpp>

#include <tip/iri/detail/iri_part.hpp>
#include <tip/iri/parsers/hex_encoded_parser.hpp>

namespace tip {
namespace iri {
inline namespace v2 {

template < typename OutputType, iri_part SegmentType, bool AllowEmpty = false >
struct escaped_sequence_parser
    : ::psst::parsers::repetition_parser<
          ::psst::parsers::alternatives_parser<
              ::psst::parsers::char_class_parser<iri_part, SegmentType>,
              pct_encoded_sequence_parser,
              hex_encoded_ucs_parser
          >,
          OutputType,
          AllowEmpty ? 0 : 1
      >{};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_PARSERS_ESCAPED_SEQUENCE_PARSER_HPP_ */

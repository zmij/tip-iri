/*
 * reg_name_parser.hpp
 *
 *  Created on: Mar 11, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_REG_NAME_PARSER_HPP_
#define TIP_IRI_PARSERS_REG_NAME_PARSER_HPP_

#include <tip/iri/detail/iri_part.hpp>
#include <tip/iri/detail/char_class.hpp>

#include <tip/iri/parsers/hex_encoded_parser.hpp>
#include <tip/iri/parsers/composite_parsers.hpp>
#include <tip/iri/parsers/repetition_parser.hpp>
#include <tip/iri/parsers/char_class_parser.hpp>

#include <string>
#include <algorithm>

namespace tip {
namespace iri {

inline namespace v2 {

template <>
struct parser<iri_part::reg_name>
    : detail::repetition_parser<
          detail::alternatives_parser<
              char_class_parser<iri_part, iri_part::reg_name>,
              pct_encoded_sequence_parser,
              hex_encoded_ucs_parser
          >,
          ::std::string
      >{};

} /* namespace v2 */

} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_DETAIL_REG_NAME_PARSER_HPP_ */

/*
 * path_parser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PARSERS_PATH_PARSER_HPP_
#define TIP_IRI_PARSERS_PATH_PARSER_HPP_

#include <tip/iri/path.hpp>
#include <tip/iri/parsers/iri_parser_base.hpp>
#include <tip/iri/parsers/escaped_sequence_parser.hpp>

namespace tip {
namespace iri {
inline namespace v2 {

template <>
struct parser<iri_part::path_segment>
        : escaped_sequence_parser<::std::string, iri_part::path_segment, true> {};

template <>
struct parser<iri_part::path_nc_segment>
        : escaped_sequence_parser<::std::string, iri_part::path_nc_segment, true> {};

template < typename OutType, iri_part SegmentType = iri_part::path_segment >
struct absolute_path_parser
        : detail::repetition_parser<
            detail::sequental_parser<
                literal_parser_v<'/'>,
                parser< SegmentType >
            >,
            OutType
        > {};

namespace detail {

template <>
struct appender<path, ::std::string> {
    static void
    append(path& s, ::std::string const& segment)
    {
        if (!segment.empty()) {
            s.push_back(segment);
        }
    }
};

} /* namespace detail */


template < iri_part SegmentType >
struct absolute_path_parser< path, SegmentType >
    : detail::repetition_parser<
        detail::sequental_parser<
            literal_parser<'/'>,
            parser< SegmentType >
        >,
        path
    > {

    using base_type = detail::repetition_parser<
                                detail::sequental_parser<
                                    literal_parser<'/'>,
                                    parser< SegmentType >
                                >,
                                path
                            >;

    absolute_path_parser() : base_type{ path{true} } {}
};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_PARSERS_PATH_PARSER_HPP_ */

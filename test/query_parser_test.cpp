/*
 * query_parser_test.cpp
 *
 *  Created on: Mar 30, 2017
 *      Author: zmij
 */

#include <gtest/gtest.h>
#include <tip/iri/parsers/query_parser.hpp>
#include "parser_test.hpp"

namespace tip {
namespace iri {
inline namespace v2 {

void
PrintTo(query const& val, ::std::ostream* os)
{
    *os << val;
}

} /* namespace v2 */

namespace test {

using query_parser = parsers::parser<iri_part::query>;
PARSER_TEST(query_parser, Query,
    ::testing::Values(
        ParseQuery::make_test_data( "?foo=bar", query{ { "foo", "bar" } } ),
        ParseQuery::make_test_data( "?foo=bar&bar=foo&answer=42",
                query{ { "foo", "bar" }, { "bar", "foo" }, { "answer", "42" } } ),
        ParseQuery::make_test_data( "?foo=&bar=",
                query{ { "foo", "" }, { "bar", "" } })
    ),
    ::testing::Values(
        ParseQuery::make_test_data( "&foo=bar" )
    )
);

} /* namespace test */
} /* namespace iri */
} /* namespace tip */

/*
 * path_parser_test.cpp
 *
 *  Created on: Mar 29, 2017
 *      Author: zmij
 */

#include <gtest/gtest.h>
#include <tip/iri/parsers/path_parser.hpp>
#include "parser_test.hpp"

namespace tip {
namespace iri {
inline namespace v2 {

void
PrintTo(path const& val, ::std::ostream* os)
{
    *os << val;
}

namespace parsers {

namespace test {

using ab_path_parser = absolute_path_parser<::std::string, iri_part::path_segment >;
PARSER_TEST(ab_path_parser, Path,
    ::testing::Values(
        ParsePath::make_test_data( "/usr", "/usr" ),
        ParsePath::make_test_data( "/", "/" ),
        ParsePath::make_test_data( "//", "//" ),
        ParsePath::make_test_data( "/usr/../opt", "/usr/../opt" ),
        ParsePath::make_test_data( "/usr/home", "/usr/home" )
    ),
    ::testing::Values(
        ParsePath::make_test_data( "" ),
        ParsePath::make_test_data( "1http" )
    )
);

using ab_path_struct_parser = absolute_path_parser<v2::path, iri_part::path_segment>;
PARSER_TEST(ab_path_struct_parser, PathStruct,
    ::testing::Values(
        ParsePathStruct::make_test_data( "/usr", path{true, {"usr"}} ),
        ParsePathStruct::make_test_data( "/", path{ true } ),
        ParsePathStruct::make_test_data( "//", path{ true } ),
        ParsePathStruct::make_test_data( "/usr/../opt", path{ true, {"usr", "..", "opt"}} ),
        ParsePathStruct::make_test_data( "/usr/sp%20ce/opt", path{ true, {"usr", "sp ce", "opt"}} ),
        ParsePathStruct::make_test_data( "/usr/%xc990pt", path{ true, {"usr", "ɐpt"}} ),
        ParsePathStruct::make_test_data( "/usr/home", path{ true, { "usr", "home" } } )
    ),
    ::testing::Values(
        ParsePathStruct::make_test_data( "" ),
        ParsePathStruct::make_test_data( "1http" )
    )
);


} /* namespace test */
} /* namespace parsers */
} /* namespace v2 */

} /* namespace iri */
} /* namespace tip */

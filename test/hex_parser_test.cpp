/*
 * hex_grammar_test.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: zmij
 */

#include <gtest/gtest.h>
#include <tip/iri/parsers/hex_encoded_parser.hpp>
#include "parser_test.hpp"

namespace tip {
namespace iri {
namespace test {

PARSER_TEST(pct_encoded_parser, PCT,
    ::testing::Values(
        ParsePCT::make_test_data("20", ' '),
        ParsePCT::make_test_data("7e", '~'),
        ParsePCT::make_test_data("0a", '\n'),
        ParsePCT::make_test_data("0d", '\r')
    ),
    ::testing::Values(
        ParsePCT::make_test_data("n"),
        ParsePCT::make_test_data("1"),
        ParsePCT::make_test_data("a")
    ));

using iprivate_parser = hex_encoded_parser<hex_range::iprivate>;
PARSER_TEST(iprivate_parser, Priv,
    ::testing::Values(
        ParsePriv::make_test_data("ee80", "\xee\x80"),
        ParsePriv::make_test_data("f7ea", "\xf7\xea")
    ),
    ::testing::Values(
        ParsePriv::make_test_data("a0")
    ));

using ucs_parser = hex_encoded_parser<hex_range::ucschar>;
PARSER_TEST(ucs_parser, UCS,
    ::testing::Values(
        ParseUCS::make_test_data("c990", "ɐ"),
        ParseUCS::make_test_data("c9b1", "ɱ"),
        ParseUCS::make_test_data("c9b5", "ɵ"),
        ParseUCS::make_test_data("ca98", "ʘ"),
        ParseUCS::make_test_data("cdbc", "ͼ"),
        ParseUCS::make_test_data("d289", "҉"),
        ParseUCS::make_test_data("d4ab", "ԫ"),
        ParseUCS::make_test_data("d596", "Ֆ"),
        ParseUCS::make_test_data("d790", "א")
    ),
    ::testing::Values(
        ParseUCS::make_test_data("d800"),
        ParseUCS::make_test_data("d8b4", "ش"),
        ParseUCS::make_test_data("db9e", "۞")
    ));

} /* namespace test */
} /* namespace irir */
} /* namespace tip */


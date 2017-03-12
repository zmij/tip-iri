/*
 * repetition_parser_test.cpp
 *
 *  Created on: Mar 12, 2017
 *      Author: zmij
 */

#include <gtest/gtest.h>
#include <tip/iri/parsers/composite_parsers.hpp>
#include <tip/iri/parsers/repetition_parser.hpp>
#include <tip/iri/parsers/char_class_parser.hpp>
#include <tip/iri/parsers/hex_encoded_parser.hpp>
#include <tip/iri/detail/iri_part.hpp>

namespace tip {
namespace iri {
namespace test {

using path_char_parser = char_class_parser<iri_part, iri_part::path_segment>;

TEST(Parser, CharClassParser)
{
    path_char_parser p;
    EXPECT_TRUE(p);
    EXPECT_NE(parse_end, p);
    *p = 'a';
    EXPECT_FALSE(p);
    EXPECT_EQ(parse_end, p);
    EXPECT_EQ(p, parse_end);

    EXPECT_EQ('a', p.value());

    p.clear();
    EXPECT_TRUE(p);
    EXPECT_NE(parse_end, p);
    *p = '/';
    EXPECT_FALSE(p);
    EXPECT_EQ(parse_end, p);
    EXPECT_EQ(p, parse_end);
    EXPECT_TRUE(p.failed());
}

using phcar_pct_ucs_parser = detail::alternatives_parser<
            path_char_parser,
            pct_encoded_sequence_parser,
            hex_encoded_ucs_parser
        >;

using path_segment_parser =
        detail::repetition_parser<phcar_pct_ucs_parser, ::std::string>;

TEST(Parser, EscapedClassParser)
{
    path_segment_parser p;
    ::std::string test_str{ "aBcdd%20bla%xc990" };
    EXPECT_TRUE(p);
    EXPECT_NE(parse_end, p);
    p.parse(test_str.begin(), test_str.end());
    EXPECT_FALSE(p);
    EXPECT_EQ(parse_end, p);
    EXPECT_EQ(p, parse_end);
    EXPECT_FALSE(p.failed());
    EXPECT_TRUE(p.done());
    EXPECT_EQ("aBcdd blaɐ", p.value());
}

} /* namespace test */
} /* namespace iri */
} /* namespace tip */

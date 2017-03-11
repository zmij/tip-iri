/*
 * literal_parser_test.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: zmij
 */

#include <gtest/gtest.h>
#include <tip/iri/detail/char_class.hpp>

namespace tip {
namespace iri {
namespace test {

namespace {

constexpr char slashslash[] = "//";
constexpr char lorem[]      = "lorem";

} /* namespace  */


TEST(Parser, LiteralParser)
{
    using slash_parser = literal_str_parser< slashslash >;
    using lorem_parser = literal_str_parser< lorem >;

    slash_parser sp;
    EXPECT_TRUE(sp.empty());
    EXPECT_TRUE(sp.want_more());
    sp.parse(slashslash, slashslash + 2);
    EXPECT_TRUE(sp.done());
    EXPECT_FALSE(sp.failed());
    EXPECT_FALSE(sp.want_more());

    sp.clear();
    EXPECT_TRUE(sp.empty());
    EXPECT_TRUE(sp.want_more());
    sp.parse(lorem, lorem + 5);
    EXPECT_FALSE(sp.done());
    EXPECT_TRUE(sp.failed());
    EXPECT_FALSE(sp.want_more());

    lorem_parser lp;
    EXPECT_TRUE(lp.empty());
    EXPECT_TRUE(lp.want_more());
    lp.parse(lorem, lorem + 5);
    EXPECT_TRUE(lp.done());
    EXPECT_FALSE(lp.failed());
    EXPECT_FALSE(lp.want_more());

    lp.clear();
    EXPECT_TRUE(lp.empty());
    EXPECT_TRUE(lp.want_more());
    lp.parse(slashslash, slashslash + 2);
    EXPECT_FALSE(lp.done());
    EXPECT_TRUE(lp.failed());
    EXPECT_FALSE(lp.want_more());
}

TEST(Parser, LiteralSeqParser)
{
    using slash_parser = literal_parser< '/', '/' >;
    slash_parser sp;
    EXPECT_TRUE(sp.empty());
    EXPECT_TRUE(sp.want_more());
    sp.parse(slashslash, slashslash + 2);
    EXPECT_TRUE(sp.done());
    EXPECT_FALSE(sp.failed());
    EXPECT_FALSE(sp.want_more());

    sp.clear();
    EXPECT_TRUE(sp.empty());
    EXPECT_TRUE(sp.want_more());
    sp.parse(lorem, lorem + 5);
    EXPECT_FALSE(sp.done());
    EXPECT_TRUE(sp.failed());
    EXPECT_FALSE(sp.want_more());
}

TEST(Parser, LiteralCharParser)
{
    using slash_parser = literal_parser< '/' >;
    slash_parser sp;
    EXPECT_TRUE(sp.empty());
    EXPECT_TRUE(sp.want_more());
    sp.parse(slashslash, slashslash + 2);
    EXPECT_TRUE(sp.done());
    EXPECT_FALSE(sp.failed());
    EXPECT_FALSE(sp.want_more());

    sp.clear();
    EXPECT_TRUE(sp.empty());
    EXPECT_TRUE(sp.want_more());
    sp.parse(lorem, lorem + 5);
    EXPECT_FALSE(sp.done());
    EXPECT_TRUE(sp.failed());
    EXPECT_FALSE(sp.want_more());
}


} /* namespace test */

} /* namespace iri */
} /* namespace tip */


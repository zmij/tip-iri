/*
 * int_parser_test.cpp
 *
 *  Created on: Mar 10, 2017
 *      Author: zmij
 */

#include <gtest/gtest.h>
#include <tip/iri/parsers/int_parser.hpp>
#include <string>

namespace tip {
namespace iri {

inline namespace v2 {
namespace detail {

void
PrintTo(parser_state s, ::std::ostream* os)
{
    switch (s) {
    case parser_state::empty:
        *os << "empty";
        break;
    case parser_state::in_progress:
        *os << "in_progress";
        break;
    case parser_state::done:
        *os << "done";
        break;
    case parser_state::failed:
        *os << "failed";
        break;
    }
}

} /* namespace detail */
} /* namespace v2 */



namespace test {

TEST(IntParse, Dec3)
{
    using parser_type = uint_parser<::std::uint32_t, 10, 3>;

    {
        char const* test_str = "127";
        parser_type p;
        EXPECT_TRUE(p.want_more());
        EXPECT_TRUE(p.empty());
        for (auto c = test_str; *c != 0; ++c) {
            auto s = p.feed_char(*c);
            if (c - test_str < 2)
                EXPECT_EQ(parser_type::parser_state::in_progress, s.first)
                    << "In progress at " << *c;
            else
                EXPECT_EQ(parser_type::parser_state::done, s.first)
                    << "Done at " << *c;
        }
        EXPECT_TRUE(p.done());
        EXPECT_FALSE(p.failed());
        EXPECT_FALSE(p.want_more());
        EXPECT_EQ(127, p.value());
    }
    {
        parser_type p;
        EXPECT_TRUE(p.want_more());
        EXPECT_TRUE(p.empty());
        EXPECT_EQ(parser_type::parser_state::failed, p.feed_char('a').first);
        EXPECT_FALSE(p.done());
        EXPECT_TRUE(p.failed());
        EXPECT_FALSE(p.want_more());
        p.clear();
        EXPECT_TRUE(p.want_more());
        EXPECT_TRUE(p.empty());
    }
    {
        char const* test_str = "12b";
        parser_type p;
        EXPECT_TRUE(p.want_more());
        EXPECT_TRUE(p.empty());
        for (auto c = test_str; *c != 0; ++c) {
            auto s = p.feed_char(*c);
            if (c - test_str < 2)
                EXPECT_EQ(parser_type::parser_state::in_progress, s.first)
                    << "In progress at " << *c;
            else
                EXPECT_EQ(parser_type::parser_state::done, s.first)
                    << "Done at " << *c;
        }
        EXPECT_TRUE(p.done());
        EXPECT_FALSE(p.failed());
        EXPECT_FALSE(p.want_more());
        EXPECT_EQ(12, p.value());
    }
    {
        char const* test_str = "1278";
        parser_type p;
        EXPECT_TRUE(p.want_more());
        EXPECT_TRUE(p.empty());
        p.parse(test_str, test_str + 3);
        EXPECT_TRUE(p.done());
        EXPECT_FALSE(p.failed());
        EXPECT_FALSE(p.want_more());
        EXPECT_EQ(127, p.value());
    }

}

TEST(IntParse, Hex4)
{
    using parser_type = uint_parser<::std::uint32_t, 16, 4>;
    {
        char const* test_str = "a127";
        parser_type p;
        EXPECT_TRUE(p.want_more());
        EXPECT_TRUE(p.empty());
        for (auto c = test_str; *c != 0; ++c) {
            auto s = p.feed_char(*c);
            if (c - test_str < 3)
                EXPECT_EQ(parser_type::parser_state::in_progress, s.first)
                    << "In progress at " << *c;
            else
                EXPECT_EQ(parser_type::parser_state::done, s.first)
                    << "Done at " << *c;
        }
        EXPECT_TRUE(p.done());
        EXPECT_FALSE(p.failed());
        EXPECT_FALSE(p.want_more());
        EXPECT_EQ(0xa127, p.value());
    }

    {
        ::std::string test_str{"aabbff"};
        parser_type p;
        EXPECT_TRUE(p.want_more());
        EXPECT_TRUE(p.empty());

        p.parse(test_str.begin(), test_str.end());
        EXPECT_TRUE(p.done());
        EXPECT_FALSE(p.failed());
        EXPECT_FALSE(p.want_more());
        EXPECT_EQ(0xaabb, p.value());
    }
}


} /* namespace test */
} /* namespace iri */
} /* namespace tip */


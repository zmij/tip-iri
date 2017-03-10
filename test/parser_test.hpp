/*
 * parser_test.hpp
 *
 *  Created on: Mar 10, 2017
 *      Author: zmij
 */

#ifndef TEST_PARSER_TEST_HPP_
#define TEST_PARSER_TEST_HPP_


#include <gtest/gtest.h>

namespace parser_test {

template < typename Parser, typename Result >
class ParserTest :
        public ::testing::TestWithParam< ::std::pair< ::std::string, Result > > {
protected:
    using parser_type = Parser;
    parser_type parser;
public:
    using result_type   = Result;
    using param_type    = ::std::pair< ::std::string, result_type >;

    template < typename T >
    static param_type
    make_test_data(::std::string const& input, T&& data)
    {
        return param_type{ input, result_type(::std::forward<T>(data)) };
    }
    static param_type
    make_test_data(::std::string const& input)
    {
        return param_type{ input, result_type{} };
    }
};

} /* namespace test */


#define PARSER_TEST(parser_name, test_name, valid_generator, invalid_generator) \
using Parse##test_name = ::parser_test::ParserTest< parser_name, parser_name::value_type >; \
using Valid_##test_name = Parse##test_name; \
using Invalid_##test_name = Parse##test_name; \
TEST_P(Valid_##test_name, ValidInput) \
{ \
    auto param = GetParam(); \
    EXPECT_TRUE(parser.empty()); \
    EXPECT_TRUE(parser.want_more()); \
    parser.parse(param.first.begin(), param.first.end()); \
    EXPECT_TRUE(parser.done()) << "Done with " << param.first; \
    EXPECT_FALSE(parser.failed()) << "Not failed with " << param.first; \
    EXPECT_FALSE(parser.want_more()) << "Finished with " << param.first; \
    EXPECT_EQ(param.second, parser.value()) << "Correct result of parsing " << param.first; \
} \
INSTANTIATE_TEST_CASE_P(ParserTest, Valid_##test_name, valid_generator); \
TEST_P(Invalid_##test_name, InvalidInput) \
{ \
    auto param = GetParam(); \
    EXPECT_TRUE(parser.empty()); \
    EXPECT_TRUE(parser.want_more()); \
    parser.parse(param.first.begin(), param.first.end()); \
    EXPECT_TRUE(parser.failed()) << "Unexpectedly parsed " << param.first \
            << " in parser " << parser.value(); \
    EXPECT_FALSE(parser.want_more()); \
} \
INSTANTIATE_TEST_CASE_P(ParserTest, Invalid_##test_name, invalid_generator);

#endif /* TEST_PARSER_TEST_HPP_ */

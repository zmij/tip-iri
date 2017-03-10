/*
 * char_classes.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: zmij
 */

#include <gtest/gtest.h>
#include <tip/iri/detail/char_classes.hpp>

namespace tip {
namespace iri {
namespace test {

namespace {

constexpr char sub_delims[]   = "!$&'()*+,;=";
constexpr char gen_delims[]   = ":/?#[]@";

} /* namespace  */


TEST(Parse, Schema)
{
    ::std::string test_str = "http";

}

} /* namespace test */
} /* namespace iri */
} /* namespace tip */


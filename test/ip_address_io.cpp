/*
 * ip_address_io.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: sergey.fedorov
 */

#include <gtest/gtest.h>
#include <tip/iri/detail/ip_address_parsers.hpp>
#include <iostream>
#include <string>

#include "parser_test.hpp"

namespace tip {
namespace iri {

void
PrintTo(ipv6_address const& val, ::std::ostream* os)
{
    *os << val;
}

namespace test {

TEST(IPAddr, IPv4)
{
    EXPECT_EQ("0.0.0.0", to_string(ipv4_address{}));
    EXPECT_EQ("127.0.0.1", to_string(ipv4_localhost));
    EXPECT_EQ("255.255.255.0", to_string(ipv4_address{ 0xffffff00 }));

    EXPECT_EQ(ipv4_localhost, ipv4_address{"127.0.0.1"});
    EXPECT_EQ(ipv4_address{0xffffffff}, ipv4_address{"255.255.255.255"});
    EXPECT_EQ(ipv4_address{0xff00ffff}, ipv4_address{"255.0.255.255"});
    EXPECT_EQ(ipv4_address{0x10203040}, ipv4_address{"16.32.48.64"});
    EXPECT_EQ(ipv4_address{0u}, ipv4_address{"0.0.0.0"});
}

using ipv4_parser = parser<iri_part::ipv4_address>;
PARSER_TEST(ipv4_parser, IPv4,
    ::testing::Values(
        ParseIPv4::make_test_data( "0.0.0.0",   ipv4_address{} ),
        ParseIPv4::make_test_data( "127.0.0.1", ipv4_localhost ),
        ParseIPv4::make_test_data( "255.255.0.1", ipv4_address{ 0xffff0001 } ),
        ParseIPv4::make_test_data( "127.0.0.1:100", ipv4_localhost ),
        ParseIPv4::make_test_data( "127.0.0.1/foo", ipv4_localhost )
    ),
    ::testing::Values(
        ParseIPv4::make_test_data("127.0.0.256"),
        ParseIPv4::make_test_data("255.255.2.387"),
        ParseIPv4::make_test_data("127.0.00.1"),
        ParseIPv4::make_test_data("192.0..33"),
        ParseIPv4::make_test_data("192.0.33."),
        ParseIPv4::make_test_data("192.0.3"),
        ParseIPv4::make_test_data("192:0:3")
    )
);

TEST(IPAddr, IPv6Construct)
{
    EXPECT_EQ(16, sizeof(ipv6_address));
    EXPECT_EQ(ipv6_localhost, (ipv6_address( {}, {1} )));
}

TEST(IPAddr, IPv6)
{
    EXPECT_EQ("::1", to_string(ipv6_localhost));
    EXPECT_EQ("::", to_string(ipv6_address{}));
    EXPECT_EQ("1:1:1:1:1:1:0:1", to_string(ipv6_address{1, 1, 1, 1, 1, 1, 0, 1}));
    EXPECT_EQ("aa::bb:0:0:1", to_string(ipv6_address{{0xaa}, {0xbb, 0, 0, 1 }}));
    EXPECT_EQ("aa:0:0:bb::1", to_string(ipv6_address{{0xaa}, {0xbb, 0, 0, 0, 1 }}));
    EXPECT_EQ("::ffff:127.0.0.1", to_string(ipv6_address{ipv4_localhost}));
}

TEST(IPAddr, IPv6Ops)
{
    EXPECT_EQ((ipv6_address{{}, {1, 0}}), (ipv6_localhost << 16));
    EXPECT_EQ((ipv6_address({}, {0x0100})), (ipv6_localhost << 8));
    EXPECT_EQ((ipv6_address{{}, {0x0100, 0}}), (ipv6_localhost << 24));

    EXPECT_EQ((ipv6_address({}, {1})), (ipv6_address{{}, {1, 0}} >> 16));
    EXPECT_EQ((ipv6_address({}, {1})), (ipv6_address({}, {0x0100}) >> 8));
    EXPECT_EQ((ipv6_address({}, {1})), (ipv6_address({}, {0x0100, 0}) >> 24));

    EXPECT_EQ((ipv6_address{{0xaa, 0xff}, {0xbb, 0xcc}}),
            (ipv6_address{{0xaa}, {0xbb, 0}} | ipv6_address{{0, 0xff}, {0xcc}}));
}

using ipv6_parser = parser<iri_part::ipv6_address>;
PARSER_TEST(ipv6_parser, IPv6,
    ::testing::Values(
        ParseIPv6::make_test_data( "::",   ipv6_address{} ),
        ParseIPv6::make_test_data( "::1", ipv6_localhost ),
        ParseIPv6::make_test_data( "1:2:3:4:5:6:7:8",   ipv6_address{ 1, 2, 3, 4, 5, 6, 7, 8 } ),
        //ParseIPv6::make_test_data( "1:2:3:4:5:6:7::",   ipv6_address{ 1, 2, 3, 4, 5, 6, 7, 0 } ),
        ParseIPv6::make_test_data( "1:2:3:4:5:6::",     ipv6_address{ 1, 2, 3, 4, 5, 6, 0, 0 } ),
        ParseIPv6::make_test_data( "1:2:3:4:5::",       ipv6_address{ 1, 2, 3, 4, 5, 0, 0, 0 } ),
        ParseIPv6::make_test_data( "1:2:3:4::",         ipv6_address{ 1, 2, 3, 4, 0, 0, 0, 0 } ),
        ParseIPv6::make_test_data( "1:2:3::",           ipv6_address{ 1, 2, 3, 0, 0, 0, 0, 0 } ),
        ParseIPv6::make_test_data( "1:2::",             ipv6_address{ 1, 2, 0, 0, 0, 0, 0, 0 } ),
        ParseIPv6::make_test_data( "1::",               ipv6_address{ 1, 0, 0, 0, 0, 0, 0, 0 } ),

        ParseIPv6::make_test_data(       "1::8",        ipv6_address{ 1, 0, 0, 0, 0, 0, 0, 8 } ),
        ParseIPv6::make_test_data(     "1:2::8",        ipv6_address{ 1, 2, 0, 0, 0, 0, 0, 8 } ),
        ParseIPv6::make_test_data(     "1:2::7:8",      ipv6_address{ 1, 2, 0, 0, 0, 0, 7, 8 } ),
        ParseIPv6::make_test_data(   "1:2:3::7:8",      ipv6_address{ 1, 2, 3, 0, 0, 0, 7, 8 } ),
        ParseIPv6::make_test_data(   "1:2:3::6:7:8",    ipv6_address{ 1, 2, 3, 0, 0, 6, 7, 8 } ),

        // Not valid because less than two zero sections skipped
        // skipped less than two zeros, parser will drop the last hextet
        ParseIPv6::make_test_data("1:2:3:4::6:7:8",     ipv6_address{ 1, 2, 3, 4, 0, 0, 6, 7 } ),

        // Not valid because less than two zero sections skipped
        // skipped less than two zeros, parser will drop the last hextet
        ParseIPv6::make_test_data( "::2:3:4:5:6:7:8",   ipv6_address{ 0, 0, 2, 3, 4, 5, 6, 7 } ),
        ParseIPv6::make_test_data( "0:2:3:4:5:6:7:8",   ipv6_address{ 0, 2, 3, 4, 5, 6, 7, 8 } ),
        ParseIPv6::make_test_data(   "::3:4:5:6:7:8",   ipv6_address{ 0, 0, 3, 4, 5, 6, 7, 8 } ),
        ParseIPv6::make_test_data(     "::4:5:6:7:8",   ipv6_address{ 0, 0, 0, 4, 5, 6, 7, 8 } ),
        ParseIPv6::make_test_data(       "::5:6:7:8",   ipv6_address{ 0, 0, 0, 0, 5, 6, 7, 8 } ),
        ParseIPv6::make_test_data(         "::6:7:8",   ipv6_address{ 0, 0, 0, 0, 0, 6, 7, 8 } ),
        ParseIPv6::make_test_data(           "::7:8",   ipv6_address{ 0, 0, 0, 0, 0, 0, 7, 8 } ),
        ParseIPv6::make_test_data(             "::8",   ipv6_address{ 0, 0, 0, 0, 0, 0, 0, 8 } ),

        // IPv4 in IPv6 according to RFC
        ParseIPv6::make_test_data(  "::ffff:8.8.8.8",   ipv6_address{ 0, 0, 0, 0, 0, 0xffff, 0x0808, 0x0808 } ),
        ParseIPv6::make_test_data(  "::ffff:127.0.0.1", ipv6_address{ 0, 0, 0, 0, 0, 0xffff, 0x7f00, 0x0001 } ),

        // Misc IPv4 in IPv6
        ParseIPv6::make_test_data(  "::127.0.0.1",          ipv6_address{ 0, 0, 0, 0, 0, 0, 0x7f00, 0x0001 } ),
        ParseIPv6::make_test_data( "::6:127.0.0.1",         ipv6_address{ 0, 0, 0, 0, 0, 6, 0x7f00, 0x0001 } ),
        ParseIPv6::make_test_data( "::5:6:127.0.0.1",       ipv6_address{ 0, 0, 0, 0, 5, 6, 0x7f00, 0x0001 } ),
        ParseIPv6::make_test_data( "::4:5:6:127.0.0.1",     ipv6_address{ 0, 0, 0, 4, 5, 6, 0x7f00, 0x0001 } ),
        ParseIPv6::make_test_data( "::3:4:5:6:127.0.0.1",   ipv6_address{ 0, 0, 3, 4, 5, 6, 0x7f00, 0x0001 } ),
        ParseIPv6::make_test_data( "1:2:3:4:5:6:127.0.0.1", ipv6_address{ 1, 2, 3, 4, 5, 6, 0x7f00, 0x0001 } ),

        ParseIPv6::make_test_data( "::1/foo", ipv6_localhost )
    ),
    ::testing::Values(
        ParseIPv6::make_test_data("127.0.0.255"),
        ParseIPv6::make_test_data("1::1::1"),
        ParseIPv6::make_test_data("1::1:"),
        ParseIPv6::make_test_data("1:::"),
        ParseIPv6::make_test_data("1:1:1:"),
        ParseIPv6::make_test_data("1:1"),
        ParseIPv6::make_test_data("1:2:3:4:5:6:7::"), // skipped less than two zeros
        //ParseIPv6::make_test_data("::2:3:4:5:6:7:8"), // skipped less than two zeros, parser will drop the last hextet
        //ParseIPv6::make_test_data("1:2:3:4::6:7:8"), // skipped less than two zeros, parser will drop the last hextet
        ParseIPv6::make_test_data("::ffff:127.00.0.1"),
        ParseIPv6::make_test_data("::ffff:327.0.0.1"),
        ParseIPv6::make_test_data( "1:2:3:4:5:6:7:127.0.0.1"),
        //ParseIPv6::make_test_data( "::2:3:4:5:6:7:127.0.0.1"), // parser will drop the ipv4 part
        ParseIPv6::make_test_data("192.0.33."),
        ParseIPv6::make_test_data("192.0.3"),
        ParseIPv6::make_test_data("192:0:3")
    )
);

using ip_lit_parser = parser<iri_part::ip_literal>;
PARSER_TEST(ip_lit_parser, IPLiteral,
    ::testing::Values(
        ParseIPLiteral::make_test_data( "[::]",   ipv6_address{} ),
        ParseIPLiteral::make_test_data( "[::1]", ipv6_localhost ),
        ParseIPLiteral::make_test_data( "[1:2:3:4:5:6:7:8]",    ipv6_address{ 1, 2, 3, 4, 5, 6, 7, 8 } ),
        //ParseIPv6::make_test_data( "1:2:3:4:5:6:7::",   ipv6_address{ 1, 2, 3, 4, 5, 6, 7, 0 } ),
        ParseIPLiteral::make_test_data( "[1:2:3:4:5:6::]",      ipv6_address{ 1, 2, 3, 4, 5, 6, 0, 0 } ),
        ParseIPLiteral::make_test_data( "[1:2:3:4:5::]",        ipv6_address{ 1, 2, 3, 4, 5, 0, 0, 0 } ),
        ParseIPLiteral::make_test_data( "[1:2:3:4::]",          ipv6_address{ 1, 2, 3, 4, 0, 0, 0, 0 } ),
        ParseIPLiteral::make_test_data( "[1:2:3::]",            ipv6_address{ 1, 2, 3, 0, 0, 0, 0, 0 } ),
        ParseIPLiteral::make_test_data( "[1:2::]",              ipv6_address{ 1, 2, 0, 0, 0, 0, 0, 0 } ),
        ParseIPLiteral::make_test_data( "[1::]",                ipv6_address{ 1, 0, 0, 0, 0, 0, 0, 0 } ),

        ParseIPLiteral::make_test_data( "[::ffff:8.8.8.8]",     ipv6_address{ 0, 0, 0, 0, 0, 0xffff, 0x0808, 0x0808 } )
    ),
    ::testing::Values(
        ParseIPLiteral::make_test_data("::1]"),
        ParseIPLiteral::make_test_data("[::"),
        ParseIPLiteral::make_test_data("127.0.0.256"),
        ParseIPLiteral::make_test_data("[::255.255.2.387]"),
        ParseIPLiteral::make_test_data("[255.255.2.387]"),
        ParseIPLiteral::make_test_data("127.0.00.1"),
        ParseIPLiteral::make_test_data("192.0..33"),
        ParseIPLiteral::make_test_data("192.0.33."),
        ParseIPLiteral::make_test_data("192.0.3"),
        ParseIPLiteral::make_test_data("192.0.3"),
        ParseIPLiteral::make_test_data("192:0:3")
    )
);

} // namespace test
}  /* namespace iri */
}  /* namespace tip */

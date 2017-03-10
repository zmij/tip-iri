/*
 * ip_address_io.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: sergey.fedorov
 */

#include <gtest/gtest.h>
#include <tip/iri/ip_address.hpp>
#include <iostream>

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

} // namespace test
}  /* namespace iri */
}  /* namespace tip */

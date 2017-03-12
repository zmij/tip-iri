/*
 * reg_name_parse_test.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: zmij
 */

#include <gtest/gtest.h>
#include <tip/iri/parsers/authority_parser.hpp>
#include <tip/iri/parsers/reg_name_parser.hpp>
#include "parser_test.hpp"

namespace tip {
namespace iri {

inline namespace v2 {


} /* namespace v2 */


namespace test {

using namespace std::literals;

using schema_parser = parser< iri_part::schema >;
PARSER_TEST(schema_parser, Schema,
    ::testing::Values(
        ParseSchema::make_test_data( "http", "http" ),
        ParseSchema::make_test_data( "https", "https" ),
        ParseSchema::make_test_data( "h2", "h2" ),
        ParseSchema::make_test_data( "ftp", "ftp" ),
        ParseSchema::make_test_data( "mailto", "mailto" ),
        ParseSchema::make_test_data( "proto+ssl", "proto+ssl" ),
        ParseSchema::make_test_data( "proto-tls", "proto-tls" ),
        ParseSchema::make_test_data( "proto.ssl", "proto.ssl" )
    ),
    ::testing::Values(
        ParseSchema::make_test_data( "" ),
        ParseSchema::make_test_data( "1http" )
    )
);

using reg_name_parser = parser<iri_part::reg_name>;
PARSER_TEST(reg_name_parser, RegName,
    ::testing::Values(
        ParseRegName::make_test_data( "google.com", "google.com" ),
        ParseRegName::make_test_data( "mail.ru", "mail.ru" ),
        ParseRegName::make_test_data( "some%20name", "some name" ),
        ParseRegName::make_test_data( "%xd181%xd0b0%xd0b9%xd182.%xd180%xd184", "сайт.рф" )
    ),
    ::testing::Values(
        ParseRegName::make_test_data( "" ),
        ParseRegName::make_test_data( "%" ),
        ParseRegName::make_test_data( "%a" ),
        ParseRegName::make_test_data( "%x" ),
        ParseRegName::make_test_data( "%r" ),
        ParseRegName::make_test_data( "[::]" ),
        ParseRegName::make_test_data( " " )
    )
);


using host_parser = parser<iri_part::host>;
PARSER_TEST(host_parser, Host,
    ::testing::Values(
        ParseHost::make_test_data( "google.com", "google.com"s ),
        ParseHost::make_test_data( "127.0.0.1", ipv4_localhost),
        ParseHost::make_test_data("[::1]", ipv6_localhost)
    ),
    ::testing::Values(
        ParseHost::make_test_data( "" ),
        ParseHost::make_test_data( " " )
    )
);

using authority_parser = parser<iri_part::authority>;
PARSER_TEST(authority_parser, Authority,
    ::testing::Values(
        ParseAuthority::make_test_data("[::1]:8080",        host{ipv6_localhost}, 8080),
        ParseAuthority::make_test_data("google.com",        host{"google.com"s}, 0),
        ParseAuthority::make_test_data("google.com:80",     host{"google.com"s}, 80),
        ParseAuthority::make_test_data("localhost:8080",    host{"localhost"s}, 8080),
        ParseAuthority::make_test_data("127.0.0.1:8080",    host{ipv4_localhost}, 8080),
        ParseAuthority::make_test_data("127.0.0.1",         host{ipv4_localhost}, 0),
        ParseAuthority::make_test_data("[::1]",             host{ipv6_localhost}, 0)
    ),
    ::testing::Values(
        ParseAuthority::make_test_data("127.0.0.1:/"),
        ParseAuthority::make_test_data( "[]" ),
        ParseAuthority::make_test_data( "" ),
        ParseAuthority::make_test_data( ":80" ),
        ParseAuthority::make_test_data( " " )
    )
);

} /* namespace test */
} /* namespace iri */
} /* namespace tip */


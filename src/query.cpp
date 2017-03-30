/*
 * query.cpp
 *
 *  Created on: Mar 30, 2017
 *      Author: zmij
 */

#include <tip/iri/query.hpp>
#include <tip/iri/detail/iri_part.hpp>
#include <tip/iri/detail/escape_string.hpp>
#include <tip/iri/parsers/query_parser.hpp>

#include <iostream>
#include <sstream>

namespace tip {
namespace iri {
inline namespace v2 {

query
query::parse(::std::string const& s)
{
    parser<iri_part::query> parser;

    parser.parse(s.begin(), s.end());
    if (parser.failed())
        throw ::std::runtime_error{ "IRI: Query parsing failed" };
    return parser.value();
}

::std::ostream&
operator <<(::std::ostream& os, query const& val)
{
    using escaper = char_escaper< iri_part, iri_part::query >;
    ::std::ostream::sentry s(os);
    if (s) {
        if (!val.empty()) {
            os << '?';
            for (auto p = val.begin(); p != val.end(); ++p) {
                if (p != val.begin())
                    os << '&';
                escaper::escape(os, p->first.begin(), p->first.end());
                os << '=';
                escaper::escape(os, p->second.begin(), p->second.end());
            }
        }
    }
    return os;
}

::std::string
to_string(query const& val)
{
    ::std::ostringstream os;
    os << val;
    return os.str();
}

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */


/*
 * path.cpp
 *
 *  Created on: Mar 30, 2017
 *      Author: zmij
 */

#include <tip/iri/path.hpp>
#include <tip/iri/detail/iri_part.hpp>
#include <tip/iri/detail/escape_string.hpp>
#include <iostream>
#include <sstream>

namespace tip {
namespace iri {
inline namespace v2 {

::std::ostream&
operator <<(::std::ostream& os, path const& val)
{
    using escaper = char_escaper< iri_part, iri_part::path >;
    ::std::ostream::sentry s(os);
    if (s) {
        if (val.is_rooted())
            os << '/';
        for (auto p = val.begin(); p != val.end(); ++p) {
            if (p != val.begin())
                os << '/';
            escaper::escape(os, p->begin(), p->end());
        }
    }
    return os;
}

::std::string
to_string(path const& val)
{
    ::std::ostringstream os;
    os << val;
    return os.str();
}

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */

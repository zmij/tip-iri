/*
 * query.hpp
 *
 *  Created on: Mar 30, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_QUERY_HPP_
#define TIP_IRI_QUERY_HPP_

#include <string>
#include <map>

namespace tip {
namespace iri {
inline namespace v2 {

struct query : ::std::multimap<::std::string, ::std::string> {
    using base_type = ::std::multimap<::std::string, ::std::string>;
    using base_type::base_type;

    static query
    parse(::std::string const&);
};

::std::ostream&
operator <<(::std::ostream& os, query const& val);
::std::string
to_string(query const&);

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_QUERY_HPP_ */

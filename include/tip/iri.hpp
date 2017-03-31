/*
 * iri.hpp
 *
 *  Created on: Aug 17, 2015
 *      Author: zmij
 */

#ifndef TIP_IRI_IRI_HPP_
#define TIP_IRI_IRI_HPP_

#include <string>
#include <vector>
#include <iosfwd>

#include <tip/iri/ip_address.hpp>
#include <tip/iri/host.hpp>
#include <tip/iri/authority.hpp>
#include <tip/iri/path.hpp>
#include <tip/iri/query.hpp>

namespace tip {
namespace iri {

inline namespace v2 {

} /* namespace v2 */


class scheme : public std::string {
public:
    scheme() : std::string() {}
    explicit
    scheme(std::string const& s) : std::string(s) {}
};


//template < typename QueryType = query >
//struct basic_iri {
//    typedef QueryType query_type;
//    class scheme    scheme;
//    class authority    authority;
//    class path        path;
//    query_type        query;
//    class fragment    fragment;
//
//    bool
//    operator == (basic_iri const& rhs) const
//    { return scheme == rhs.scheme && authority == rhs.authority &&
//            path == rhs.path && query == rhs.query && fragment == rhs.fragment;
//    }
//    bool
//    empty() const
//    { return authority.empty() && path.empty(); }
//};
//
//using iri = basic_iri<>;
//
//iri
//parse_iri(std::string const&);

}  // namespace iri
}  // namespace tip

#endif /* TIP_IRI_IRI_HPP_ */

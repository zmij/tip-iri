/*
 * authority.hpp
 *
 *  Created on: Mar 31, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_AUTHORITY_HPP_
#define TIP_IRI_AUTHORITY_HPP_

#include <tip/iri/host.hpp>
#include <boost/optional.hpp>

namespace tip {
namespace iri {
inline namespace v2 {

struct userinfo {
    ::std::string   user;
    ::std::string   password;

    bool operator ==(userinfo const& rhs) const
    { return user == rhs.user && password == rhs.password; }

    bool operator !=(userinfo const& rhs) const
    { return !(*this == rhs); }

    bool
    empty() const
    { return user.empty(); }
};

::std::ostream&
operator <<(::std::ostream& os, userinfo const& val);

struct authority {
    using opt_userinfo = ::boost::optional<userinfo>;
    opt_userinfo    user_info;
    struct host     host;
    ::std::uint16_t port;
};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */


#endif /* TIP_IRI_AUTHORITY_HPP_ */

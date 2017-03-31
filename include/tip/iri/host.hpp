/*
 * host.hpp
 *
 *  Created on: Mar 11, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_HOST_HPP_
#define TIP_IRI_HOST_HPP_

#include <boost/variant.hpp>
#include <tip/iri/ip_address.hpp>
#include <string>
#include <iosfwd>

namespace tip {
namespace iri {
inline namespace v2 {

struct host {
    using data_type = ::boost::variant<ipv4_address, ipv6_address, ::std::string>;
    enum class type {
        ipv4,
        ipv6,
        reg_name
    };

    host() : data {} {}

    explicit
    host(data_type const& val) : data{ val } {}

    bool operator ==(host const& rhs) const
    {
        return data == rhs.data;
    }

    bool operator !=(host const& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator <(host const& rhs) const
    {
        return data < rhs.data;
    }

    /**
     * Get a string suitable for lookup. IPv6 will be output as is.
     * @return
     */
    ::std::string
    lookup_string() const;

    type
    address_type() const
    { return static_cast<type>(data.which()); }

    bool
    empty() const;
private:
    /**
     * Will output contents of data. IPv6 address will be output as an IP literal
     * (in square brackets)
     * @param os
     * @param val
     * @return
     */
    friend ::std::ostream&
    operator <<(::std::ostream& os, host const& val);
    friend ::std::string
    to_string(host const&);

    data_type  data;
};

::std::ostream&
operator <<(::std::ostream& os, host::type const& val);

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_HOST_HPP_ */

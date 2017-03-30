/*
 * path.hpp
 *
 *  Created on: Mar 30, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_PATH_HPP_
#define TIP_IRI_PATH_HPP_

#include <vector>
#include <string>

namespace tip {
namespace iri {
inline namespace v2 {

class path : public ::std::vector< ::std::string > {
public:
    using base_type = ::std::vector< ::std::string >;

    path() : base_type(), rooted_(false) {}
    path(bool rooted) : base_type(), rooted_(rooted) {}
    path(bool rooted, ::std::initializer_list<::std::string> args) :
        base_type(args), rooted_(rooted) {}

    path(path const& rhs)
        : base_type(rhs), rooted_{rhs.rooted_} {}
    path(path&& rhs)
        : base_type(::std::move(rhs)), rooted_{rhs.rooted_} {}

    path&
    operator = (path const& rhs)
    {
        path{rhs}.swap(*this);
        return *this;
    }

    path&
    operator = (path&& rhs)
    {
        swap(rhs);
        return *this;
    }

    void
    swap(path& rhs)
    {
        ::std::swap(rooted_, rhs.rooted_);
        base_type::swap(rhs);
    }

    bool
    operator == (path const& rhs) const
    {
        return rooted_ == rhs.rooted_ &&
                static_cast<base_type const&>(*this) == static_cast<base_type const&>(rhs);
    }
    bool
    is_rooted() const
    { return rooted_; }
    bool&
    is_rooted()
    { return rooted_; }

    static path
    parse(::std::string const&);
private:
    bool rooted_;
};

::std::ostream&
operator <<(::std::ostream& os, path const& val);
::std::string
to_string(path const&);

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */



#endif /* TIP_IRI_PATH_HPP_ */

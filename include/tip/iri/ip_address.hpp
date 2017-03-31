/*
 * ip_address.cpp
 *
 *  Created on: Mar 9, 2017
 *      Author: sergey.fedorov
 */

#ifndef TIP_IRI_IP_ADDRESS_HPP_
#define TIP_IRI_IP_ADDRESS_HPP_

#include <iosfwd>
#include <string>
#include <cstdint>
#include <stdexcept>

namespace tip {
namespace iri {

inline namespace v2 {

struct ipv4_address {
    using repr = ::std::uint32_t;
    repr data;

    constexpr
    ipv4_address() : data{0} {}

    constexpr explicit
    ipv4_address(repr d) : data{d} {}

    constexpr explicit
    ipv4_address(char const* str);

    constexpr bool
    operator == (ipv4_address const& rhs) const
    { return data == rhs.data; }
    constexpr bool
    operator < (ipv4_address const& rhs) const
    { return data < rhs.data; }

    constexpr bool
    empty() const
    { return data == 0; }
};

::std::ostream&
operator << (::std::ostream& os, ipv4_address const& val);
::std::string
to_string(ipv4_address const& val);

inline constexpr ipv4_address
operator << (ipv4_address addr, ::std::size_t bits)
{
    return ipv4_address{ addr.data << bits };
}

inline constexpr ipv4_address
operator >> (ipv4_address addr, ::std::size_t bits)
{
    return ipv4_address{ addr.data >> bits };
}

inline constexpr ipv4_address
operator | (ipv4_address lhs, ipv4_address rhs)
{
    return ipv4_address{ lhs.data | rhs.data };
}

inline constexpr ipv4_address
operator & (ipv4_address lhs, ipv4_address rhs)
{
    return ipv4_address{ lhs.data & rhs.data };
}

struct ipv6_address {
    using element           = ::std::uint16_t;
    static constexpr auto element_size = sizeof(element) * 8;
    static constexpr auto size = 128 / element_size;

    using repr              = element[ size ];
    using init_list         = ::std::initializer_list<element>;
    using iterator          = element*;
    using const_iterator    = element const*;

    static constexpr repr empty_addr{0};

    repr data;

    constexpr
    ipv6_address() : data{0} {}

    constexpr
    ipv6_address(init_list const& d)
        : data{0}
    {
        if (d.size() == 1) {
            for (auto& e : data) {
                e = *d.begin();
            }
        } else if (d.size() == size) {
            auto p = data;
            for (auto e : d) {
                *p++ = e;
            }
        } else {
            throw ::std::runtime_error{ "Invalid initializer list size" };
        }
    }
    constexpr
    ipv6_address(init_list const& hi, init_list const& lo)
        : data{0}
    {
        if (hi.size() + lo.size() > size) {
            throw ::std::runtime_error{ "Invalid initializer list sum size" };
        }
        auto p = data;
        for (auto e : hi)
            *p++ = e;
        p = data + (size - lo.size());
        for (auto e : lo)
            *p++ = e;
    }

    constexpr explicit
    ipv6_address(ipv4_address::repr ipv4)
        : data{ 0, 0, 0, 0, 0, 0xffff,
            static_cast<element>(ipv4 >> 16),
            static_cast<element>(ipv4 & 0xffff)}
    {}

    constexpr explicit
    ipv6_address(ipv4_address const& ipv4)
        : ipv6_address(ipv4.data) {}

    explicit
    ipv6_address(char const* str);

    constexpr bool
    operator == (ipv6_address const& rhs) const
    {
        auto lp = data;
        auto rp = rhs.data;

        for (; lp != data + size; ++lp, ++rp) {
            if (*lp != *rp)
                return false;
        }
        return true;
    }

    constexpr bool
    operator < (ipv6_address const& rhs) const
    {
        auto lp = data;
        auto rp = rhs.data;

        for (; lp != data + size; ++lp, ++rp) {
            if (*lp < *rp)
                return true;
        }
        return false;
    }

    constexpr iterator
    begin()
    { return data; }
    constexpr const_iterator
    begin() const
    { return data; }

    constexpr iterator
    end()
    { return data + size; }
    constexpr const_iterator
    end() const
    { return data + size; }

    constexpr bool
    empty() const
    { return data == empty_addr; }
};

::std::ostream&
operator << (::std::ostream& os, ipv6_address const& val);
::std::string
to_string(ipv6_address const& val);

namespace detail {

// Function for falling back from reading ipv6 hextet to ipv4 decimal octet
// Will convert 0x127 -> 127
constexpr ::std::uint16_t
literal_hex_to_dec(::std::uint16_t hex)
{
    return (hex & 0x0f)
        + (((hex >> 4) & 0x0f) * 10)
        + (((hex >> 8) & 0x0f) * 100);
}

}  /* namespace detail */

inline constexpr ipv6_address
operator << (ipv6_address val, ::std::size_t bits)
{
    using element = ipv6_address::element;
    if (bits > 0) {
        ipv6_address v{};
        auto num_items = bits / ipv6_address::element_size;
        if (num_items < ipv6_address::size) {
            auto p = val.data + num_items;
            auto o = v.data;
            for (; p != val.data + ipv6_address::size;)
                *o++ = *p++;
        } else {
            return v;
        }
        bits %= ipv6_address::element_size;
        if (bits > 0) {
            // Beginning with size - num_items item
            // shift bits and carry bits to previous
            auto p = v.data + ipv6_address::size - num_items;
            element carry{0};
            do {
                --p;
                ::std::uint32_t e = *p;
                e = (e << bits) | carry;
                *p = static_cast< element >(e & 0xffff);
                carry = static_cast< element >( e >> 16);
            } while (p != v.data);
        }
        return v;
    }
    return val;
}

inline constexpr ipv6_address
operator >> (ipv6_address val, ::std::size_t bits)
{
    using element = ipv6_address::element;
    if (bits > 0) {
        ipv6_address v{};
        auto num_items = bits / ipv6_address::element_size;
        if (num_items < ipv6_address::size) {
            auto p = val.data;
            auto o = v.data + num_items;
            for (; p != val.data + ipv6_address::size - num_items;)
                *o++ = *p++;
        } else {
            return v;
        }
        bits %= ipv6_address::element_size;
        if (bits > 0) {
            // Beginning with num_items item
            // shift bits and carry bits to next
            auto p = v.data + num_items;
            element carry{0};
            for (; p != v.data + ipv6_address::size; ++p) {
                ::std::uint32_t e = (*p << 16) >> bits;
                *p = static_cast<element>(e >> 16) | carry;
                carry = static_cast< element >(e & 0xffff);
            }
        }
        return v;
    }
    return val;
}

inline constexpr ipv6_address
operator | (ipv6_address const& lhs, ipv6_address const& rhs)
{
    ipv6_address res;
    auto lp = lhs.data;
    auto rp = rhs.data;
    for (auto& e: res) {
        e = *lp++ | *rp++;
    }
    return res;
}

inline constexpr ipv4_address
operator "" _ipv4(char const* str, ::std::size_t sz)
{
    return ipv4_address{str};
}

constexpr
ipv4_address::ipv4_address(char const* str)
    : data{0}
{
    ::std::uint32_t curr{0};
    ::std::size_t curr_offset{ 24 };
    while (*str != 0) {
        if (*str == '.') {
            if (curr_offset == 0)
                throw ::std::runtime_error{ "Unexpected symbol in IPv4 literal" };
            data |= (curr << curr_offset);
            curr = 0;
            curr_offset -= 8;
        } else if ( '0' <= *str && *str <= '9' ) {
            curr *= 10;
            curr += (*str - '0');
            if (curr > 255)
                throw ::std::runtime_error{ "Invalid IPv4 octet " + ::std::to_string(curr) };
        } else {
            throw ::std::runtime_error{ "Unexpected symbol in IPv4 literal" };
        }
        ++str;
    }
    if (curr_offset > 0)
        throw ::std::runtime_error{ "Unexpected IPv4 literal end" };
    data |= curr;
}

constexpr ipv4_address const ipv4_localhost{ "127.0.0.1"_ipv4 };
constexpr ipv6_address const ipv6_localhost({}, { 1 });

} /* namespace v2 */

}  /* namespace iri */
}  /* namespace tip */


#endif /* TIP_IRI_IP_ADDRESS_HPP_ */

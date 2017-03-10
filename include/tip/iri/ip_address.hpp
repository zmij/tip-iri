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

#include <tip/iri/detail/char_class.hpp>
#include <tip/iri/detail/iri_part.hpp>

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

    constexpr explicit
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

inline constexpr ipv6_address
operator "" _ipv6(char const* str, ::std::size_t sz)
{
    return ipv6_address{str};
}

constexpr
ipv6_address::ipv6_address(char const* str)
    : data{0}
{
    using element = ipv6_address::element;

    element head[ ipv6_address::size ] {0};
    ::std::size_t head_size{0};
    element tail[ ipv6_address::size ] {0};
    ::std::size_t tail_size{0};

    ::std::int32_t  curr_hextet{0};
    // track classifications to find out if the number contains decimal digits only
    char_type       curr_chars{ char_type::none };
    //char            prev_char;

    ::std::size_t   hextet_cnt{0};

    auto p = str;
    while (*p != 0) {
        auto cls = char_classification::classify(*p);
        if (any(cls & char_type::xdigit)) {
            if (curr_chars == char_type::none) {
                curr_chars = cls;
            } else {
                curr_chars = curr_chars & cls;
            }
            curr_hextet <<= 8;
            curr_hextet += detail::digit_value(*p);
        } else {
            if (*p == ':') {

            } else if (*p == '.') {
                // previous hextet was actually the first decimal octet of IPv4 address
            } else {

            }
        }
    }
}

template <>
struct parser<iri_part::ipv4_address> :
        detail::parser_base< parser<iri_part::ipv4_address>, iri_part::ipv4_address> {
    using value_type    = ipv4_address;
    using digit_parser  = uint_parser<ipv4_address::repr, 10, 3>;
    using parser_state  = detail::parser_state;
    using base_type     = detail::parser_base< parser<iri_part::ipv4_address>, iri_part::ipv4_address>;

    constexpr parser()
        : octet_{}, offset_{24}, value_{}
    {
    }
    // Constructor for reading a trailing IPv4 part of an IPv6 address
    constexpr parser(ipv4_address::repr first_octet)
        : octet_{}, offset_{16}, value_{ first_octet << 24 }
    {
        if (first_octet > 255)
            fail();
        else
            start();
    }
    parser_state
    feed_char(char c)
    {
        if (want_more()) {
            auto cls = char_classification::classify(c);
            if (any(cls & char_type::digit)) {
                start();
                if (octet_.done())
                    return fail();
                octet_.feed_char(c);
                if (octet_.value() > 255)
                    return fail();
            } else {
                if (empty())
                    return fail();
                // If this is the last octet and we have some digits - push it and done
                if (last_octet()) {
                    return finish();
                } else if (c == '.') {
                    if (octet_.empty()) {
                        return fail();
                    }
                    push_octet();
                } else {
                    return fail();
                }
            }
        }
        return state;
    }
    constexpr bool
    last_octet() const
    { return offset_ == 0; }

    parser_state
    finish()
    {
        if (!done()) {
            if (octet_.empty()) {
                return fail();
            }
            if (!last_octet())
                return fail();
            push_octet();
            return base_type::finish();
        }
        return state;
    }

    void
    clear()
    {
        reset_octet();
        value_ = ipv4_address{};
        base_type::reset();
    }

    value_type
    value() const
    {
        return value_;
    }
private:
    void
    reset_octet()
    {
        octet_.clear();
    }
    void
    push_octet()
    {
        if (octet_.value() == 0 && octet_.digits() > 1) {
            fail();
        }
        value_.data |= octet_.value() << offset_;
        if (offset_ >= 8)
            offset_ -= 8;
        else
            base_type::finish();
        reset_octet();
    }
    digit_parser        octet_;
    ::std::size_t       offset_;
    ipv4_address        value_;
};

template <>
struct parser<iri_part::ipv6_address>
        : detail::parser_base< parser<iri_part::ipv6_address>, iri_part::ipv6_address> {
    using base_type     = detail::parser_base< parser<iri_part::ipv6_address>, iri_part::ipv6_address>;
    using value_type    = ipv6_address;
    using element       = ipv6_address::element;
    using parser_state  = detail::parser_state;
    using digit_parser  = uint_parser<element, 16, 4>;
    using ipv4_parser   = parser< iri_part::ipv4_address >;

    constexpr parser()
        : stage_{none}, prev_{0}, head_{0}, head_size_{0}, tail_{0}, tail_size_{0}
    {
    }

    parser_state
    feed_char(char c)
    {
        if (want_more()) {
            auto cls = char_classification::classify(c);
            if (stage_ == none) {
                stage_ = head;
                start();
            }
            if (stage_ == tail_ipv4) {
                ipv4_parser_.feed_char(c);
                if (ipv4_parser_.failed())
                    return fail();
                if (!ipv4_parser_.want_more()) {
                    // Set the bytes
                    return finish();
                }
            } else {
                if (any(cls & char_type::xdigit)) {
                    if (stage_ == tail_from_begin)
                        // Fist char is colon and second is not
                        return fail();
                    if (hextet_.done()) {
                        // Extra digit in hextet
                        return fail();
                    }
                    hextet_.feed_char(c);
                } else if (c == ':') {
                    switch (prev_) {
                        case 0:
                            // Leading colon, want a second one
                            stage_ = tail_from_begin;
                            break;
                        case ':':
                            // An extra colon or double colon for a second time
                            if (stage_ == tail)
                                return fail();
                            stage_ = tail;
                            break;
                        default:
                            push_hextet();
                            break;
                    }
                } else if (c == '.') {
                    // convert previous hextet to a decimal octet (literally)
                    // and proceed with parsing ipv4 part
                    if (hextet_.digits() == 0)
                        return fail();
                    // No hextets skipped, must be exactly 6 before
                    if (stage_ == head && head_size_ != 6)
                        return fail();
                    if (stage_ == tail && head_size_ + tail_size_ > 4)
                        return fail();
                    ipv4_parser_ = ipv4_parser{ detail::literal_hex_to_dec(hextet_.value()) };
                    hextet_.clear();
                    if (ipv4_parser_.failed())
                        return fail();
                    stage_ = tail_ipv4;
                } else {
                    finish();
                }
            }
            prev_ = c;
        }
        return state;
    }

    parser_state
    finish()
    {
        if (!done()) {
            if (stage_ == tail_from_begin)
                return fail();
            if (stage_ == tail_ipv4) {
                ipv4_parser_.finish();
                if (ipv4_parser_.failed())
                    return fail();
                // Set the bytes
                set_ipv4();
                return base_type::finish();
            }
            if (prev_ == ':') {
                // A single trailing colon
                if (stage_ != tail)
                    return fail();
                // A colon after a tail hextet
                if (tail_size_ > 0)
                    return fail();
            } else {
                // Prev char is not colon, so it's a hexnumber
                push_hextet();
            }
            if (stage_ == head && head_size_ < ipv6_address::size)
                return fail();
            // Less than two zeros skipped
            if (stage_ == tail &&
                    (head_size_ + tail_size_) > (ipv6_address::size - 2))
                return fail();
            return base_type::finish();
        }
        return state;
    }

    ipv6_address
    value() const
    {
        if (head_size_ + tail_size_ > ipv6_address::size)
            throw ::std::runtime_error{ "Invalid hextet count" };
        ipv6_address addr{};

        auto p = addr.data;
        for (auto h = head_; h != head_ + head_size_;) {
            *p++ = *h++;
        }

        p = addr.data + ipv6_address::size - tail_size_;
        for (auto t = tail_; p != addr.data + ipv6_address::size;) {
            *p++ = *t++;
        }
        return addr;
    }

    void
    clear()
    {
        ;
    }
private:
    void
    push_hextet()
    {
        if (hextet_.digits() == 0) {
            fail();
            return;
        }

        if (stage_ == head) {
            head_[head_size_++] = hextet_.value();
            if (head_size_ == ipv6_address::size)
                base_type::finish();
        } else if (stage_ == tail) {
            tail_[tail_size_++] = hextet_.value();
            if (head_size_ + tail_size_ == ipv6_address::size - 2)
                base_type::finish();
        }
        hextet_.clear();
    }
    void
    set_ipv4()
    {
        ipv4_parser_.finish();
        if (ipv4_parser_.failed())
            return;
        auto ipv4 = ipv4_parser_.value().data;
        tail_[tail_size_++] = (ipv4 >> 16) & 0xffff;
        tail_[tail_size_++] = ipv4 & 0xffff;
    }
    enum ipv6_stage {
        none,
        head,
        tail_from_begin,
        tail,
        tail_ipv4
    };
    ipv6_stage      stage_;
    char            prev_;
    digit_parser    hextet_;
    ipv4_parser     ipv4_parser_;

    element         head_[ipv6_address::size];
    ::std::size_t   head_size_;
    element         tail_[ipv6_address::size];
    ::std::size_t   tail_size_;
};

constexpr ipv4_address const ipv4_localhost{ "127.0.0.1"_ipv4 };
constexpr ipv6_address const ipv6_localhost({}, { 1 });

} /* namespace v2 */

}  /* namespace iri */
}  /* namespace tip */


#endif /* TIP_IRI_IP_ADDRESS_HPP_ */

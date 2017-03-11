/*
 * ip_address_parsers.hpp
 *
 *  Created on: Mar 11, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_DETAIL_IP_ADDRESS_PARSERS_HPP_
#define TIP_IRI_DETAIL_IP_ADDRESS_PARSERS_HPP_

#include <tip/iri/detail/char_class.hpp>
#include <tip/iri/detail/iri_part.hpp>

#include <tip/iri/ip_address.hpp>

namespace tip {
namespace iri {
inline namespace v2 {

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
        stage_ = none;
        prev_ = 0;
        ipv4_parser_.clear();
        hextet_.clear();

        head_size_ = 0;
        tail_size_ = 0;
        base_type::reset();
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

/**
 * Parser for IP literal, that is IPv6 or IP future in square brackets
 * TODO IP Future
 */
template <>
struct parser<iri_part::ip_literal>
        : detail::parser_base< parser<iri_part::ip_literal>, iri_part::ip_literal> {
    using base_type     = detail::parser_base< parser<iri_part::ip_literal>, iri_part::ip_literal>;
    using value_type    = ipv6_address;
    using parser_state  = detail::parser_state;
    using ipv6_parser   = parser<iri_part::ipv6_address>;

    constexpr parser()
        : ipv6_{} {}

    parser_state
    feed_char(char c)
    {
        if (empty()) {
            // Want [ as a first symbol
            if (c != '[')
                return fail();
            start();
        } else if (want_more()){
            if (c == ']') {
                // finish this
                ipv6_.finish();
                if (ipv6_.failed())
                    return fail();
                base_type::finish();
            } else {
                if (ipv6_.want_more()) {
                    ipv6_.feed_char(c);
                } else {
                    return fail();
                }
            }
        }
        return state;
    }

    parser_state
    finish()
    {
        if (want_more())
            return fail();
        return state;
    }

    value_type
    value() const
    {
        return ipv6_.value();
    }

    void
    clear()
    {
        ipv6_.clear();
        base_type::reset();
    }
private:
    ipv6_parser        ipv6_;
};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */




#endif /* TIP_IRI_DETAIL_IP_ADDRESS_PARSERS_HPP_ */

/*
 * escape_string.hpp
 *
 *  Created on: Mar 30, 2017
 *      Author: zmij
 */

#ifndef TIP_IRI_DETAIL_ESCAPE_STRING_HPP_
#define TIP_IRI_DETAIL_ESCAPE_STRING_HPP_

#include <tip/iri/detail/char_class.hpp>
#include <iostream>
#include <iomanip>

namespace tip {
namespace iri {
inline namespace v2 {

template < typename T, T Filter >
struct char_escaper {
    using classificator = typename char_classification_traits<T>::type;
    static constexpr T filter = Filter;

    template < typename InputIterator >
    static void
    escape(::std::ostream& os, InputIterator begin, InputIterator end)
    {
        while (begin != end) {
            auto cls = classificator::classify(*begin);
            if (any(cls & filter)) {
                os.put(*begin++);
            } else {
                auto c_cls = char_classification::classify(*begin);
                if (any( c_cls & char_type::utf_byte )) {
                    // Hex encode UTF-8 sequence
                    auto sz = codepoint_size(c_cls);
                    os << "%x";
                    ::std::uint32_t val{0};
                    for (; sz > 0 && begin != end; --sz) {
                        val <<= 8;
                        val |= (unsigned char)*begin++;
                    }
                    os << std::hex << val;
                } else {
                    // Percent encode single char
                    os << '%' << ::std::hex << (int)*begin++;
                }
            }
        }
    }
};

} /* namespace v2 */
} /* namespace iri */
} /* namespace tip */


#endif /* TIP_IRI_DETAIL_ESCAPE_STRING_HPP_ */

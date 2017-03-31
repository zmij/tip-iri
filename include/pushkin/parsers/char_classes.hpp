/*
 * char_classes.hpp
 *
 *  Created on: Mar 9, 2017
 *      Author: zmij
 */

#ifndef PUSHKIN_DETAIL_CHAR_CLASSES_HPP_
#define PUSHKIN_DETAIL_CHAR_CLASSES_HPP_

#include <pushkin/meta/char_sequence.hpp>

namespace psst {
namespace parsers {

namespace detail {
template < bool, typename T, T ifTrue, T ifFalse >
struct conditional_c : ::std::integral_constant<T, ifTrue> {};

template < typename T, T ifTrue, T ifFalse >
struct conditional_c< false, T, ifTrue, ifFalse > : ::std::integral_constant<T, ifFalse> {};

template < typename T, T ... Values >
struct binary_or;

template < typename T, T Val, T ... Values >
struct binary_or< T, Val, Values... > {
    using type = ::std::integral_constant< T, ( binary_or< T, Values... >::type::value | Val ) >;
};

template < typename T, T Val >
struct binary_or< T, Val > {
    using type = ::std::integral_constant<T, Val>;
};

template < typename T >
struct binary_or<T> {
    static constexpr T def_val{};
    using type = ::std::integral_constant<T, def_val>;
};

} /* namespace detail */

template < typename Charset, typename T, T Value, T Default = T{} >
struct character_class {
    using charset = Charset;
    template < char C >
    struct class_of : detail::conditional_c<
        ::psst::meta::chars::contains< charset, C >::value, T, Value, Default > {};
};

namespace detail {

template < ::std::size_t Idx, typename T, typename ... Classes >
struct nth_classification {
    using type = typename binary_or< T, Classes::template class_of<Idx>::value... >::type;
};

template < typename Index, typename MapTo, typename ... Class >
struct character_class_table_impl;

template < ::std::size_t ... Indexes, typename T, typename ... Classes >
struct character_class_table_impl< ::std::integer_sequence<::std::size_t, Indexes ...>,
        T, Classes...> {
    using value_type    = T;

    template < ::std::size_t Idx >
    using nth_value = typename nth_classification<Idx, value_type, Classes...>::type;
    using type = ::std::integer_sequence<value_type, nth_value<Indexes>::value...>;
    static constexpr ::std::size_t size = sizeof ... (Indexes);
    static constexpr value_type value[] = {
        nth_value< Indexes >::value...
    };

    static constexpr value_type
    classify(char c)
    {
        if (c >= size)
            return value_type{};
        return value[c];
    }
};

template < ::std::size_t ... Indexes, typename T, typename ... Classes >
constexpr T character_class_table_impl<
                ::std::integer_sequence<::std::size_t, Indexes ...>,
                T, Classes... >::value[];

} /* namespace detail */

template < ::std::size_t Size, typename MapTo, typename ... Class >
struct character_class_table :
        detail::character_class_table_impl<::std::make_index_sequence<Size>,
                MapTo, Class...> {};



namespace char_classes {

using namespace ::psst::meta;

using horizontal_space      = char_sequence< ' ', '\t' >;
using vertical_space        = char_sequence< '\r', '\n', '\v' >;
using whitespace            = join< horizontal_space, vertical_space >::type;
using latin_upper           = char_range< 'A', 'Z' >;
using latin_lower           = char_range< 'a', 'z' >;
using alpha_chars           = join< latin_upper, latin_lower >::type;
using digits                = char_range< '0', '9' >;
using alnum_chars           = join< alpha_chars, digits >::type;

using oct_digits            = char_range< '0', '7' >;
using hex_digits            = join< digits,
                                char_range<'A', 'F'>,
                                char_range<'a', 'f'> >::type;

constexpr char punct_str[]  = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";
constexpr char arithm_str[] = "+-*/=";
constexpr char logic_str[]  = "!&|^<>=";

using punctuation_chars     = make_char_sequence<punct_str>;
using arithmetic_ops        = make_char_sequence<arithm_str>;
using logic_ops             = make_char_sequence<logic_str>;

} /* namespace char_classes */


} /* namespace parsers */
} /* namespace psst */



#endif /* PUSHKIN_DETAIL_CHAR_CLASSES_HPP_ */

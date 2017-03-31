/*
 * append.hpp
 *
 *  Created on: Mar 29, 2017
 *      Author: zmij
 */

#ifndef PUSHKIN_PARSERS_APPEND_HPP_
#define PUSHKIN_PARSERS_APPEND_HPP_

#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <boost/variant.hpp>

namespace psst {
namespace parsers {

template < typename Output, typename Input >
void
append(Output& out, Input&& in);

namespace detail {

template < typename Output, typename Input >
struct appender;

template <>
struct appender< ::std::string, char > {
    static void
    append(::std::string& s, char c)
    {
        s.push_back(c);
    }
};

template <>
struct appender< ::std::string, unsigned char > {
    static void
    append(::std::string& s, unsigned char c)
    {
        s.push_back(c);
    }
};

template <>
struct appender< ::std::string, ::std::string > {
    static void
    append(::std::string& s, ::std::string const& v)
    {
        s += v;
    }
};

template < typename T, typename ... U >
struct appender < T, ::boost::variant<U...> > {
    struct append_visitor : ::boost::static_visitor<> {
        T& val;

        append_visitor(T& v) : val{v} {}

        template < typename V >
        void
        operator()(V const& v) const
        {
            appender< T, V >::append(val, v);
        }
    };

    static void
    append(T& val, ::boost::variant< U... > const& var)
    {
        ::boost::apply_visitor(append_visitor{val}, var);
    }
};

template < ::std::size_t N >
struct append_nth {
    template < typename T, typename Tuple >
    static void
    append_element(T& val, Tuple const& var)
    {
        append_nth<N - 1>::append_element(val, var);
        append(val, ::std::get<N>(var));
    }
};

template <>
struct append_nth<0> {
    template < typename T, typename Tuple >
    static void
    append_element(T& val, Tuple const& var)
    {
        append(val, ::std::get<0>(var));
    }
};

template < typename T, typename ... U>
struct appender < T, ::std::tuple<U...> > {
    using indexes   = ::std::make_index_sequence< sizeof ... (U) >;

    static void
    append(T& val, ::std::tuple<U...> const& var)
    {
        append_nth<sizeof ... (U) - 1>::append_element(val, var);
    }
};

template < typename T >
struct appender < T, ::std::tuple<> > {
    static void
    append(T& val, ::std::tuple<> const& var) {}
};

template < typename T, typename V, typename ... Rest >
struct appender<::std::vector<T, Rest...>, V> {
    static void
    append(::std::vector<T, Rest...>& val, V const& var)
    {
        val.emplace_back(var);
    }
};

template < typename K, typename V, typename ... Rest >
struct appender< ::std::map<K, V, Rest...>, ::std::pair<K, V> > {
    static void
    append(::std::map<K, V, Rest...>& val, ::std::pair<K, V> const& var)
    {
        val.emplace(var);
    }
};

template < typename K, typename V, typename ... Rest >
struct appender< ::std::multimap<K, V, Rest...>, ::std::pair<K, V> > {
    static void
    append(::std::multimap<K, V, Rest...>& val, ::std::pair<K, V> const& var)
    {
        val.emplace(var);
    }
};

template < typename K, typename V, typename ... Rest >
struct appender< ::std::map<K, V, Rest...>, ::std::tuple<K, V> > {
    static void
    append(::std::map<K, V, Rest...>& val, ::std::tuple<K, V> const& var)
    {
        val.emplace(::std::get<0>(var), ::std::get<1>(var));
    }
};

template < typename K, typename V, typename ... Rest >
struct appender< ::std::multimap<K, V, Rest...>, ::std::tuple<K, V> > {
    static void
    append(::std::multimap<K, V, Rest...>& val, ::std::tuple<K, V> const& var)
    {
        val.emplace(::std::get<0>(var), ::std::get<1>(var));
    }
};

} /* namespace detail */

template < typename Output, typename Input >
void
append(Output& out, Input&& in)
{
    using appender_type = detail::appender<
                                typename ::std::decay<Output>::type,
                                typename ::std::remove_const<
                                    typename ::std::decay<Input>::type
                                >::type
                          >;
    appender_type::append(out, ::std::forward<Input>(in));
}


} /* namespace parsers */
} /* namespace psst */


#endif /* PUSHKIN_PARSERS_APPEND_HPP_ */

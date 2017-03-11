/*
 * gcc_internal_error.cpp
 *
 *  Created on: Mar 10, 2017
 *      Author: zmij
 */

#include <gtest/gtest.h>
#include <utility>

namespace test {

template < typename T, typename U, U Value >
struct some_struct {};

template < typename Index, typename ... Classes >
struct test_parallel_variadic_unpack;

template < ::std::size_t ... Indexes, typename ... T, typename U, U ... Values >
struct test_parallel_variadic_unpack<::std::integer_sequence<::std::size_t, Indexes...>,
    some_struct<T, U, Values>...> {};

template < typename T, int V >
using int_constant = some_struct<T, int, V>;

struct instantiation : test_parallel_variadic_unpack<
        ::std::make_index_sequence<64>,
        int_constant<int, 0>, int_constant<double, -5>, int_constant<long, 42>> {};

} /* namespace test */


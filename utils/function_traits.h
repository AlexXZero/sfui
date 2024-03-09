#ifndef UTILS_FUNCTION_TRAITS_H
#define UTILS_FUNCTION_TRAITS_H

#include <tuple>
#include <type_traits>

namespace utils {

#define UTILS_FUNCTION_TRAITS_FIELDS                                    \
    static inline constexpr std::size_t arity = sizeof...(Args);        \
    using result_type = ReturnType;                                     \
    using args_tuple = std::tuple<Args...>;                             \
                                                                        \
    template<std::size_t N, typename = std::enable_if_t<(N < arity)>>   \
    using arg = std::tuple_element<N, args_tuple>;                      \


// Helper function to deduce the argument types of a callable object
template<typename Func>
struct function_traits : function_traits<decltype(&std::remove_reference_t<Func>::operator())> {};

// Specialization for class member function (including lambda functions)
template<typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const> { UTILS_FUNCTION_TRAITS_FIELDS; };

// Specialization for regular function types
template<typename ReturnType, typename... Args>
struct function_traits<ReturnType(Args...)> { UTILS_FUNCTION_TRAITS_FIELDS; };

// Specialization for function pointers
template<typename ReturnType, typename... Args>
struct function_traits<ReturnType(*)(Args...)> { UTILS_FUNCTION_TRAITS_FIELDS; };

// Specialization for function references
template<typename ReturnType, typename... Args>
struct function_traits<ReturnType(&)(Args...)> { UTILS_FUNCTION_TRAITS_FIELDS; };

template<typename Func>
inline constexpr std::size_t arguments_count = function_traits<Func>::arity;

template<typename Func, std::size_t N, typename = std::enable_if_t<(N < arguments_count<Func>)>>
using function_argument_t = typename function_traits<Func>::arg<N>::type;

template<typename Func, typename = std::enable_if_t<(arguments_count<Func> > 0)>>
using first_argument_t = function_argument_t<Func, 0>;

// See https://stackoverflow.com/questions/57787666/is-it-possible-to-implement-always-false-in-the-c-standard-library
template<typename...>
inline constexpr bool always_false_v = false;

}

#endif // UTILS_FUNCTION_TRAITS_H

#pragma once

namespace type_mutator {
namespace detail {
template <int N> struct Cnt : Cnt<N - 1> { static constexpr int depth = N; };
template <> struct Cnt<0> {};

template <class T, int N> struct RetType {
    using type = T;
    static constexpr int depth = N;
};

template <class T, int N> struct RetValue : RetType<T, N> {
    T value;
    constexpr RetValue(T t) : value(t) {}
};

template <int N = 100> struct MaxDepth { static constexpr int depth = N; };
} // namespace detail
} // namespace type_mutator

#define TM_VAR(Name, ...)                                                      \
    using TypeMutatorDetailMaxDepth##Name =                                    \
        ::type_mutator::detail::MaxDepth<__VA_ARGS__>;                         \
    static ::type_mutator::detail::RetType<void, 0> __detail_tm_helper_##Name( \
        ::type_mutator::detail::Cnt<0> n) {                                    \
        TypeMutatorDetailMaxDepth##Name k;                                     \
        static_assert(k.depth >= 0, "TypeMutator: depth size must >= 0");      \
        __detail_tm_helper_##Name(n);                                          \
        return {};                                                             \
    }

#define DETAIL_TM_MAX_DEPTH(Name) TypeMutatorDetailMaxDepth##Name::depth
#define DETAIL_TM_LAST_FUNCTION_CALL(Name)                                         \
    __detail_tm_helper_##Name(                                                 \
        ::type_mutator::detail::Cnt<DETAIL_TM_MAX_DEPTH(Name)>{})

#define DETAIL_TM_LAST_RESULT(Name) decltype(DETAIL_TM_LAST_FUNCTION_CALL(Name))

#define DETAIL_TM_NEXT_INDEX(Name) DETAIL_TM_LAST_RESULT(Name)::depth + 1

#define TM_GET(Name) DETAIL_TM_LAST_RESULT(Name)::type
#define TM_SET(Name, Type)                                                     \
    static ::type_mutator::detail::RetType<Type, DETAIL_TM_NEXT_INDEX(Name)>   \
        __detail_tm_helper_##Name(                                             \
            ::type_mutator::detail::Cnt<DETAIL_TM_NEXT_INDEX(Name)> n) {       \
        static_assert(n.depth <= DETAIL_TM_MAX_DEPTH(Name),                    \
                      "TypeMutator " #Name ": Depth size is too small");       \
        return {};                                                             \
        return __detail_tm_helper_##Name(n);                                   \
    }

#define VM_GET(Name) DETAIL_TM_LAST_FUNCTION_CALL(Name).value
#define VM_SET(Name, Expr)                                                     \
    constexpr static ::type_mutator::detail::RetValue<                         \
        decltype(Expr), DETAIL_TM_NEXT_INDEX(Name)>                            \
        __detail_tm_helper_##Name(                                             \
            ::type_mutator::detail::Cnt<DETAIL_TM_NEXT_INDEX(Name)> n) {       \
        static_assert(n.depth <= DETAIL_TM_MAX_DEPTH(Name),                    \
                      "TypeMutator " #Name ": Depth size is too small");       \
        return true ? ::type_mutator::detail::RetValue<decltype(Expr),         \
                                                       n.depth>{(Expr)}        \
                    : __detail_tm_helper_##Name(n);                            \
    }

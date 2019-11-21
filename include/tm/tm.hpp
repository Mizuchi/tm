#pragma once

namespace type_mutator {
namespace detail {
template <int N> struct Cnt : Cnt<N - 1> { static constexpr int depth = N; };
template <> struct Cnt<0> { static constexpr int depth = 0; };

template <class T, int N> struct RetType {
    using type = T;
    static constexpr int depth = N;
};

template <int N = 100> struct MaxDepth { static constexpr int depth = N; };
} // namespace detail
} // namespace type_mutator

#define DETAIL_TM_MAX_DEPTH(Name) DetailTypeMutatorMaxDepth##Name::depth
#define DETAIL_TM_DEFINE(Name, Type, Index)                                    \
    constexpr static ::type_mutator::detail::RetType<Type, Index>              \
        __detail_tm_helper_##Name(::type_mutator::detail::Cnt<Index> n) {      \
        static_assert(n.depth <= DETAIL_TM_MAX_DEPTH(Name),                    \
                      "TypeMutator " #Name ": Depth size is too small");       \
        return {};                                                             \
    }

#define DETAIL_TM_LAST_FUNC_CALL(Name)                                         \
    __detail_tm_helper_##Name(                                                 \
        ::type_mutator::detail::Cnt<DETAIL_TM_MAX_DEPTH(Name)>{})

#define DETAIL_TM_LAST_RESULT(Name) decltype(DETAIL_TM_LAST_FUNC_CALL(Name))

#define DETAIL_TM_NEXT_INDEX(Name) DETAIL_TM_LAST_RESULT(Name)::depth + 1

#define TM_VAR(Name, ...)                                                      \
    using DetailTypeMutatorMaxDepth##Name =                                    \
        ::type_mutator::detail::MaxDepth<__VA_ARGS__>;                         \
    DETAIL_TM_DEFINE(Name, void, 0)

#define TM_GET(Name) DETAIL_TM_LAST_RESULT(Name)::type
#define TM_SET(Name, Type)                                                     \
    DETAIL_TM_DEFINE(Name, Type, DETAIL_TM_NEXT_INDEX(Name))

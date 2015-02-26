#include "tm/tm.hpp"

#include <map>
#include <type_traits>

using namespace std;

TM_VAR(T); // define a variable

static_assert(is_same<TM_GET(T), void>::value, ""); // initial T == void

TM_SET(T, int);                                    // set T = int
static_assert(is_same<TM_GET(T), int>::value, ""); // T == int

TM_SET(T, double); // set T = double
static_assert(is_same<TM_GET(T), double>::value, "");

// we can't TM_SET(T, map<int, double>);
// it doesn't work since C++ macro think there are 3 arguments.
using Int2Double = map<int, double>;
TM_SET(T, Int2Double);
static_assert(is_same<TM_GET(T), Int2Double>::value, "");

TM_SET(T, double);

class Scope {
    // if we use TM_SET(T) inside class, we must redefine by TM_VAR(T)
    TM_VAR(T);
    static_assert(is_same<TM_GET(T), void>::value, "");

    TM_VAR(U);
    TM_SET(U, TM_GET(T));
    static_assert(is_same<TM_GET(U), void>::value, "");

    TM_SET(T, double);
    static_assert(is_same<TM_GET(T), double>::value, "");

    TM_SET(T, TM_GET(U));
    static_assert(is_same<TM_GET(T), void>::value, "");
};

class Scope2 {
    // if we didn't use TM_SET(T) inside class, we don't need to TM_VAR(T);
    static_assert(is_same<TM_GET(T), double>::value, "");
    TM_VAR(U);
    TM_SET(U, TM_GET(T));
    static_assert(is_same<TM_GET(U), double>::value, "");
};

namespace scope {
// namespace is little bit different than class, we could use TM_SET(T)
// without using TM_VAR(T) (but it's okay to redefine by TM_VAR(T))
TM_VAR(U);
TM_SET(U, TM_GET(T));
static_assert(is_same<TM_GET(U), double>::value, "");
TM_SET(T, int);
static_assert(is_same<TM_GET(T), int>::value, "");
}

namespace scope2 {
// namespace is little bit different than class, we could use TM_SET(T)
// without using TM_VAR(T) (but it's okay to redefine by TM_VAR(T))
TM_VAR(T);
TM_VAR(U);
TM_SET(U, TM_GET(T));
static_assert(is_same<TM_GET(U), void>::value, "");
TM_SET(T, int);
static_assert(is_same<TM_GET(T), int>::value, "");
}

// T is restored after leaving the namespace
static_assert(is_same<TM_GET(T), double>::value, "");

TM_VAR(Depth, 2 /* at most 2 TM_SET statements */);
TM_SET(Depth, int);
static_assert(is_same<TM_GET(Depth), int>::value, "");
TM_SET(Depth, double);
static_assert(is_same<TM_GET(Depth), double>::value, "");
// TM_SET(Depth, void); // compile-error: exceed max depth

int main() {
    class Scope1 {
        TM_VAR(T);

        static_assert(is_same<TM_GET(T), void>::value, "");

        TM_SET(T, int);
        static_assert(is_same<TM_GET(T), int>::value, "");

        TM_SET(T, double);
        static_assert(is_same<TM_GET(T), double>::value, "");
        class Scope2 {
            TM_VAR(T);
            TM_SET(T, int);
            static_assert(is_same<TM_GET(T), int>::value, "");

            TM_VAR(U);
            TM_SET(U, TM_GET(T));
            static_assert(is_same<TM_GET(U), int>::value, "");

            TM_SET(T, double);
            static_assert(is_same<TM_GET(T), double>::value, "");

            TM_SET(T, TM_GET(U));
            static_assert(is_same<TM_GET(T), int>::value, "");
        };
    };
}

TM_VAR(NoUnusedWarning);
TM_VAR(NoUnusedWarning2, 0);
TM_VAR(NoUnusedWarning3, 1);
TM_SET(NoUnusedWarning3, void);

TM_VAR(x);
VM_SET(x, 2 + 2);
static_assert(VM_GET(x) == 4, "");

// Limitations:
//
// 1. TM_GET(T) could be used anywhere, but TM_SET(T) can't be used inside
// function.
//
// 2. "restored after leaving the namespace" semantics are counter intuitive to
// imperative programming. This is a bug which looks like a feature.

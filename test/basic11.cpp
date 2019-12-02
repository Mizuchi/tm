#include <tm/tm.hpp>

#include <tuple>
#include <type_traits>

using namespace std;

#define VAR TM_VAR
#define SET TM_SET
#define GET TM_GET

// Using __VA_ARGS__ in case second argument contains ','
#define IS_SAME(T, ...) static_assert(is_same_v<GET(T), __VA_ARGS__>)

VAR(T);           // define a variable T
IS_SAME(T, void); // by default T's value is void

SET(T, int); // set T = int
IS_SAME(T, int);

SET(T, double); // set T = double
IS_SAME(T, double);

SET(T, tuple<int, double>);
IS_SAME(T, tuple<int, double>);

// Add float to the type list
SET(T, decltype(tuple_cat(GET(T)(), tuple<float>())));
IS_SAME(T, tuple<int, double, float>);

GET(T) x;
static_assert(is_same_v<decltype(x), tuple<int, double, float>>);

namespace Scope {
IS_SAME(T, tuple<int, double, float>);
SET(T, double);
IS_SAME(T, double);
} // namespace Scope

// when we exit scope, T will be restored to original value
IS_SAME(T, tuple<int, double, float>);
SET(T, double);

class Class {
    // class is bit different than namespace.
    // if we use SET(T) inside class, we must redefine by VAR(T)
    VAR(T);
    IS_SAME(T, void);

    VAR(U);
    SET(U, GET(T));
    IS_SAME(U, void);

    SET(T, double);
    IS_SAME(T, double);

    SET(T, GET(U));
    IS_SAME(T, void);
};

class Class2 {
    // if we don't use SET(T) inside class, we don't need VAR(T);
    IS_SAME(T, double);
    VAR(U);
    SET(U, GET(T));
    IS_SAME(U, double);
};

VAR(Depth, 2 /* at most 2 SET statements */);
SET(Depth, int);
IS_SAME(Depth, int);
SET(Depth, double);
IS_SAME(Depth, double);
// SET(Depth, void); // compile-error: exceed max depth

int main() {
    class Scope1 {
        VAR(T);

        IS_SAME(T, void);

        SET(T, int);
        IS_SAME(T, int);

        SET(T, double);
        IS_SAME(T, double);

        class Scope2 {
            VAR(T);
            SET(T, int);
            IS_SAME(T, int);

            VAR(U);
            SET(U, GET(T));
            IS_SAME(U, int);

            SET(T, double);
            IS_SAME(T, double);

            SET(T, GET(U));
            IS_SAME(T, int);
        };
    };
}

VAR(NoUnusedWarning);
VAR(NoUnusedWarning2, 0);
VAR(NoUnusedWarning3, 1);
SET(NoUnusedWarning3, void);

// Limitations:
//
// 1. GET(T) could be used anywhere, but SET(T) can't be used inside
// function.
//
// 2. "restored after leaving the namespace" semantics are counter intuitive to
// imperative programming. This is a bug which looks like a feature.

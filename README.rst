Tutorial
=========

Template metaprograms have no mutable variables - that is, no variable can change value once it has been initialized, therefore template metaprogramming can be seen as a form of functional programming.

.. code-block:: cpp

    using T = int;
    // do something
    T = double; // illegal, you can't change variable's value

This library is inspired by imperative programming. It provides mutable variables for template programming. Here are some real C++ code (you can find this in unit-test):

.. code-block:: cpp

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

Here is a use case, assume we want to process data by multiple components. Each component look like this:

.. code-block:: cpp

    // CompA.h
    struct CompA { static void process(Data& data); };

    // CompB.h
    struct CompB { static void process(Data& data); };

    // CompC.h
    struct CompC { static void process(Data& data); };

    /// So I am thinking about how to combine these components together.

    // === Approach #1 ===

    // Process.cpp
    #include "CompA.h"
    #include "CompB.h"
    #include "CompC.h"
    #include <fatal/type/type_list.h>

    using CompList = lib::type_list<CompA, CompB, CompC>;

    int main() {
     Data d;
     CompList::foreach([&d](auto comp){ comp.process(d); });
    }

    // Advantage, not runtime-penalty
    // Disadvantage, when we add/remove component, we need to modify CompList in another file.

    // === Approach #2 ===

    // Comp.h
    struct Base {
     virtual void process(Data&data) const = 0;
     ~Base(){}
    };

    auto register_n_get(Base* b = nullptr) {
     static vector<Base*> r;
     if (b) { r.push_back(b); }
     return r;
    }

    // CompA.h
    #include "Comp.h"
    struct CompA: Base { void process(Data& data)const override; };

    // CompA.cpp
    struct Register {
     Register(){ register_n_get(new CompA); }
    } r;

    // CompB.h, CompB.cpp are similar

    // Process.cpp
    #include "CompA.h"
    #include "CompB.h"
    #include "CompC.h"

    int main() {
     Data d;
     for(Base *b: register_n_get()) { b->process(d); }
    }

    // Advantage, self-register, single source of truth
    // Disadvantage, runtime penalty, registering & processing order is indeterminate

    // === Approach #3 ===

    // Comp.h
    #include <fatal/type/type_list.h>
    #include <my_diff.h>
    struct T {};
    SET(T, lib::type_list<>);

    // CompA.h
    #include "Comp.h"
    struct CompA { static void process(Data& data); };
    SET(T, GET(T)::push_back<CompA>);

    // CompB.h
    #include "Comp.h"
    struct CompB { static void process(Data& data); };
    SET(T, GET(T)::push_back<CompB>);

    // CompC.h
    #include "Comp.h"
    struct CompC { static void process(Data& data); };
    SET(T, GET(T)::push_back<CompC>);

    // Process.cpp
    #include "CompA.h"
    #include "CompB.h"
    #include "CompC.h"

    using CompList = GET(T);

    int main() {
     Data d;
     CompList::foreach([&d](auto comp){ comp.process(d); });
    }

    // Advantage, self-register, single source of truth, not runtime-penalty

License
=======

Code is licensed under Reciprocal Public License 1.5 (RPL-1.5): https://opensource.org/licenses/RPL-1.5

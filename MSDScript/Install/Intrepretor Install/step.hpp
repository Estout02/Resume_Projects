#pragma once
#include <iostream>
#include "pointer.hpp"

class Expr;
class Cont;
class Env;
class Val;

class Step {
public:
    typedef enum {
        interp_mode,
        continue_mode
    }mode_t;
    static mode_t mode;
    static PTR(Expr) expr;
    static PTR(Env) env;
    static PTR(Val) val;
    static PTR(Cont) cont;
    static PTR(Val)interp_by_steps(PTR(Expr) e);

};


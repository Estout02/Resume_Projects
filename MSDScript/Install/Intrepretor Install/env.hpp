#pragma once
#include "pointer.hpp"
#include <iostream>
#include <string>

class Val;
class Expr;

class Env {
public:
    virtual PTR(Val) lookup(std::string find_name) = 0;
    static PTR(Env) empty;
};

class EmptyEnv : public Env {
public:
    EmptyEnv();
    PTR(Val) lookup(std::string find_name);
};

class ExtendedEnv : public Env {
public:
    std::string name;
    PTR(Val) val;
    PTR(Env) rest;


    PTR(Val) lookup(std::string find_name);

    ExtendedEnv(std::string name, PTR(Val) val, PTR(Env) rest);
};


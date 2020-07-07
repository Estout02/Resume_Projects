#pragma once
#include "pointer.hpp"
#include <iostream>

class Expr;
class Env;
class Cont;


class Val {
public:

    /** Checks if vals of expressions are equal.
     *
     * @param val
     * @return
     */
    virtual bool equals(PTR(Val) val) = 0;

    /** Attempts to add two Vals
     *
     * @param other_val
     * @return
     */
    virtual PTR(Val) add_to(PTR(Val) other_val) = 0;

    /** Attempts to multiply two vals
     *
     * @param other_val
     * @return
     */
    virtual PTR(Val) mult_with(PTR(Val) other_val) = 0;

    /** Returns an expr from the given val.
     *
     * @return
     */
    virtual PTR(Expr) to_expr() = 0;

    /** Returns a string representation of the val it is called on.
     *
     * @return
     */
    virtual std::string to_string() = 0;

    /** Attempts to return the bool of a value.
     *
     * @return
     */
    virtual bool is_true() = 0;

    /** Attemps to call a function.
     *
     * @param actual_arg
     * @return
     */
    virtual PTR(Val)call(PTR(Val) actual_arg)=0;

    /** Attemps to call a function, using step mode.
     *
     * @param actual_arg
     * @return
     */
    virtual void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) = 0;
};

/** Treats a number as a Value rather than an Expression.
 *
 * Note* inherits all virtual methods from Class Val.
 */
class NumVal : public Val {
public:
    int rep;

    NumVal(int rep);

    bool equals(PTR(Val) val);
    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    PTR(Expr) to_expr();
    std::string to_string();
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);

};

/** Treats a bool as a val rather than an expression.
 *
 *  Note* inherits all virtual methods from Class Val.
 */
class BoolVal : public Val {
public:
    bool rep;

    BoolVal(bool rep);

    bool equals(PTR(Val) val);
    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    PTR(Expr) to_expr();
    std::string to_string();
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
};

/**
 * Treats functions as values rather than expressions.
 * FunVal used in conjunction with CallVal to call functions.
 *
 * Note* inherits all virtual methods from Class Val.
 */
class FunVal : public Val {
public:
    std::string formal_arg;
    PTR(Expr) body;
    PTR(Env) env;

    FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env);

    bool equals(PTR(Val) val);
    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    PTR(Expr) to_expr();
    std::string to_string();
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);


};

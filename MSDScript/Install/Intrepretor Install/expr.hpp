#pragma once

#include <iostream>
#include <string>
#include "pointer.hpp"

class Val;
class Env;

class Expr {
public:
    /** Sets up expressions so that they can be compared using boolean
     *  expressions.
     *
     * @param e an Expr
     * @return
     */
    virtual bool equals(PTR(Expr) e) = 0;

    /** Calls interpret on an environment.
     *
     * @param env
     * @return
     */
    virtual PTR(Val) interp(PTR(Env) env) = 0;

    /** Returns a new Expr with the same values, substituted where possible.
     *
     * @param var
     * @param val
     * @return
     */
    virtual PTR(Expr) subst(std::string var, PTR(Val) val) = 0;

    /** Checks if the expression contains a variable, returns a boolean of the check.
     *
     * @return boolean
     */
    virtual bool hasVariable()= 0;

    /** Attempts to optimize the expression. Returns either an
     *  equivalent expression or an optimized one.
     *
     * @return Expr
     */
    virtual PTR(Expr) optimize()=0;

    /** Creates string representation of expressions.
     *
     * @return
     */
    virtual std::string toString() = 0;

    /** Interp that doesn't remember all of the variables, passes what it
     * needs so that there is no stack overflow for input that would take up a lot
     * of stack space, for example a recursive function that counts down from 100.
     *
     */
    virtual void step_interp() = 0;
};

/** NumExpr Class defines numbers as expressions.
 *
 * Note* inherits all virtual methods from Class Expr.
 */
class NumExpr : public Expr {
public:
    int rep;
    PTR(Val) val;

    NumExpr(int rep);
    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool hasVariable();
    PTR(Expr) optimize();
    std::string toString();
    void step_interp();
};
/** AddExpr Class defines the adding of two exprs.
 *
 * Note* inherits all virtual methods from Class Expr.
 */
class AddExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;

    AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);
    bool equals(PTR(Expr) e);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool hasVariable();
    PTR(Expr) optimize();
    std::string toString();
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
};

/** MultExpr Class defines the Multiplying of two exprs.
 *  Member variables are the left hand side expression and the right hand
 *  side expressions to be multiplied.
 *
 * Note* inherits all virtual methods from Class Expr.
 */
class MultExpr : public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;

    MultExpr(PTR(Expr) lhs,PTR(Expr) rhs);

    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool hasVariable();
    PTR(Expr) optimize();
    std::string toString();
    void step_interp();
};

/** VarExpr defines the use of variables.
 *  Member variable of string name is variable name.
 *
 *  Note* inherits all virtual methods from Class Expr.
 */
class VarExpr : public Expr {
public:
    std::string name;

    VarExpr(std::string name);

    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var,PTR(Val) val);
    bool hasVariable();
    PTR(Expr) optimize();
    std::string toString();
    void step_interp();
};

/** LetExpr defines the use of Let Expressions.
 *  example _let x = 4 _in x + 2
 *  Member variables:
 *  _let x, x is the name
 *  rhs is the 4
 *  body is x + 2
 *
 *  Note* inherits all virtual methods from Class Expr.
 */
class LetExpr : public Expr{
public:
    std::string variable;
    PTR(Expr) rhs;
    PTR(Expr) body;

    LetExpr(std::string variable, PTR(Expr) rhs, PTR(Expr) body);

    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool hasVariable();
    PTR(Expr) optimize();
    std::string toString();
    void step_interp();

};

/** BoolExpr defines boolean expressions.
 *      Member Variables: rep = boolean
 *
 *
 *  Note* inherits all virtual methods from Class Expr.
 */
class BoolExpr : public Expr {
public:
    bool rep;

    BoolExpr(bool rep);

    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool hasVariable();
    PTR(Expr) optimize();
    std::string toString();
    void step_interp();

};

/** IfExpr Class defines If Expressions.
 *  ex. _if x == 5 _then 2 _else x + -1
 *      Member Variables:
 *          Condition = x == 5
 *          conTrue = 2
 *          conFalse = x + -1
 *
 * Note* inherits all virtual methods from Class Expr.
 */
class IfExpr : public Expr {
public:
    IfExpr(PTR(Expr) condition, PTR(Expr) conTrue, PTR(Expr) conFalse);

    PTR(Expr) condition;
    PTR(Expr) conTrue;
    PTR(Expr) conFalse;

    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env)env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool hasVariable();
    PTR(Expr) optimize();
    std::string toString();
    void step_interp();
};

/** EqualsExpr does the boolean == expression on two expressions.
 *      Member variables:
 *          lhs, left hand side expression.
 *          rhs, right hand side expression.
 * Note* inherits all virtual methods from Class Expr.
 */
class EqualsExpr : public Expr {
public:
    EqualsExpr(PTR(Expr) lhs, PTR(Expr) rhs);

     PTR(Expr) lhs;
     PTR(Expr) rhs;

    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool hasVariable();
    PTR(Expr) optimize();
    std::string toString();
    void step_interp();
};


/** Defines Function Expers
 *  ex. _let f = _fun (x) x + 10 _in f (5)
 *  Member Variables:
 *      formal_arg = function name (f)
 *      body = function body (x + 10)
 *
 * Note* inherits all virtual methods from Class Expr.
 */
class FunExpr : public Expr {
public:
    FunExpr(std::string formal_arg, PTR(Expr) body);

    std::string formal_arg;
    PTR(Expr) body;

    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool hasVariable();
    PTR(Expr) optimize();
    std::string toString();
    void step_interp();

};

/** Defines all call Expressions
 *  ex. _let f = _fun (x) x + 10 _in f (5)
 *  Member Variables:
 *      to_be_called = function name (f)
 *      actual_arg = (5)
 *          this is the value fed into the function
 *
 * Note* inherits all virtual methods from Class Expr.
 */
class CallExpr : public Expr {
public:
    CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg);

    PTR(Expr) to_be_called;
    PTR(Expr) actual_arg;



    bool equals(PTR(Expr) e);
    PTR(Val) interp(PTR(Env) env);
    PTR(Expr) subst(std::string var, PTR(Val) val);
    bool hasVariable();
    PTR(Expr) optimize();
    std::string toString();
    void step_interp();

};





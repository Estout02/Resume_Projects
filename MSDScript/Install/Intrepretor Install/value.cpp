#include "expr.hpp"
#include <stdexcept>
#include "value.hpp"
#include "catch.hpp"
#include "pointer.hpp"
#include "env.hpp"
#include "cont.hpp"
#include "step.hpp"

NumVal::NumVal(int rep) {
    this->rep = rep;
}

bool NumVal::equals(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr)
        return false;
    else
        return rep == other_num_val->rep;
}

PTR(Val) NumVal::add_to(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr)
        throw std::runtime_error("Integer addition between integers only.");
    else
        return NEW(NumVal)((unsigned)rep + (unsigned)other_num_val->rep);
}

PTR(Val) NumVal::mult_with(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr)
        throw std::runtime_error("Integer multiplication between integers only.");
    else
        return NEW(NumVal)((unsigned)rep * (unsigned)other_num_val->rep);
}

PTR(Expr) NumVal::to_expr() {
    return NEW(NumExpr)(rep);
}

std::string NumVal::to_string() {
    return std::to_string(rep);
}

bool NumVal::is_true() {
    throw std::runtime_error("Number is not true or false, Not a bool.");
}

PTR(Val) NumVal::call(PTR(Val) actual_arg) {
    throw std::runtime_error("Value, no function here.");
}

void NumVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    throw std::runtime_error("Tried to call_step Numval.");
}

BoolVal::BoolVal(bool rep) {
    this->rep = rep;
}

bool BoolVal::equals(PTR(Val) other_val) {
    PTR(BoolVal) other_bool_val = CAST(BoolVal)(other_val);
    if (other_bool_val == nullptr)
        return false;
    else
        return rep == other_bool_val->rep;
}

PTR(Val) BoolVal::add_to(PTR(Val) other_val) {
    throw std::runtime_error("No adding booleans.");
}

PTR(Val) BoolVal::mult_with(PTR(Val) other_val) {
    throw std::runtime_error("no multiplying booleans");
}

PTR(Expr) BoolVal::to_expr() {
    return NEW(BoolExpr)(rep);
}

std::string BoolVal::to_string() {
    if (rep)
        return "_true";
    else
        return "_false";
}

bool BoolVal::is_true() {
    return this->rep;
}

PTR(Val) BoolVal::call(PTR(Val) actual_arg) {
    throw std::runtime_error("Bool, No function here.");
}

void BoolVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    throw std::runtime_error("Tried to call_step on bool.");

}

FunVal::FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env){
this->formal_arg = formal_arg;
this->body = body;
this->env = env;
}

PTR(Val) FunVal::call(PTR(Val) actual_arg) {
    return body->interp(NEW(ExtendedEnv)(formal_arg, actual_arg, env));
}

bool FunVal::equals(PTR(Val) other_val) {
    PTR(FunVal) other_fun_val = CAST(FunVal)(other_val);
    if (other_fun_val == nullptr)
        return false;
    else
        return (this->formal_arg == other_fun_val->formal_arg||this->body->equals(other_fun_val->body));
}

PTR(Val) FunVal::add_to(PTR(Val) other_val) {
    throw std::runtime_error("No adding function vals.");
}

PTR(Val) FunVal::mult_with(PTR(Val) other_val) {
    throw std::runtime_error("No multiplying function vals.");
}

PTR(Expr) FunVal::to_expr() {
    return NEW(FunExpr)(this->formal_arg,this->body);
}

std::string FunVal::to_string() {
    return "_fun (" + this->formal_arg + ") " + this->body->toString();
}

bool FunVal::is_true() {
    throw std::runtime_error("Function is not true or false, Not a bool.");
}

void FunVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(formal_arg, actual_arg_val, env);
    Step::cont = rest;
}

//

TEST_CASE( "values equals" ) {
    CHECK( (NEW(NumVal)(5))->equals(NEW(NumVal)(5)) );
    CHECK( ! (NEW(NumVal)(7))->equals(NEW(NumVal)(5)) );

    CHECK( (NEW(BoolVal)(true))->equals(NEW(BoolVal)(true)) );
    CHECK( ! (NEW(BoolVal)(true))->equals(NEW(BoolVal)(false)) );
    CHECK( ! (NEW(BoolVal)(false))->equals(NEW(BoolVal)(true)) );

    CHECK( ! (NEW(NumVal)(7))->equals(NEW(BoolVal)(false)) );
    CHECK( ! (NEW(BoolVal)(false))->equals(NEW(NumVal)(8)) );
    CHECK( (NEW(NumVal)(5))->equals(NEW(NumVal)(5)) );
    CHECK( ! (NEW(NumVal)(7))->equals(NEW(NumVal)(5)) );

    CHECK( (NEW(BoolVal)(true))->equals(NEW(BoolVal)(true)) );
    CHECK( ! (NEW(BoolVal)(true))->equals(NEW(BoolVal)(false)) );
    CHECK( ! (NEW(BoolVal)(false))->equals(NEW(BoolVal)(true)) );

    CHECK( ! (NEW(NumVal)(7))->equals(NEW(BoolVal)(false)) );
    CHECK( ! (NEW(BoolVal)(false))->equals(NEW(NumVal)(8)) );

    CHECK( (NEW(FunVal)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(EmptyEnv)()))
                   ->equals(NEW(FunVal)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(EmptyEnv)())) );
    CHECK( ! (NEW(FunVal)("x", NEW(NumExpr)(4), NEW(EmptyEnv)()))
            ->equals(NEW(BoolVal)(false)) );
//    CHECK( (NEW(FunVal)("x", NEW(AddExpr)(NEW(NumExpr)(3), NEW(NumExpr)(4)), NEW(EmptyEnv)()))->to_string()
//           == "(_fun (x) (3 + 4))" );
}

TEST_CASE( "add_to" ) {

    CHECK ((NEW(NumVal)(5))->add_to(NEW(NumVal)(8))->equals(NEW(NumVal)(13)));
    CHECK ((NEW(NumVal)(5))->add_to(NEW(NumVal)(8))->equals(NEW(NumVal)(13)));
}


TEST_CASE( "value to_expr" ) {
    CHECK( (NEW(NumVal)(5))->to_expr()->equals(NEW(NumExpr)(5)) );
    CHECK( (NEW(BoolVal)(true))->to_expr()->equals(NEW(BoolExpr)(true)) );
    CHECK( (NEW(BoolVal)(false))->to_expr()->equals(NEW(BoolExpr)(false)) );
}

TEST_CASE( "value to_string" ) {
    CHECK( (NEW(NumVal)(5))->to_string() == "5" );
    CHECK( (NEW(BoolVal)(true))->to_string() == "_true" );
    CHECK( (NEW(BoolVal)(false))->to_string() == "_false" );
}



TEST_CASE( "value is_true" ) {

    CHECK( (NEW(BoolVal)(true))->is_true() );
    CHECK( ! (NEW(BoolVal)(false))->is_true() );

}

TEST_CASE( "value call" ) {
    CHECK( (NEW(FunVal)("x", NEW(NumExpr)(3), NEW(EmptyEnv)()))->call(NEW(NumVal)(4))
                   ->equals(NEW(NumVal)(3)) );
    CHECK( (NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)), NEW(EmptyEnv)()))->call(NEW(NumVal)(4))
                   ->equals(NEW(NumVal)(7)) );
    CHECK( (NEW(FunVal)("x", NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)),NEW(EmptyEnv)()))->call(NEW(NumVal)(4))
                   ->equals(NEW(NumVal)(12)) );
}



#include "parse.hpp"
#include "step.hpp"
#include "cont.hpp"
#include "value.hpp"
#include "env.hpp"
#include "expr.hpp"
#include "catch.hpp"

NumExpr::NumExpr(int rep) {
    this->rep = rep;
    val = NEW(NumVal)(rep);
}


bool NumExpr::equals(PTR(Expr) e) {
    PTR(NumExpr) n = CAST(NumExpr)(e);
    if (n == NULL)
        return false;
    else
        return rep == n->rep;
}

PTR(Val) NumExpr::interp(PTR(Env) env) {
    return val;
}

PTR(Expr) NumExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(NumExpr)(rep);
}

bool NumExpr::hasVariable() {
    return false;
}

PTR(Expr) NumExpr::optimize() {
    return NEW(NumExpr)(rep);
}

std::string NumExpr::toString() {
    return std::to_string(rep);
}


AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool AddExpr::equals(PTR(Expr) e) {
    PTR(AddExpr) a = CAST(AddExpr)(e);
    if (a == NULL)
        return false;
    else
        return (lhs->equals(a->lhs)
                && rhs->equals(a->rhs));
}

PTR(Val) AddExpr::interp(PTR(Env) env) {
    PTR(Val) lhs_val = lhs->interp(env);
    PTR(Val) rhs_val = rhs->interp(env);
    return lhs_val->add_to(rhs_val);
}

PTR(Expr) AddExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(AddExpr)(lhs->subst(var, new_val),
                        rhs->subst(var, new_val));
}

bool AddExpr::hasVariable() {
    return lhs->hasVariable() || rhs->hasVariable();
}

std::string AddExpr::toString() {
    return lhs->toString() + "" + " + " + "" + rhs->toString();
}


PTR(Expr) AddExpr::optimize() {
    PTR(Expr) lhsSimp = lhs->optimize();
    PTR(Expr) rhsSimp = rhs->optimize();
    if (!lhsSimp->hasVariable() && !rhsSimp->hasVariable())
        return lhsSimp->interp(NEW(EmptyEnv)())->add_to(rhsSimp->interp(NEW(EmptyEnv)()))->to_expr();
    else
        return NEW(AddExpr)(lhsSimp, rhsSimp);
}


MultExpr::MultExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool MultExpr::equals(PTR(Expr) e) {
    PTR(MultExpr) m = CAST(MultExpr)(e);
    if (m == NULL)
        return false;
    else
        return (lhs->equals(m->lhs)
                && rhs->equals(m->rhs));
}

PTR(Val) MultExpr::interp(PTR(Env) env) {
    return lhs->interp(env)->mult_with(rhs->interp(env));
}

PTR(Expr) MultExpr::subst(std::string input, PTR(Val) value) {
    return NEW(MultExpr)(lhs->subst(input, value),
                         rhs->subst(input, value));
}

bool MultExpr::hasVariable() {
    return lhs->hasVariable() || rhs->hasVariable();
}

PTR(Expr) MultExpr::optimize() {

    PTR(Expr) lhsSimp = lhs->optimize();
    PTR(Expr) rhsSimp = rhs->optimize();
    if (!lhsSimp->hasVariable() && !rhsSimp->hasVariable())
        return lhsSimp->interp(NEW(EmptyEnv)())->mult_with(rhsSimp->interp(NEW(EmptyEnv)()))->to_expr();
    else
        return NEW(MultExpr)(lhsSimp, rhsSimp);
}

std::string MultExpr::toString() {
    return lhs->toString() + "" + " * " + "" + rhs->toString();
}

void MultExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env; /* no-op, so could omit */
    Step::cont = NEW(RightThenMultCont)(rhs, Step::env, Step::cont);

}

VarExpr::VarExpr(std::string name) {
    this->name = name;
}

bool VarExpr::equals(PTR(Expr) e) {
    PTR(VarExpr) v = CAST(VarExpr)(e);
    if (v == NULL)
        return false;
    else
        return name == v->name;
}

PTR(Val) VarExpr::interp(PTR(Env) env) {
    return env->lookup(name);
}

PTR(Expr) VarExpr::subst(std::string var, PTR(Val) new_value) {
    if (name == var) {
        return new_value->to_expr();
    } else {
        return NEW(VarExpr)(name);
    }
}

bool VarExpr::hasVariable() {
    return true;
}

PTR(Expr) VarExpr::optimize() {
    return NEW(VarExpr)(name);
}

std::string VarExpr::toString() {
    return this->name;
}

void VarExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = Step::env->lookup(name);
    Step::cont = Step::cont;
}

LetExpr::LetExpr(std::string name, PTR(Expr) rhs, PTR(Expr) expression) {
    this->variable = name;
    this->rhs = rhs;
    this->body = expression;
}

bool LetExpr::equals(PTR(Expr) e) {
    PTR(LetExpr) l = CAST(LetExpr)(e);
    if (l == NULL)
        return false;
    else
        return (this->rhs->equals(l->rhs)
                && this->body->equals(l->body) &&
                this->variable == (l->variable));
}

PTR(Val) LetExpr::interp(PTR(Env) env) {
    PTR(Val) rhs_val = rhs->interp(env);
    PTR(Env) new_env = NEW(ExtendedEnv)(variable, rhs_val, env);
    return body->interp(new_env);
}


PTR(Expr) LetExpr::subst(std::string input, PTR(Val) value) {
    //different variables subs both body and rhs, letx in lety
    //if same subs rhs, rarely need to subs body, letx in letx
    if (input == variable) {
        return NEW(LetExpr)(variable, rhs->subst(variable, value), body);
    }
    return NEW(LetExpr)(variable, rhs->subst(input, value), body->subst(input, value));
}

bool LetExpr::hasVariable() {
    return true;
}

PTR(Expr) LetExpr::optimize() {
    PTR(Expr) rhsSimp = rhs->optimize();

    if (rhsSimp->hasVariable()) {
        return NEW(LetExpr)(variable, rhsSimp, body->optimize());
    } else {
        return body->subst(variable, rhsSimp->interp(NEW(EmptyEnv)()))->optimize();
    }
}


std::string LetExpr::toString() {
    return ("_let " + variable + " = " + rhs->toString() +
            " _in " + body->toString());

}

void LetExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = rhs;
    Step::env = Step::env;
    Step::cont = NEW(LetBodyCont)(variable, body, Step::env, Step::cont);
}

BoolExpr::BoolExpr(bool rep) {
    this->rep = rep;
}

bool BoolExpr::equals(PTR(Expr) e) {
    PTR(BoolExpr) b = CAST(BoolExpr)(e);
    if (b == NULL)
        return false;
    else
        return rep == b->rep;
}

PTR(Val) BoolExpr::interp(PTR(Env) env) {
    return NEW(BoolVal)(rep);
}

PTR(Expr) BoolExpr::subst(std::string var, PTR(Val) val) {
    return NEW(BoolExpr)(rep);
}

bool BoolExpr::hasVariable() {
    return false;
}

std::string BoolExpr::toString() {
    if (rep) {
        return "_true";
    } else {
        return "_false";
    }
}

PTR(Expr) BoolExpr::optimize() {
    return NEW(BoolExpr)(this->rep);
}

void BoolExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(BoolVal)(rep);
    Step::cont = Step::cont;
}

IfExpr::IfExpr(PTR(Expr) condition, PTR(Expr) conTrue, PTR(Expr) conFalse) {
    this->condition = condition;
    this->conTrue = conTrue;
    this->conFalse = conFalse;
}

bool IfExpr::equals(PTR(Expr) e) {
    PTR(IfExpr) b = CAST(IfExpr)(e);
    if (b == NULL)
        return false;
    else {
        if (condition == b->condition && conTrue == b->conTrue && conFalse == b->conFalse) {
            return true;
        }
        return false;
    }
}

PTR(Val) IfExpr::interp(PTR(Env) env) {
    PTR(Val) test_val = condition->interp(env);
    if (this->condition->interp(env)->is_true()) {
        return conTrue->interp(env);
    } else
        return conFalse->interp(env);
}

PTR(Expr) IfExpr::subst(std::string var, PTR(Val) val) {
    return NEW(IfExpr)(this->condition->subst(var, val),
                       this->conTrue->subst(var, val),
                       this->conFalse->subst(var, val));

}

bool IfExpr::hasVariable() {
    return (this->condition->hasVariable() | this->conTrue->hasVariable() | this->conFalse->hasVariable());
}

PTR(Expr) IfExpr::optimize() {
    PTR(Expr) testExp = condition->optimize();
    if (this->condition->hasVariable()) {
        return NEW(IfExpr)(testExp, conTrue->optimize(), conFalse->optimize());
    }
    if (this->condition->interp(NEW(EmptyEnv)())->is_true()) {
        return conTrue->optimize();
    } else
        return conFalse->optimize();
}

std::string IfExpr::toString() {
    return ("_if " + condition->toString() + " _then " + conTrue->toString() +
            " _else " + conFalse->toString());

}

EqualsExpr::EqualsExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool EqualsExpr::equals(PTR(Expr) e) {
    PTR(EqualsExpr) b = CAST(EqualsExpr)(e);
    if (b == NULL)
        return false;
    else {
        return rhs->equals(b->rhs) && lhs->equals(b->lhs);
    }
}

PTR(Val) EqualsExpr::interp(PTR(Env) env) {
    return NEW(BoolVal)(this->lhs->interp(env)->equals(rhs->interp(env)));
}

PTR(Expr) EqualsExpr::subst(std::string var, PTR(Val) val) {
    return NEW(EqualsExpr)(this->lhs->subst(var, val),
                           this->rhs->subst(var, val));
}

bool EqualsExpr::hasVariable() {
    return (this->lhs->hasVariable() && this->rhs->hasVariable());
}

PTR(Expr) EqualsExpr::optimize() {

    PTR(Expr) opLhs = lhs->optimize();
    PTR(Expr) opRhs = rhs->optimize();

    if (opLhs->equals(opRhs)) {
        return NEW(BoolExpr)(true);
    }
    if (opLhs->hasVariable() || opRhs->hasVariable()) {
        return NEW(EqualsExpr)(opLhs, opRhs);
    } else {
        return NEW(BoolExpr)(false);
    }
}

std::string EqualsExpr::toString() {
    return (lhs->toString() + " == " + rhs->toString());
}

void EqualsExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env; /* no-op, so could omit */
    Step::cont = NEW(RightThenCompCont)(rhs, Step::env, Step::cont);

}


FunExpr::FunExpr(std::string formal_arg, PTR(Expr) body) {
    this->formal_arg = formal_arg;
    this->body = body;
}

bool FunExpr::equals(PTR(Expr) e) {
    PTR(FunExpr) b = CAST(FunExpr)(e);
    if (b == nullptr)
        return false;
    else {
        return formal_arg == (b->formal_arg) && body->equals(b->body);
    }
}

PTR(Val) FunExpr::interp(PTR(Env) env) {
    return NEW(FunVal)(formal_arg, body, env);
}

PTR(Expr) FunExpr::subst(std::string var, PTR(Val) val) {
    if (var != this->formal_arg) {
        return NEW(FunExpr)(formal_arg, body->subst(var, val));
    }
    return NEW(FunExpr)(formal_arg, body);
}

bool FunExpr::hasVariable() {
    return true;
}

PTR(Expr) FunExpr::optimize() {
    return NEW(FunExpr)(formal_arg, body->optimize());
}

std::string FunExpr::toString() {
    return "(_fun (" + this->formal_arg + ") " + this->body->toString() + ')';
}

void FunExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(FunVal)(formal_arg, body, Step::env);
    Step::cont = Step::cont;
}

CallExpr::CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_arg) {
    this->to_be_called = to_be_called;
    this->actual_arg = actual_arg;
}

bool CallExpr::equals(PTR(Expr) e) {
    PTR(CallExpr) b = CAST(CallExpr)(e);
    if (b == NULL)
        return false;
    else {
        return to_be_called->equals(b->to_be_called) && actual_arg->equals(b->actual_arg);
    }
}

PTR(Val) CallExpr::interp(PTR(Env) env) {
    return to_be_called->interp(env)->call(actual_arg->interp(env));
}

PTR(Expr) CallExpr::subst(std::string var, PTR(Val) val) {
    return NEW(CallExpr)(to_be_called->subst(var, val), actual_arg->subst(var, val));
}

bool CallExpr::hasVariable() {
    return (to_be_called->hasVariable() || actual_arg->hasVariable());
}

PTR(Expr) CallExpr::optimize() {
    return NEW(CallExpr)(to_be_called->optimize(), actual_arg->optimize());
}

std::string CallExpr::toString() {
    return this->to_be_called->toString() + '(' + this->actual_arg->toString() + ')';
}

void CallExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = to_be_called;
    Step::cont = NEW(ArgThenCallCont)(actual_arg, Step::env, Step::cont);
}


void AddExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env; /* no-op, so could omit */
    Step::cont = NEW(RightThenAddCont)(rhs, Step::env, Step::cont);
}

void NumExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(NumVal)(rep);
    Step::cont = Step::cont; /* no-op */
}

void IfExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = condition;
    Step::env = Step::env;
    Step::cont = NEW(IfBranchCont)(conTrue, conFalse, Step::env, Step::cont);
}


TEST_CASE("equals") {
    CHECK((NEW(NumExpr)(1))->equals(NEW(NumExpr)(1)));
    CHECK(!(NEW(NumExpr)(1))->equals(NEW(NumExpr)(2)));
    CHECK(!(NEW(NumExpr)(1))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(4))));
    CHECK((NEW(VarExpr)("x"))->equals(NEW(VarExpr)("x")));
    CHECK(!(NEW(VarExpr)("x"))->equals(NEW(NumExpr)(5)));
    CHECK(!(NEW(AddExpr)(NEW(NumExpr)(8), NEW(NumExpr)(9)))
            ->equals(NEW(NumExpr)(8)));
}

TEST_CASE("value") {
//    CHECK((NEW( NumExpr)(10))->interp(NEW(EmptyEnv)())->equals(NEW( NumVal)(10)));
//    CHECK((NEW( AddExpr)(NEW( NumExpr)(3), NEW(NumExpr)(2)))->interp(NEW(EmptyEnv)())
//                  ->equals(NEW( NumVal)(5)));
//    CHECK((NEW( MultExpr)(NEW( NumExpr)(3), NEW(NumExpr)(2)))->interp(NEW(EmptyEnv)())
//                  ->equals(NEW( NumVal)(6)));
}

TEST_CASE("subst") {
    CHECK((NEW(NumExpr)(10))->subst("dog", NEW(NumVal)(3))
                  ->equals(NEW(NumExpr)(10)));
    CHECK((NEW(VarExpr)("fish"))->subst("dog", NEW(NumVal)(3))
                  ->equals(NEW(VarExpr)("fish")));
    CHECK((NEW(VarExpr)("dog"))->subst("dog", NEW(NumVal)(3))
                  ->equals(NEW(NumExpr)(3)));
    CHECK((NEW(VarExpr)("dog"))->subst("dog", NEW(BoolVal)(true))
                  ->equals(NEW(BoolExpr)(true)));
    CHECK((NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
                  ->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))));
    CHECK((NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))
                  ->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))));
}







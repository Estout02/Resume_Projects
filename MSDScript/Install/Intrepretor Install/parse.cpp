#include <iostream>
#include <sstream>
#include "parse.hpp"
#include "catch.hpp"
#include "pointer.hpp"
#include "expr.hpp"
#include "env.hpp"
#include "value.hpp"
#include "step.hpp"

static PTR(Expr) parse_expr(std::istream &in);

static PTR(Expr) parse_addend(std::istream &in);

static PTR(Expr) parse_inner(std::istream &in);

static PTR(Expr) parse_number(std::istream &in);

static PTR(Expr) parse_variable(std::istream &in);

static std::string parse_keyword(std::istream &in);

static std::string parse_alphabetic(std::istream &in, std::string prefix);

static PTR(Expr) parse_multicand(std::istream &in);

static char peek_after_spaces(std::istream &in);

// Take an input stream that contains an expression,
// and returns the parsed representation of that expression.
// Throws `runtime_error` for parse errors.



/**
 * Takes an input stream that contains an expression,
 * and returns the parsed representation of that expression.
 * Throws `runtime_error` for parse errors.
 *
 * @param in inputstream
 * @return PTR to an Expression
 */
PTR(Expr) parse(std::istream &in) {
    PTR(Expr) e = parse_expr(in);

    // This peek is currently redundant, since we would have
    // consumed whitespace to decide that the expression
    // doesn't continue.
    char c = peek_after_spaces(in);
    if (in.eof())
        return e;

    return e;
}



/**
 * Takes an input stream that starts with an expression,
 * consuming the largest initial expression possible.
 *
 * @param in
 * @return
 */
static PTR(Expr) parse_expr(std::istream &in) {
    PTR(Expr) e = parse_addend(in);

    char c = peek_after_spaces(in);

    if (c == '=') {
        in.get();
        char c = peek_after_spaces(in);
        if (c == '=') {
            in.get();
            PTR(Expr) equalTo = parse_expr(in);
            e = NEW(EqualsExpr)(e, equalTo);
        }

    }
    if (c == '+') {
        in >> c;
        PTR(Expr) rhs = parse_expr(in);
        e = NEW(AddExpr)(e,rhs);
    }

    return e;
}


/**Takes an input stream that starts with an addend,
 * consuming the largest initial addend possible, where
 * an addend is an expression that does not have `+`
 * except within nested expressions (like parentheses).
 *
 * @param in
 * @return
 */
static PTR(Expr) parse_addend(std::istream &in) {
    PTR(Expr) e = parse_multicand(in);


    char c = peek_after_spaces(in);
    if (c == '*') {
        c = in.get();
        PTR(Expr) rhs = parse_addend(in);
        e = NEW(MultExpr)(e, rhs);
    }

    return e;
}

/** Parses on letters, creates expression of the parsed word.
 *
 * @param in
 * @param prefix
 * @return
 */
static std::string parse_alphabetic(std::istream &in, std::string prefix) {
    std::string name = prefix;
    while (1) {
        char c = in.peek();
        if (!isalpha(c))
            break;
        name += in.get();
    }
    return name;
}

/** Parses on Keyword and creates corresponding expr.
 *
 * @param in
 * @return
 */
static std::string parse_keyword(std::istream &in) {
    in.get(); // consume `_`
    return parse_alphabetic(in, "_");
}

static PTR(Expr) parse_number(std::istream &in) {
    int num = 0;
    in >> num;
    return NEW(NumExpr)(num);
}

static PTR(Expr) parse_multicand(std::istream &in) {
    PTR(Expr) e = parse_inner(in);
    while (in.peek() == '(') {
        in.get();
        peek_after_spaces(in);
        PTR(Expr) actual_arg = parse_expr(in);
        e = NEW(CallExpr)(e, actual_arg);
        if (in.peek() == ')') {
            in.get();
        }

    }
    return e;
}

static PTR(Expr) parse_fun(std::istream &in) {
    peek_after_spaces(in);
    if (in.peek() == '(') {
        in.get();
    } else {
        throw std::runtime_error("( was expected after _fun.");
    }
    peek_after_spaces(in);
    std::string vName = parse_alphabetic(in, "");
    peek_after_spaces(in);
    if (in.peek() == ')') {
        in.get();
    } else {
        throw std::runtime_error("), expected a close parenthesis.");
    }
    peek_after_spaces(in);
    PTR(Expr) body = parse_expr(in);
    return NEW(FunExpr)(vName, body);
}

/** Parses something with no immediate `+` or `*` from `in`.
 *
 * @param in
 * @return
 */
static PTR(Expr) parse_inner(std::istream &in) {
    PTR(Expr) e;
    char c = peek_after_spaces(in);
    if (c == '(') {
        c = in.get();
        e = parse_expr(in);
        c = peek_after_spaces(in);
        if (c == ')')
            c = in.get();
        else
            throw std::runtime_error("), expected a close parenthesis");
    } else if (c == '-' || isdigit(c)) {
        e = parse_number(in);
    } else if (isalpha(c)) {
        e = parse_variable(in);
    } else if (c == '_') {
        std::string _keyword = parse_keyword(in);
        if (_keyword == "_let") {
            peek_after_spaces(in);
            std::string vName = parse_alphabetic(in, "");
            peek_after_spaces(in);
            char d = in.get();
            PTR(Expr) xtemp = parse_expr(in);
            std::string _in = parse_keyword(in);
            PTR(Expr) processTemp = parse_expr(in);
            e = NEW(LetExpr)(vName, xtemp, processTemp);
        } else if (_keyword == "_true") {
            e = NEW(BoolExpr)(true);
        } else if (_keyword == "_false") {
            e = NEW(BoolExpr)(false);
        } else if (_keyword == "_if") {
            peek_after_spaces(in);
            PTR(Expr) condition = parse_expr(in);
            peek_after_spaces(in);
            std::string _then = parse_keyword(in);
            peek_after_spaces(in);
            PTR(Expr) conditionTrue = parse_expr(in);
            peek_after_spaces(in);
            std::string _else = parse_keyword(in);
            peek_after_spaces(in);
            PTR(Expr) conditionFalse = parse_expr(in);
            e = NEW(IfExpr)(condition, conditionTrue, conditionFalse);
        } else if (_keyword == "_fun") {
            e = parse_fun(in);
        }

    } else {
        throw std::runtime_error((std::string) "expected a digit or open parenthesis at " + c);
    }

    return e;
}


/** Parses an expression, assuming that `in` starts with a letter.
 *
 * @param in
 * @return
 */
static PTR(Expr) parse_variable(std::istream &in) {
    std::string name = "";
    while (true) {
        char c = in.peek();
        if (!isalpha(c))
            break;
        name += in.get();
    }
    return NEW(VarExpr)(name);
}

/** Like in.peek(), but consume an whitespace at the start of `in`.
 *
 * @param in
 * @return
 */
static char peek_after_spaces(std::istream &in) {
    char c;
    while (true) {
        c = in.peek();
        if (!isspace(c))
            break;
        c = in.get();
    }
    return c;
}

/* for tests */
static PTR(Expr) parse_str(std::string s) {
    std::istringstream in(s);
    return parse(in);
}

/* for tests */
static std::string parse_str_error(std::string s) {
    std::istringstream in(s);
    try {
        (void) parse(in);
        return "";
    } catch (std::runtime_error exn) {
        return exn.what();
    }
}

TEST_CASE("See if this works") {
    PTR(Expr) ten_plus_one = NEW(AddExpr)(NEW( NumExpr)(10),NEW(NumExpr)(1));

//    CHECK (parse_str_error(" ( 1 ") == "expected a close parenthesis");

    CHECK(parse_str("10")->equals(NEW(NumExpr)(10)));
    CHECK(parse_str("(10)")->equals(NEW(NumExpr)(10)));
    CHECK(parse_str("10+1")->equals(ten_plus_one));
    CHECK(parse_str("(10+1)")->equals(ten_plus_one));
    CHECK(parse_str("(10)+1")->equals(ten_plus_one));
    CHECK(parse_str("10+(1)")->equals(ten_plus_one));
    CHECK(parse_str("1+2*3")->equals(NEW(AddExpr)(NEW(NumExpr)(1),
                                                NEW(MultExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)))));
    CHECK(parse_str("1*2+3")->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(1),NEW(NumExpr)(2)),
                                                NEW(NumExpr)(3))));
    CHECK(parse_str("4*2*3")->equals(NEW(MultExpr)(NEW(NumExpr)(4),
                                                 NEW(MultExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)))));
    CHECK(parse_str("4+2+3")->equals(NEW(AddExpr)(NEW(NumExpr)(4),
                                                NEW(AddExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)))));
    CHECK(parse_str("4*(2+3)")->equals(NEW(MultExpr)(NEW(NumExpr)(4),
                                                   NEW(AddExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)))));
    CHECK(parse_str("(2+3)*4")->equals(NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)),
                                                   NEW(NumExpr)(4))));
    CHECK(parse_str("xyz")->equals(NEW(VarExpr)("xyz")));
    CHECK(parse_str("xyz+1")->equals(NEW(AddExpr)(NEW(VarExpr)("xyz"),NEW(NumExpr)(1))));

//    CHECK (parse_str_error("!") == "expected a digit or open parenthesis at !");
//    CHECK (parse_str_error("(1") == "expected a close parenthesis");

    CHECK(parse_str(" 10 ")->equals(NEW(NumExpr)(10)));
    CHECK(parse_str(" (  10 ) ")->equals(NEW(NumExpr)(10)));
    CHECK(parse_str(" 10  + 1")->equals(ten_plus_one));
    CHECK(parse_str(" ( 10 + 1 ) ")->equals(ten_plus_one));
    CHECK(parse_str(" 11 * ( 10 + 1 ) ")->equals(NEW(MultExpr)(NEW(NumExpr)(11),
                                                              ten_plus_one)));
    CHECK(parse_str(" ( 11 * 10 ) + 1 ")
                  ->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(11),NEW(NumExpr)(10)),
                                      NEW(NumExpr)(1))));
    CHECK(parse_str(" 1 + 2 * 3 ")
                  ->equals(NEW(AddExpr)(NEW(NumExpr)(1),
                                      NEW(MultExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)))));

//    CHECK (parse_str_error(" ! ") == "expected a digit or open parenthesis at !");
//    CHECK (parse_str_error(" ( 1 ") == "expected a close parenthesis");
//    CHECK ( parse_str_error(" 1 )") == "expected end of file at )" );
//    CHECK (parse_str_error(" ( 1 ") == "expected a close parenthesis");

    CHECK(parse_str("10")->equals(NEW(NumExpr)(10)));
    CHECK(parse_str("(10)")->equals(NEW(NumExpr)(10)));
    CHECK(parse_str("(-10)")->equals(NEW(NumExpr)(-10)));
    CHECK(parse_str("10+1")->equals(NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(1))));
    CHECK(parse_str("(10+1)")->equals(NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(1))));
    CHECK(parse_str("(10)+1")->equals(NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(1))));
    CHECK(parse_str("10+(1)")->equals(NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(1))));
    CHECK(parse_str("1+2*3")->equals(NEW(AddExpr)(NEW(NumExpr)(1),NEW(MultExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)))));
    CHECK(parse_str("1*2+3")->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(1),NEW(NumExpr)(2)),NEW(NumExpr)(3))));
    CHECK(parse_str("bird*2+3")->equals(
           NEW(AddExpr)(NEW(MultExpr)(NEW(VarExpr)("bird"),NEW(NumExpr)(2)),NEW(NumExpr)(3))));
    CHECK(parse_str("4*2*3")->equals(NEW(MultExpr)(NEW(NumExpr)(4),NEW(MultExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)))));
    CHECK(parse_str("4+2+3")->equals(NEW(AddExpr)(NEW(NumExpr)(4),NEW(AddExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)))));
    CHECK(parse_str("4*(2+3)")->equals(NEW(MultExpr)(NEW(NumExpr)(4),NEW(AddExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)))));
    CHECK(parse_str("(2+3)*4")->equals(NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)),NEW(NumExpr)(4))));

    CHECK (parse_str_error("(1") == "), expected a close parenthesis");

    CHECK(parse_str(" 10 ")->equals(NEW(NumExpr)(10)));
    CHECK(parse_str(" (  10 ) ")->equals(NEW(NumExpr)(10)));
    CHECK(parse_str(" 10  + 1")->equals(NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(1))));
    CHECK(parse_str(" ( 10 + 1 ) ")->equals(NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(1))));
    CHECK(parse_str(" 11 * ( 10 + 1 ) ")->equals(
           NEW(MultExpr)(NEW(NumExpr)(11),NEW(AddExpr)(NEW(NumExpr)(10),NEW(NumExpr)(1)))));
    CHECK(parse_str(" ( 11 * 10 ) + 1 ")->equals(
           NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(11),NEW(NumExpr)(10)),NEW(NumExpr)(1))));
    CHECK(parse_str(" 1 + 2 * 3 ")->equals(NEW(AddExpr)(NEW(NumExpr)(1),NEW(MultExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)))));
    CHECK(parse_str("4*(2+3)*4+3")->equals(NEW(AddExpr)(
           NEW(MultExpr)(NEW(NumExpr)(4),NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(2),NEW(NumExpr)(3)),NEW(NumExpr)(4))),
           NEW(NumExpr)(3))));
}

TEST_CASE("it_all_ran_together_so_deal") {
    CHECK(parse_str("3+Jim")->subst("Jim",NEW(NumVal)(2))->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(5)));
    CHECK(parse_str("Pam*Jim")->subst("Pam",NEW(NumVal)(5))->equals(NEW(MultExpr)(NEW(NumExpr)(5),NEW(VarExpr)("Jim"))));
    CHECK(parse_str(" 1 + jim * 3 ")->subst("jim",NEW(NumVal)(2))->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(7)));
    CHECK(parse_str(" x ")->hasVariable());
    CHECK(parse_str("_let x = 5\n _in x + x")->equals(
           NEW(LetExpr)("x",NEW(NumExpr)(5),NEW(AddExpr)(NEW(VarExpr)("x"),NEW(VarExpr)("x")))));
    CHECK(parse_str("_let x = 5\n _in x + x")->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(10)));
    CHECK(parse_str("_let x = 5 _in _let y = z + 2 _in x + y + (2 * 3)")->optimize()->toString() ==
          "_let y = z + 2 _in 5 + y + 6");
    CHECK(parse_str("3+Jim")->subst("Jim",NEW(NumVal)(2))->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(5)));
    CHECK(parse_str("(_let x = 5 _in x) + 3")->optimize()->equals(NEW(NumExpr)(8)));
    CHECK(parse_str("(_let x = 5 _in x) + 3")->optimize()->toString() == "8");
    CHECK(parse_str("_let z =(_let x = 5 _in x) _in (z+1)")->optimize()->toString() == "6");
    CHECK(parse_str("_let x = 1 + x _in 3")->optimize()->toString() == "_let x = 1 + x _in 3");
    CHECK(parse_str("_let y = y _in y")->subst("y",NEW(NumVal)(7))->toString() == "_let y = 7 _in y");
    CHECK(parse_str("_let x = 3 + x in x + x")->subst("x",NEW(NumVal)(5))->toString() == "_let x = 3 + 5 _in x + x");
    CHECK(parse_str("_if _false _then 5 _else 6")->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(6)));
    CHECK(parse_str("_if _true _then 5 _else 6")->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(5)));
    CHECK(parse_str("_if _false _then 5 _else 6")->toString() == ("_if _false _then 5 _else 6"));






}

TEST_CASE("simplify_tests") {
    CHECK(parse_str("4+4")->optimize()->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(8)));
    CHECK(parse_str("4*2")->optimize()->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(8)));
    CHECK(parse_str("4*2+2")->optimize()->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(10)));
    CHECK(parse_str("4+y")->toString() == "4 + y");
    CHECK(parse_str("4*y")->toString() == "4 * y");
    CHECK( parse_str("4 == y")->toString() == "4 == y");



}

TEST_CASE("boolean_tests") {
    CHECK(parse_str("_true")->equals(NEW(BoolExpr)(true)));
    CHECK(parse_str("_false")->equals(NEW(BoolExpr)(false)));
}
TEST_CASE("Double Call Expr") {
    CHECK(parse_str("f(10)(1)")->equals(NEW(CallExpr)(NEW(CallExpr)(NEW(VarExpr)("f"), NEW(NumExpr)(10)), NEW(NumExpr)(1))));
}

TEST_CASE("Double_Equals_tests") {
    CHECK(parse_str("3 == 3")->equals(NEW(EqualsExpr)(NEW(NumExpr)(3),NEW(NumExpr)(3))));
    CHECK(parse_str("-3 == -3")->equals(NEW(EqualsExpr)(NEW(NumExpr)(-3),NEW(NumExpr)(-3))));
    CHECK(parse_str("-9 == -3")->equals(NEW(EqualsExpr)(NEW(NumExpr)(-9),NEW(NumExpr)(-3))));
    CHECK(parse_str("_let f = _fun (x) x + 1 _in f(10)")->interp(NEW(EmptyEnv)())->to_string() == ("11"));
    CHECK(parse_str("_let f = _fun (x) x + 1 _in f(10)")->toString()== ("_let f = (_fun (x) x + 1) _in f(10)"));
    CHECK(parse_str("_let f = _fun (x) _fun (y) x*x + y*y _in f(2)(3)")->interp(NEW(EmptyEnv)())->equals(NEW(NumVal)(13)));


    std::string fibString =
                            " _let fib = _fun (fib)\n"
                            "              _fun (x)\n"
                            "                 _if x == 0\n"
                            "                 _then 1\n"
                            "                 _else _if x == 2 + -1\n"
                            "                 _then 1\n"
                            "                 _else fib(fib)(x + -1)\n"
                            "                       + fib(fib)(x + -2)\n"
                            " _in fib(fib)(10)";

//    std::cout << parse_str(fibString)->interp(NEW(EmptyEnv)())->to_string() << "\n";
    CHECK(parse_str(fibString)->interp(NEW(EmptyEnv)())->to_string() == "89");




}

//TEST_CASE("Fun step interp"){
//
//
//
//    CHECK(Step::interp_by_steps(parse_str("_let count = _fun (count)\n"
//                                          " _fun (n)\n"
//                                          " _if n == 0\n"
//                                          " _then 0\n"
//                                          " _else 1 + count (count) (n + -1)\n"
//                                          " _in count (count)(100000)")
//    )->equals(NEW(NumVal)(100000)));
//}


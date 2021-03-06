#include <iostream>
#include "pointer.hpp"
#include "expr.hpp"
#include "env.hpp"
#include "parse.hpp"
#include "value.hpp"
#include "step.hpp"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"


int main(int argc, char **argv) {

    try {
        bool optimize_mode = false;
        bool step_mode = false;
        PTR(Expr) e;

        if ((argc > 1) && !strcmp(argv[1], "--opt")){
            optimize_mode = true;
            argc--;
            argv++;
        } else if ((argc > 1) && !strcmp(argv[1], "--step")) {
            step_mode = true;
            argc--;
            argv++;
        }

        if (argc > 1) {
            std::ifstream prog_in(argv[1]);
            e = parse(prog_in);
        } else {
            e = parse(std::cin);
        }
        try {
            if(optimize_mode){
                std::cout << e->optimize()->toString() << std::endl;
            } else if (step_mode) {
                std::cout << Step::interp_by_steps(e)->to_string() << std::endl;
            } else {
                std::cout << e->interp(Env::empty)->to_string() << std::endl;
            }
        } catch (std::runtime_error err) {
            std::cerr << err.what() << std::endl;
            return 2;
        }
        return 0;
    } catch (std::runtime_error err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }



}




//    Catch::Session().run(argc, argv);
//
//    PTR(Expr) e = parse(std::cin);
//
//    std::cout << e->interp(NEW(EmptyEnv)()) << std::endl;

Library Guide

This guide is provided as a supplement to the User Guide. 
It is recommended that all users start with the user guide then move onto
this guide as the library file is created when cmake is run. If you have 
run the install instructions you will see that along with the msdscript
executable there is also a library file name MSDscriptlib. 

Embed this library file in your application Using #include `"MSDscriptlib"`.

**Using the library**

msdscript, takes a string and parses it into expressions, you can see the grammar
that msdscript follows in the user guide. It follows all of the input rules 
that are covered in the user guide. The parser takes a string and
parses it into expressions that can then be interpreted or optimized. The way that the command
line tool works is as follows.

`PTR(Expr) e;`

`e = parse(std::cin);`

 Parse takes an input stream that contains an expression,
 and returns the parsed representation of that expression.
 Throws `runtime_error` for parse errors. This function is used
 to parse input. 
 
 After input has been parsed you will need to choose which mode to run;
 optimize, step, or interpret. This is done using the following commands.
 
 For optimize mode:
 
 `std::cout << e->optimize()->toString();`
 
 For interpret mode:
 
 `std::cout << e->interp(Env::empty)->to_string();`
 
 For step mode:
 
 `std::cout << Step::interp_by_steps(e)->to_string();`
 
 See the documentation for more in depth descriptions of the 
 different expression types and more information about how they
 can be used.
 
 
 
 
 
 



 
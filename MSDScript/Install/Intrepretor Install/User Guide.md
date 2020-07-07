    MSDscript Users Guide, (see pdf of complete users guide in zip)
    
    MSDScript is a basic interpreter written in C++. It is used to interpret user input of mathematic operations and returns either an integer value, or an optimized version of the input received. In this package a command line implementation of the language is included. Also a library that can be inserted into existing projects. MSDscript can be used as a way to provide users a basic language for user input. An example of this would be for a calendar.
    	It includes support for addition, multiplication, booleans, functions and recursion. MSDscript follows a strict grammar, seen later, as a rule and input that follows the grammar is valid input and will be evaluated.
    
   
    
    
    Installation
    1. Download CMake from https://cmake.org/download/
    2. Install CMake
    3. Unzip the “MS    DScript.zip” file.
    4. Open terminal in the unzipped directory by right clicking on the build directory and choosing the Services tab, then “New Terminal at folder.
    5. Type “cmake ..” And press enter.
    6. Type “cmake —build .”
    7. Masdscript executable has now been created in the build folder and can be run by double clicking msdscript, or by navigating to the folder in the terminal and typing,       “./msdscript”.
    
   
    Getting started with msdscript
    There are three parts to this system.
    Interpreter, which evaluates an MSDscript expression (defined below) and returns a 	value. The default operation mode.
    Optimizer, which simplifies an MSDscript expression and returns a simpilified MSDscript expression. Enabled with the —opt tag “./msdscript —opt”
    Step, same as interpreter but takes much less space on the stack, use this if you are getting a segfault. Enabled with the —step tag “./msdscript —opt”
    
    Entering an Expression
    After setting what mode you want, an expression can be entered for evaluation. Expressions follow the grammar and there are examples below to show the format that the program.
    
    Expression Definition and General Input Guidelines
    	Some general input guidelines.
    All valid input follows the grammar, the grammar and examples are a good reference for creating good input.
    Some input is legal but not useful and will return a phrase indicating that something that does not make sense has been input. For example, multiplying booleans _true * _true will not evaluate and will return the message “no multiplying booleans”. Keep this in mind if you receive this or similar messages.
    
    See MSDscript.PDF for the entire guide.
    
    
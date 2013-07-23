== Tool: predefined macro detector
This is a simple generator for detecting predefined macros of specific target C++ implementations.
The generated file is a C++ source file, which can be used to compiled, trying "#ifdef"s to filter a set of input macros and writing the output to a file.

= Precondition
The source requires (limited) ISO C++11 support.
G++ 4.8.1 and VC++2013 Preview have been tested.
(Because raw string literal is used, VC++ prior 2013 can't be used.)
An input file is used for the generator. It should contain a set of specific predefined macros, one macro a line.
A sample of input file (only G++ macro set supported) is provided with default name.
The generated file requires ISO C++98/03/11 implementations, which is the target implementation.

= Source configuration
Several macro in the source file can be passed from command line. They are mainly used to specifying names of generated/output files.
The default input file name is "predefined-macro-detector.txt".
The default generated file name is "__gen_predefined-macro-detector.cc".
The default output file name is "predefined-macro-detector.log".

= Use for hosted environment
Compile the source, then run the generator with the input file to get the generated file.
Compile the generated file and run. The output is a file whose content is a subset of input file, consisting of all macros predefined in the target implementation.

= Use for freestanding environment
Since the generator uses ISO C++ file stream, the generator is not guaranteed to be compiled, but can be just compiled as hosted program with an input file for freestanding environment.
The generated file is also not guaranteed to be supported. However, because only "#ifdef"s are used in the detection, the generated file can be preprocessed(like "g++ -E") and then be compiled and run in hosted environment to get intended output.
This method is fit for cross-compilers. 


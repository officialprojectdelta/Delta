# &#916; Cross Compiler (dcc)


A programming language, designed as an alternative to C++, based off of C. This is the compiler for the project. This will use (at least for now) the human readable format of llvm ir. This is to make it easier for debugging. Eventually Delta will migrate to the in memory format of the llvm IR. 

The Plan
See TODO for features that are going to be implemented soon, and IDEAS for ideas for the future of the project. The current plan is to get everything up to structs (see TODO) well tested as a clone of C so this compiler can be tested against clang (for the most part), and then start implenting a more modern syntax as well as more features
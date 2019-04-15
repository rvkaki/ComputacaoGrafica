# ComputacaoGrafica

Authors:
José Boticas
José Costa
Ricardo Vieira

Compile:
compile tinyxml2.cpp with "g++ -c tinyxml2.cpp" in Engine directory
create build directory in both Engine and Generate directories
run "cmake ../." in both Engine/build and Generate/build directories
run "make" in Generate/build directory
run "make" in Engine/build directory

Usage:
"generate [geometric figure] [arguments] ../../[figura.3d]"     // in Generate directory
"generate bezier [patch_file] tesselation ../../[bezier.3d]"    // in Generate directory
"engine coisas.xml"                                             // in Engine directory
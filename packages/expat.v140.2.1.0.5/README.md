# EXPAT.V140

This package contains the Expat C++ SDK, with DLLs, import libraries and header files.
It was built with Visual Studio 2015 (V140).

## Source Code

The source code used to build this package is available at https://github.com/zeroc-ice/expat.

## Build Instructions

git clone git@github.com:zeroc-ice/expat.git -b msvc
cd expat
MSBuild msbuild\expat.proj /t:NugetPack

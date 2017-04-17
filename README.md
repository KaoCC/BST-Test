# HeteroBench
HeteroBench is a collection of numerous OpenCL benchmarks


## Build

### Windows
- Create Visual Studio 2017 Solution

`./Tool/premake/win/premake5.exe vs2017`


### Linux

- Install GCC

`sudo apt-get install g++`

- Create the Makefile

`./Tool/premake/linux64/premake gmake`

- Build it !

`make config=release_x64`


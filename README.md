# HeteroBench

HeteroBench is a collection of numerous OpenCL benchmarks.


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


## How to contribute

- Fork the repository, create a branch based on the current dev branch.

- Add your benchmark files to the "Testbed" folder with the following format: "FrameworkType_ProjectName".

- If you wish to extend the utilities, put the files to the "Utility" folder

- Modify the premake configuration, append the project at the bottom of the list.

- Create a "Pull Request".


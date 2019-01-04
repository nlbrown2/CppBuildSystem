# C++ Build System
A build system in C++ for C++

## Main Focus
The main goal is to be easier and simpler to use than `make`. I plan on this project being simpler, more convenient, and faster than `make`.

## Features
* Automatic header file scanning
   * Will scan all ```#include "Example.h"``` headers for each target. This scanning includes scanning all includes as well. Therefore, if a header nested layers deep is changed, all corresponding object files will be rebuilt as well as the executable being relinked.
* Automatically uses a process pool as large as the number of cores on the current machine.
* Customizable and simple compilation and link commands.
    * The Buildfile syntax is intended to be easy and intuitive to use. It is still a work in progress, but currently should cover basic use cases.
* Builds corresponding .o files for every relevant .cpp file.

## Installation
Easy installation can be done using the provided ```install.sh``` script. You may have to grant executable permissions using ```$ chmod +x install.sh``` before installing with ```$ ./install.sh```. The default name for the executable is cppb. If you wish to build the executable under a different name, then provide it as the first argument to the install script. The binary will be built and then copied to /usr/local/bin. To uninstall, remove the executable from /usr/local/bin.

## Usage
I like to build the executable as `cppb`, which stands for cpp build. Currently, this does not accept any command line arguments. Running this will cause the Buildfile to be read and used in building the executable.
### Buildfile Syntax
There are three main components to the Buildfile currently. The lines do not have to come in any particular order. The commands generally follow the syntax of OPTION: VALUE. The one exception is naming the executable and relevant files. See the provided Buildfile for this project as an example.
#### Executable Name
Instead of taking up two lines for the executable name and listing the .cpp files that are to be compiled and linked for it, this information fits all on one line. The syntax is 

    NAME: example1.cpp example2.cpp

The names of the cpp files must be full names relative to the current directory. I have not tested multi-directory structures at this time.
Alternatively, one could put a list of regex patterns to match against the files in the directory. For example, this can be used to include all cpps (*\.cpp). The "flavor" of regex is ECMAScript's regex.
#### COMPILECMD
This is used to determine what commands are issued to build individual object files. The name for the source file should be replaced with "SOURCE" and the corresponding object file replaced with "OBJECT". This command template will be used to compile every cpp file into the object file. The name of the object file is the same as the cpp file, but with the ".cpp" extension converted to ".o".
Example:

    COMPILECMD: g++ SOURCE -o OBJECT
#### LINKCMD
This is used as a template to link the executable. The placeholder OBJECTS is used to represent all of the ".o"s generated from the list of ".cpp" files listed for the executable. The placeholder EXE is used to represent the executable name, as determined from the other line in the Buildfile.
Example:

     LINKCMD: g++ OBJECTS -o EXE

# Future Changes:
- [x] Add in the EXE substitution for the LINKCMD
- [x] Add ability to say all .cpp files in current directory
- [x] Add ability to say all .cpp files except for certian ones
- [ ] Add ability to have multiple EXEs built.
- [ ] Add ability to have profiles. Example: a testing profile builds a different exe than a production one, or a development profile builds the same exe with a different COMPILECMD
- [ ] Add ability to run shell commands before/after an executable has been built.
- [ ] Add cross-platform support
- [ ] Add multi-level directory structure support if currently not supported

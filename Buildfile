COMPILECMD: g++ -std=c++1z -c SOURCE -o OBJECT
main_exe: main.cpp File.cpp File_Factory.cpp Utility.cpp
LINKCMD: g++ -std=c++1z -pedantic OBJECTS -o main_exe

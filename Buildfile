COMPILECMD: g++ -std=c++1z -c SOURCE -o OBJECT
cppb: main.cpp File.cpp FileFactory.cpp CppFile.cpp MemoryMappedFile.cpp ObjectFile.cpp Buildfile.cpp Utility.cpp
LINKCMD: g++ -std=c++1z -pedantic OBJECTS -o EXE

default:
    COMPILECMD: g++ -std=c++1z -g3 -c SOURCE -o OBJECT
    cppb: .*\.cpp
    LINKCMD: g++ -std=c++1z -pedantic -g3 OBJECTS -o EXE
    POSTCMD: ./profile.sh
prod:
    COMPILECMD: g++ -std=c++1z -O3 -c SOURCE -o OBJECT
    cppb: .*\.cpp
    LINKCMD: g++ -std=c++1z -pedantic -O3 OBJECTS -o EXE

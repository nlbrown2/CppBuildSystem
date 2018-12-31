#include "File_Factory.h"
#include "File.h"
using namespace std;

const CppFile& FileFactory::get_cpp_file(const string& filename, set<string>& seen) {
    auto it = name_to_cppfile.find(filename);
    if(it == name_to_cppfile.end()) {
        CppFile file{filename, seen};
        it = name_to_cppfile.insert(make_pair(filename, file)).first;
    }
    return it->second;
}

const CppFile& FileFactory::get_cpp_file(const std::string& filename) {
    set<string> seen;
    return get_cpp_file(filename, seen);
}

const ObjectFile& FileFactory::get_obj_file(const std::string& filename) {
    auto it = name_to_objfile.find(filename);
    if(it == name_to_objfile.end()) {
        it = name_to_objfile.insert(make_pair(filename, ObjectFile{filename})).first;
    }
    return it->second;
}

#ifndef FILE_FACTORY_H
#define FILE_FACTORY_H
#include <string>
#include <unordered_map>
#include "Utility.h"
#include "ObjectFile.h"
#include "CppFile.h"
class FileFactory {
public:
    static FileFactory& get() {
        static FileFactory instance;
        return instance;
    }
    const CppFile& get_cpp_file(const std::string& filename, std::set<std::string>& seen);
    const CppFile& get_cpp_file(const std::string& filename);
    const ObjectFile& get_obj_file(const std::string& filename);
private:
    std::unordered_map<std::string, CppFile> name_to_cppfile;
    std::unordered_map<std::string, ObjectFile> name_to_objfile;
    FileFactory() = default;
    ~FileFactory() = default;
};
#endif

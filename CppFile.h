#ifndef CPPFILE_H
#define CPPFILE_H
#include "MemoryMappedFile.h"
#include <set>
#include <string>
class CppFile : public MemoryMappedFile {
public:
    CppFile(const std::string& name, std::set<std::string>& seen);
    const std::set<std::string>& get_headers() const {
        return headers;
    }
private:
    std::set<std::string> headers;
};
#endif

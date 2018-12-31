#ifndef MEMORYMAPPEDFILE_H
#define MEMORYMAPPEDFILE_H
#include "File.h"
class MemoryMappedFile : public File {
public:
    using iterator = const char*;
    iterator begin() const {
        return start;
    }
    iterator end() const {
        return start + get_filesize();
    }
protected:
    MemoryMappedFile(const std::string& name);
    ~MemoryMappedFile();
private:
    char* start;
    char* end_m;
};
#endif

#ifndef FILE_H
#define FILE_H
#include <string>
#include "Utility.h"

class File {
public:
    int get_file_descriptor() const {
        return fd;
    }
    size_t get_filesize() const {
        return get_statistics().st_size;
    }
    const std::string& get_filename() const {
        return filename;
    }
    const Stat& get_statistics() const {
        return stats;
    }
protected:
    File(const std::string& name);
    ~File();
private:
    std::string filename;
    int fd;
    Stat stats;
};
#endif

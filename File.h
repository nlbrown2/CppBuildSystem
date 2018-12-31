#ifndef FILE_H
#define FILE_H
#include <string>
#include <vector>
#include <set>
#include <map>
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

class ObjectFile: public File {
public:
    ObjectFile(const std::string& name);
};

class CppFile : public MemoryMappedFile {
public:
    CppFile(const std::string& name, std::set<std::string>& seen);
    const std::set<std::string>& get_headers() const {
        return headers;
    }
private:
    std::set<std::string> headers;
};

class Buildfile: public MemoryMappedFile {
public:
    Buildfile();
    const std::vector<std::string>& get_cpp_filenames() const {
        return cpps;
    }
    const std::vector<std::string>& get_object_filenames() const {
        return objects;
    }
    const std::string& get_execuatable_name() const {
        return executable_name;
    }
    //these two are lazily loaded to speed up when no compilation is needed
    const std::vector<std::string>& get_compile_command(const std::string& source, const std::string& object);
    const std::string& get_link_command(const std::vector<std::string>& object_names);
private:
    void add_compile_arg(std::string&& arg, const std::string& source, const std::string& object);
    void add_link_arg(std::string&& arg, const std::vector<std::string>& object_names);
    static std::string cpp_to_object(const std::string& cpp_filename);
    const char* first_alnum_char_past(const std::string& phrase) const;
    std::vector<std::string> compile_cmd;
    size_t source_index;
    size_t object_index;
    std::string linkcmd;
    std::string executable_name;
    std::vector<std::string> cpps;
    std::vector<std::string> objects;
};
#endif

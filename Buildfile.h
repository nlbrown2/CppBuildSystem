#ifndef BUILDFILE_H
#define BUILDFILE_H
#include "MemoryMappedFile.h"
#include <vector>
#include <string>
#include <regex>
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
    const std::string& get_postlink_command() const {
        return postlink_cmd;
    }
private:
    void add_compile_arg(std::string&& arg, const std::string& source, const std::string& object);
    void add_link_arg(std::string&& arg, const std::vector<std::string>& object_names);
    static std::string cpp_to_object(const std::string& cpp_filename);
    iterator first_alnum_char_past(const std::string& phrase) const;
    iterator first_alnum_char_past_colon(iterator it, iterator end) const;
    bool filter(const std::string& filename);
    std::vector<std::string> compile_cmd;
    std::vector<std::string> cpps;
    std::vector<std::regex> cpp_regexs;
    std::vector<std::string> objects;
    std::string linkcmd;
    std::string executable_name;
    std::string postlink_cmd;
    size_t source_index;
    size_t object_index;
};
#endif

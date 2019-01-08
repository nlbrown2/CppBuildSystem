#ifndef BUILDFILE_H
#define BUILDFILE_H
#include "MemoryMappedFile.h"
#include <vector>
#include <string>
#include <regex>
class Buildfile: public MemoryMappedFile {
public:
    Buildfile(const std::string_view& profile);
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
    iterator skip_to_profile() const;
    const char* first_alnum_char_past(const std::string& phrase) const;
    iterator skip_past_first_alnum(iterator it, iterator end) const;
    iterator skip_leading_whitespace(iterator it, iterator end) const;
    std::vector<std::string> compile_cmd;
    size_t source_index;
    size_t object_index;
    std::string linkcmd;
    std::string executable_name;
    std::vector<std::string> cpps;
    std::vector<std::regex> cpp_regexs;
    bool filter(const std::string& filename);
    std::vector<std::string> objects;
    std::string_view profile_;
};
#endif

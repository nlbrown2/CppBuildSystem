#include "Buildfile.h"
#include "Directory.h"
#include <regex>
#include <iostream>
using namespace std;
using namespace std::placeholders;

auto Buildfile::skip_past_first_alnum(iterator it, iterator end) const -> iterator  {
    auto res = skip_past(it, end);
    return skip_leading_whitespace(res, end);
}

Buildfile::Buildfile(const string_view& profile) : MemoryMappedFile("Buildfile"), profile_(profile) {
    auto it = begin();
    if(!profile.empty()) {
        while(strncmp(it, profile.data(), profile.length()))
            it = skip_past(it, end()); //scan forward until we determine the start of the desired profile
        it = skip_past(it, end()); //skip line of profile
    }
    it = skip_leading_whitespace(it, end());
    while(!strncmp(it, "COMPILECMD", 10) || !strncmp(it, "LINKCMD", 7))
        it = skip_past_first_alnum(it, end()); //skip lines that begin with COMPILECMD or LINKCMD
    if(it == end())
        throw runtime_error("No executable name found!");
    it = skip_leading_whitespace(it, end());
    while(it != end() && *it != ':') //read from beginning of line until ':'
        executable_name += *it++;
    while(it != end() && (isspace(*it) || *it == ':') && *it != '\n')
        ++it;
    string regex_str;
    while(it != end() && *it != '\n') {
        if(isspace(*it)) {
            cpp_regexs.emplace_back(regex_str);
            regex_str.clear();
        }
        else
            regex_str += *it;
        ++it;
    }
    if(!regex_str.empty())
        cpp_regexs.emplace_back(regex_str);

    Directory current_dir;
    cpps = current_dir.matching_filenames(bind(&Buildfile::filter, this, _1));
    transform(cpps.begin(), cpps.end(), back_inserter(objects), &Buildfile::cpp_to_object);
}

bool Buildfile::filter(const string& filename) {
    for(auto& r : cpp_regexs)
        if(regex_match(filename, r))
            return true;
    return false;
}

const vector<string>& Buildfile::get_compile_command(const string& source, const string& object) {
    if(!compile_cmd.empty()) {
        compile_cmd[source_index] = source;
        compile_cmd[object_index] = object;
        return compile_cmd;
    }
    //first time loading this, so need to fill compile_cmd
    auto it = first_alnum_char_past("COMPILECMD");
    string arg;
    while(it != end() && *it != '\n') {
        //read each word delimited by whitespace. Could conver to stringstream
        //but that would be expensive. Eaiser to just do this
        if(isspace(*it)) {
            add_compile_arg(move(arg), source, object);
            arg.clear();
        } else {
            arg += *it;
        }
        ++it;
    }
    if(!arg.empty()) {
        add_compile_arg(move(arg), source, object);
    }
    return compile_cmd;
}

const string& Buildfile::get_link_command(const vector<string>& object_names) {
    if(!linkcmd.empty())
        return linkcmd;
    auto it = first_alnum_char_past("LINKCMD");
    string word;
    while(it != end() && *it != '\n') {
        if(isspace(*it)) {
            add_link_arg(move(word), object_names);
            word.clear();
        } else {
            word += *it;
        }
        ++it;
    }
    if(!word.empty())
        add_link_arg(move(word), object_names);
    return linkcmd;
}

void Buildfile::add_link_arg(string&& arg, const vector<string>& object_names) {
    if(arg == "OBJECTS") {
        for(auto& obj : object_names) {
            linkcmd += obj;
            linkcmd += ' ';
        }
    } else if(arg == "EXE") {
        linkcmd += executable_name;
        linkcmd += ' ';
    } else {
        linkcmd += arg;
        linkcmd += ' ';
    }
}

void Buildfile::add_compile_arg(string&& arg, const string& source, const string& object) {
    if(arg == "SOURCE") {
        source_index = compile_cmd.size();
        compile_cmd.emplace_back(source);
    } else if(arg == "OBJECT") {
        object_index = compile_cmd.size();
        compile_cmd.emplace_back(object);
    } else
        compile_cmd.emplace_back(move(arg));
}

string Buildfile::cpp_to_object(const string& cpp_filename) {
    size_t cpp_index = cpp_filename.find(".cpp");
    string object_filename{cpp_filename.begin(), cpp_filename.begin() + cpp_index};
    object_filename += ".o";
    return object_filename;
}

const char* Buildfile::first_alnum_char_past(const string& phrase) const {
    auto it = begin();
    it = skip_to_profile();
    it = skip_leading_whitespace(it, end());
    while(strncmp(it, phrase.c_str(), phrase.size())) {
        it = skip_past(it, end()); //move it to begin at line of phrase
        it = skip_leading_whitespace(it, end());
    }
    it = skip_past(it, end(), ':');
    while(it != end() && isspace(*it) && *it != '\n')
        ++it; //skip all whitespace that is not the end of the line
    return it;
}

auto Buildfile::skip_to_profile() const -> iterator {
    auto it = begin();
    if(!profile_.empty()) {
        while(strncmp(it, profile_.data(), profile_.length()))
            it = skip_past(it, end()); //scan forward until we determine the start of the desired profile
    }
    return it;
}

auto Buildfile::skip_leading_whitespace(iterator it, iterator end) const -> iterator {
    while(it != end && isspace(*it))
        ++it;
    return it;
}

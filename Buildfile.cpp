#include "Buildfile.h"
#include "Directory.h"
#include <regex>
#include <iostream>
using namespace std;
using namespace std::placeholders;

const char* const compilecmd_c = "COMPILECMD:";
const int compilecmd_len_c = 11;
const char* const linkcmd_c = "LINKCMD:";
const int linkcmd_len_c = 8;
const char* const postlinkcmd_c = "POSTCMD:";

Buildfile::Buildfile() : MemoryMappedFile("Buildfile") {
    auto it = begin();
    while(!strncmp(it, compilecmd_c, compilecmd_len_c) || !strncmp(it, linkcmd_c, linkcmd_len_c))
        it = skip_past(it, end()); //skip lines that begin with compile or link command keywords
    if(it == end())
        throw runtime_error("No executable name found!");
    while(it != end() && *it != ':') //read from beginning of line until ':'
        executable_name += *it++;
    it = first_alnum_char_past_colon(it, end());
    string regex_str;
    while(it != end() && *it != '\n') {
        if(isspace(*it)) {
            cpp_regexs.emplace_back(move(regex_str));
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
    it = first_alnum_char_past(postlinkcmd_c);
    if(it == end())
        return;
    it = first_alnum_char_past_colon(it, end());
    while(*it != '\n')
        postlink_cmd += *it++;
}

auto Buildfile::first_alnum_char_past_colon(iterator it, iterator end) const -> iterator {
    while(it != end && (isspace(*it) || *it == ':'))
        ++it; //skip until first alnum char that isn't ':'
    return it;
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
    auto it = first_alnum_char_past(compilecmd_c);
    string arg;
    while(it != end() && *it != '\n') {
        //read each word delimited by whitespace. Could conver to stringstream
        //but that would be expensive. Eaiser to just do this
        if(isspace(*it)) {
            add_compile_arg(move(arg), source, object);
            arg.clear();
        } else
            arg += *it;
        ++it;
    }
    if(!arg.empty())
        add_compile_arg(move(arg), source, object);
    return compile_cmd;
}

const string& Buildfile::get_link_command(const vector<string>& object_names) {
    if(!linkcmd.empty())
        return linkcmd;
    auto it = first_alnum_char_past(linkcmd_c);
    string word;
    while(it != end() && *it != '\n') {
        if(isspace(*it)) {
            add_link_arg(move(word), object_names);
            word.clear();
        } else
            word += *it;
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
    while(strncmp(it, phrase.c_str(), phrase.size()))
        it = skip_past(it, end()); //move it to begin at line of phrase
    it = skip_past(it, end(), ':');
    while(it != end() && isspace(*it) && *it != '\n')
        ++it; //skip all whitespace that is not the end of the line
    return it;
}

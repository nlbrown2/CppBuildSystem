#include "File.h"
#include "File_Factory.h"
#include <algorithm>
#include <fcntl.h> //for open
#include <unistd.h>
#include <sys/stat.h> //for fstat
#include <errno.h>
#include <sys/mman.h> //for mmap
#include <regex>
using namespace std;

const regex includes_c{"\\s*#include\\s*\"(.*)\""};
const regex header_c{".*\\.h(pp)?"};

File::File(const string& name) : filename(name) {
    fd = open(filename.c_str(), O_RDONLY);
    if(fd < 0) {
        throw runtime_error("Error opening: " + filename);
    }
    if(fstat(fd, &stats)) {
        string error_msg = "Error getting stats for: " + filename;
        switch(errno) {
        case EBADF:
            error_msg += " bad file descriptor";
            break;
        case EFAULT:
            error_msg += " sb bad address";
            break;
        case EIO:
            error_msg += " IO error";
            break;
        case EOVERFLOW:
            error_msg += " file size in bytes cannot be represented correctly in the stat_info structure";
        default:
            error_msg += " something else";
        }
    }
}

File::~File() {
    close(fd);
}

ObjectFile::ObjectFile(const string& name) : File(name) {}

MemoryMappedFile::MemoryMappedFile(const string& name) : File(name) {
    off_t file_size = get_filesize();
    start = static_cast<char*>(mmap(nullptr, file_size, PROT_READ, MAP_FILE | MAP_SHARED, get_file_descriptor(), 0));
    end_m = start + file_size;
}

MemoryMappedFile::~MemoryMappedFile() {
    munmap(static_cast<void*>(start), get_filesize());
}

CppFile::CppFile(const std::string& name, std::set<std::string>& seen) : MemoryMappedFile(name) {
    if(regex_match(get_filename(), header_c))
        seen.insert(get_filename()); //we have "seen" ourselves in the case of circular dependencies
    set<string> header_names;
    string_view file{begin(), get_filesize()};
    for(regex_iterator it{file.begin(), file.end(), includes_c}; it != regex_iterator<string_view::iterator>{}; ++it) {
        if(it->size() > 1)
            //regex_iterator matches both the #include and the filename
            //the filename is at the first position.
            header_names.insert(it->str(1));
    }
    //keep track of what we have already seen
    //so we don't get caught in a CppFile constructor loop
    while(!header_names.empty()) {
        const string& current = *header_names.begin();
        if(seen.find(current) != seen.end()) {
            header_names.erase(current);
            continue;
        }
        //this is a new headerfile, so open it up and scan it for dependencies
        const CppFile& header = FileFactory::get().get_cpp_file(current, seen);
        const set<string>& nested = header.get_headers();
        copy(nested.begin(), nested.end(), inserter(header_names, header_names.begin()));
        seen.insert(move(current));
    }
    headers = seen;
}

Buildfile::Buildfile() : MemoryMappedFile("Buildfile") {
    auto it = begin();
    while(!strncmp(it, "COMPILECMD", 10) || !strncmp(it, "LINKCMD", 7))
        it = skip_past(it, end()); //skip lines that begin with COMPILECMD or LINKCMD
    if(it == end())
        throw runtime_error("No executable name found!");
    while(it != end() && *it != ':') //read from beginning of line until ':'
        executable_name += *it++;
    while(it != end() && (isspace(*it) || *it == ':') && *it != '\n')
        ++it;
    while(it != end() && *it != '\n') {
        string filename;
        while(it != end() && isspace(*it) && *it != '\n')
            ++it; //skip leading whitespace
        while(it != end() && !isspace(*it))
            filename += *it++;
        cpps.push_back(move(filename));
    }
    transform(cpps.begin(), cpps.end(), back_inserter(objects), &Buildfile::cpp_to_object);
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
            if(word == "OBJECTS") {
                for(auto& obj : object_names) {
                    linkcmd += obj;
                    linkcmd += ' ';
                }
            } else {
                linkcmd += word;
                linkcmd += ' ';
            }
            word.clear();
        } else {
            word += *it;
        }
        ++it;
    }
    if(!word.empty())
        linkcmd += word + ' ';
    return linkcmd;
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

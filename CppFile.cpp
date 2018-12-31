#include "CppFile.h"
#include "FileFactory.h"
#include <regex>
using namespace std;
const regex includes_c{"\\s*#include\\s*\"(.*)\""};
const regex header_c{".*\\.h(pp)?"};


CppFile::CppFile(const string& name, set<string>& seen) : MemoryMappedFile(name) {
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


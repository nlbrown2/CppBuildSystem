#include "Directory.h"
using namespace std;

Directory::Directory(const string& path) : _directory(opendir(path.c_str())) { }

vector<string> Directory::matching_filenames(function<bool (const string&)> predicate) {
    vector<string> result;
    while(struct dirent* ent = readdir(_directory)) {
        if(predicate(ent->d_name))
            result.emplace_back(ent->d_name);
    }
    return result;
    //Important: the directory is not rewound because it does not need to be for our use cases.
}

Directory::~Directory() {
    closedir(_directory);
}

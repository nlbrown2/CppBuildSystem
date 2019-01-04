#ifndef DIRECTORY_H
#define DIRECTORY_H
#include <vector>
#include <string>
#include <functional>
#include <sys/types.h>
#include <dirent.h>
class Directory {
public:
    Directory(const std::string& path = ".");
    std::vector<std::string> matching_filenames(std::function<bool (const std::string&)> predicate);
private:
    DIR* _directory;
};
#endif

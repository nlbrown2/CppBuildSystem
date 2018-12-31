#include "File.h"
#include <fcntl.h> //for open
#include <unistd.h>
#include <sys/stat.h> //for fstat
#include <errno.h>
using namespace std;

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

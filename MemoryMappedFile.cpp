#include "MemoryMappedFile.h"
#include <string>
#include <sys/mman.h> //for mmap
using namespace std;
MemoryMappedFile::MemoryMappedFile(const string& name) : File(name) {
    off_t file_size = get_filesize();
    start = static_cast<char*>(mmap(nullptr, file_size, PROT_READ, MAP_FILE | MAP_SHARED, get_file_descriptor(), 0));
    end_m = start + file_size;
}

MemoryMappedFile::~MemoryMappedFile() {
    munmap(static_cast<void*>(start), get_filesize());
}


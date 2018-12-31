#ifndef OBJECTFILE_H
#define OBJECTFILE_H
#include "File.h"
class ObjectFile: public File {
public:
    ObjectFile(const std::string& name);
};

#endif

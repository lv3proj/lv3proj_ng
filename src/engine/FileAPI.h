#ifndef FILE_API_H
#define FILE_API_H

// TODO: VFS output functions.

#include "ttvfs/VFS.h"
#include <sstream>

extern ttvfs::VFSHelper vfs;

typedef ttvfs::VFSFile VFILE;


VFILE *vfopen(const char *fn, const char *mode);
size_t vfread(void *ptr, size_t size, size_t count, VFILE *vf);
int vfclose(VFILE *vf);
size_t vfwrite(const void *ptr, size_t size, size_t count, VFILE *vf);
int vfseek(VFILE *vf, long int offset, int origin);
char *vfgets(char *str, int num, VFILE *vf);
void vfclear(VFILE *vf);
long int vftell(VFILE *vf);

// This class is a minimal adapter to support STL-like read-only file streams for VFS files, using std::istringstream.
class InStream : public std::istringstream
{
public:
    InStream(const char *fn);
    InStream(const std::string& fn);
    bool open(const char *fn);
    inline bool is_open() { return good(); }
    inline void close() {}
private:
    void _init(const char *fn);
};


#endif // FILE_API_H

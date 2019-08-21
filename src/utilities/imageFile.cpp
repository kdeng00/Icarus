#include "utilities/imageFile.h"

imageFile::imageFile(const char *file) : TagLib::File(file)
{
}

TagLib::ByteVector imageFile::data()
{
    return readBlock(length());
}

#include "utilities/imageFile.h"

Utility::imageFile::imageFile(const char *file) : TagLib::File(file)
{
}

TagLib::ByteVector Utility::imageFile::data()
{
    return readBlock(length());
}

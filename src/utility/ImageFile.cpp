#include "utility/ImageFile.h"

utility::ImageFile::ImageFile(const char *file) : TagLib::File(file)
{
}

TagLib::ByteVector utility::ImageFile::data()
{
    return readBlock(length());
}

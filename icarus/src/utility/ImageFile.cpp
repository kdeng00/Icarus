#include "utility/ImageFile.h"

namespace utility {
    ImageFile::ImageFile(const char *file) : TagLib::File(file) { }

    TagLib::ByteVector ImageFile::data() {
        return readBlock(length());
    }
}

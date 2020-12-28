#ifndef IMAGEFILE_H_
#define IMAGEFILE_H_

#include <iostream>

#include <taglib/attachedpictureframe.h>
#include <taglib/tag.h>
#include <taglib/tfile.h>
#include <taglib/tfilestream.h>
#include <taglib/fileref.h>
#include <taglib/tbytevector.h>
#include <taglib/tbytevectorstream.h>
#include <taglib/tpropertymap.h>
#include <taglib/id3v2tag.h>


namespace utility {
    class ImageFile : public TagLib::File {
    public:
        ImageFile(const char *file);

        TagLib::ByteVector data();

    private:
        virtual TagLib::Tag *tag() const { return 0; }
        virtual TagLib::AudioProperties *audioProperties() const { return 0; }
        virtual bool save() { return false; }
    };
}

#endif

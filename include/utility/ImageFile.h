#ifndef IMAGEFILE_H_
#define IMAGEFILE_H_

#include <iostream>

#include <attachedpictureframe.h>
#include <tag.h>
#include <tfile.h>
#include <tfilestream.h>
#include <fileref.h>
#include <tbytevector.h>
#include <tbytevectorstream.h>
#include <tpropertymap.h>
#include <id3v2tag.h>

namespace utility
{
    class ImageFile : public TagLib::File
    {
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

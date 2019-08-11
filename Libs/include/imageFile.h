#include <iostream>

#include <taglib/attachedpictureframe.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>
#include <taglib/tfile.h>
#include <taglib/tfilestream.h>
#include <taglib/fileref.h>
#include <taglib/tbytevector.h>
#include <taglib/tbytevectorstream.h>
#include <taglib/tpropertymap.h>
#include <taglib/id3v2tag.h>

class imageFile : public TagLib::File
{
public:
    imageFile(const char *file);
                                  /**
    imageFile(const char *file) : TagLib::File(file)
    {

    }
    */

    TagLib::ByteVector data();
    /**
    TagLib::ByteVector data()
    {
        return readBlock(length());
    }
    */


private:
    virtual TagLib::Tag *tag() const { return 0; }
    virtual TagLib::AudioProperties *audioProperties() const { return 0; }
    virtual bool save() { return false; }
};

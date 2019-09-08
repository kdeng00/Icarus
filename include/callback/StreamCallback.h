#ifndef STREAMCALLBACK_H_
#define STREAMCALLBACK_H_

#include <string>

#include "oatpp/core/data/stream/FileStream.hpp"
#include "oatpp/web/protocol/http/outgoing/ChunkedBody.hpp"

namespace callback
{
    class StreamCallback : public oatpp::data::stream::ReadCallback
    {
    public:
        StreamCallback();
        StreamCallback(const std::string&);

        oatpp::data::v_io_size read(void*, oatpp::data::v_io_size);
    private:
        std::string m_songPath;

        long m_bytesRead;
        long m_counter;
        long m_fileSize;
    };
}

#endif

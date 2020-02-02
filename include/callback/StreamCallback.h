#ifndef STREAMCALLBACK_H_
#define STREAMCALLBACK_H_

#include <string>

#include "oatpp/core/data/stream/FileStream.hpp"
#include "oatpp/core/data/stream/Stream.hpp"
#include "oatpp/core/async/Coroutine.hpp"
//#include "oatpp/web/protocol/http/outgoing/ChunkedBody.hpp"

namespace callback {
    class StreamCallback : public oatpp::data::stream::ReadCallback {
    public:
        StreamCallback();
        StreamCallback(const std::string&);

        oatpp::v_io_size read(void*, v_buff_size, oatpp::async::Action&);
    private:
        std::string m_songPath;

        long m_bytesRead;
        long m_counter;
        long m_fileSize;
    };
}

#endif

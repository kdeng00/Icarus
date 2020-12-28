#include "callback/StreamCallback.h"

#include <iostream>
#include <fstream>
#include <memory>

namespace callback {
    StreamCallback::StreamCallback() : m_counter(0) { }

    StreamCallback::StreamCallback(const std::string& songPath) : 
            m_songPath(songPath),
            m_counter(0),
            m_bytesRead(0) { 
        // getting file size
        std::ifstream song(m_songPath.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
        m_fileSize = song.tellg();
        std::cout << "file size is " << m_fileSize << " bytes\n";
    }



    oatpp::v_io_size StreamCallback::read(void *buff, v_buff_size count,
            oatpp::async::Action& action) {
        if (m_counter >= m_fileSize) {
            std::cout << "done streaming song\n";
            return 0;
        }

        std::fstream song(m_songPath.c_str(), std::ios::binary | std::ios::in);
        song.seekg(m_counter);

        std::unique_ptr<char[]> data(new char[count]);

        song.read(data.get(), count);
        std::memcpy(buff, data.get(), count);

        m_counter += count;

        song.close();

        return count;
    }
}

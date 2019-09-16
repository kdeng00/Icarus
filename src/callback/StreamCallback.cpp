#include "callback/StreamCallback.h"

#include <iostream>
#include <fstream>
#include <memory>

callback::StreamCallback::StreamCallback() : 
    m_counter(0) { }

callback::StreamCallback::StreamCallback(const std::string& songPath) : 
    m_songPath(songPath),
    m_counter(0),
    m_bytesRead(0) 
{ 
    // getting file size
    std::ifstream song(m_songPath.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
    m_fileSize = song.tellg();
    std::cout << "file size is " << m_fileSize << " bytes" << std::endl;
}


oatpp::data::v_io_size callback::StreamCallback::read(void *buff, oatpp::data::v_io_size count) 
{
    if (m_counter >= m_fileSize) {
        std::cout << "done streaming song" << std::endl;
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
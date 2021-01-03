#ifndef GENERAL_UTIL_H_
#define GENERAL_UTIL_H_

#include <chrono>
#include <ctime>

namespace utility
{
    class General
    {
    public:
	    template<typename time_point_type, typename tm_type>
	    static tm_type convert_time_point_to_tm(const time_point_type &time)
	    {
	        auto tmp_time_t = std::chrono::system_clock::to_time_t(time);
	
	        return *std::localtime(&tmp_time_t);
	    }
	
	    template<typename time_point_type, typename tm_type>
	    static time_point_type convert_tm_to_time_point(tm_type &time)
	    {
	        auto time_t_val = std::mktime(&time);
	
	        return std::chrono::system_clock::from_time_t(time_t_val);
	    }
    };
}

#endif

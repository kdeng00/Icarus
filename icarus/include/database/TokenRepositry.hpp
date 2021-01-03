#ifndef TOKENREPOSITORY_H_
#define TOKENREPOSITORY_H_

#include <vector>

#include <icarus_lib/icarus.h>


namespace database
{
    template<typename token>
    class TokenRepository
    {
    public:
        TokenRepository() = delete;
        TokenRepository(const icarus_lib::binary_path &config)
        {
        }


        template<typename container = std::vector<token>>
        container retrieve_all_tokens()
        {
            container contr;

            return contr;
        }
    protected:
    private:
    };
}


#endif

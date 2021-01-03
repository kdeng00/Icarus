#ifndef REPOSITORIES_H_
#define REPOSITORIES_H_

#include <string>


#include <icarus_lib/icarus.h>
#include <soci/soci.h>

#include "database/TokenRepository.hpp"


namespace database
{

    using token_rep = TokenRepository<icarus_lib::token, soci::session>;
}


#endif

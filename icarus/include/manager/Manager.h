#ifndef MANAGER_H_
#define MANAGER_H_

#include <icarus_lib/icarus.h>

#include "manager/TokenManager.hpp"

namespace manager
{
    using token_manager = TokenManager<icarus_lib::token>;
}

#endif

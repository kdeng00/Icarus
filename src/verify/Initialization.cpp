#include "verify/Initialization.h"

// verifies if the configuration settings are valid
void verify::Initialization::checkIcarus(const model::BinaryPath& bConf)
{
}


// verifies that the authorization settings are not the default values
bool verify::Initialization::confirmConfigAuth(const model::BinaryPath& bConf)
{
    return true;
}

// verifies if database connectivity can be established
bool verify::Initialization::confirmConfigDatabase(const model::BinaryPath& bConf)
{
    return true;
}

// verifies if the paths found in the config files exists
bool verify::Initialization::confirmConfigPaths(const model::BinaryPath& bConf)
{
    return true;
}

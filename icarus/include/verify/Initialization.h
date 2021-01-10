#ifndef INITIALIZATION_H_
#define INITIALIZATION_H_

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

#include <icarus_lib/icarus.h>


namespace fs = std::filesystem;

namespace verify
{
    class Initialization
    {
    public:
        static bool skipVerification(const std::string &argument) noexcept;

        static void checkIcarus(const icarus_lib::binary_path &bConf);
    private:
        static bool confirmConfigDatabase(const icarus_lib::binary_path &bConf);
        static bool confirmConfigPaths(const icarus_lib::binary_path &bConf);

        static void failedConfirmation();

        template<typename Obj, template <typename> class Container>
        static bool confirmMultiplePaths(const Container<Obj> &con)
        {
            auto result = true;

            for (auto &path : con)
            {
                if (!fs::exists(path))
                {
                    std::cout << path << " is not properly configured\n";
                    return false;
                }
            }


            return result;
        }
    };
}

#endif

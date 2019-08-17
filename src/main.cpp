#include <iostream>
#include <filesystem>
#include <memory>
#include <string>

#include "oatpp/network/server/Server.hpp"
#include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "appComponent.hpp"
#include "controller/loginController.hpp"
//#include "loginHandler.hpp"

namespace fs = std::filesystem;

void run(const std::string& working_path)
{
    appComponent component;

    //auto router = oatpp::web::server::HttpRouter::createShared();
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    //router->route("GET", "/test", std::make_shared<loginHandler>());
    auto logController = std::make_shared<loginController>(working_path);
    logController->addEndpointsToRouter(router);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::server::ConnectionHandler>, connectionHandler);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    oatpp::network::server::Server server(connectionProvider, connectionHandler);

    OATPP_LOGI("icarus", "Server running on port %s", connectionProvider->getProperty("port").getData());

    server.run();
}


int main(int argc, char **argv)
{
    oatpp::base::Environment::init();
    std::string working_path = argv[0];
    //std::cout << fs::canonical(fs::path(working_path.c_str())).parent_path().string() << std::endl;

    run(working_path);

    oatpp::base::Environment::destroy();

    return 0;
}

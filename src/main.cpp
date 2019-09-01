#include <exception>
#include <iostream>
#include <filesystem>
#include <memory>
#include <string>

#include <mysql/mysql.h>
#include "oatpp/network/server/Server.hpp"
#include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "appComponent.hpp"
#include "controller/loginController.hpp"
#include "controller/songController.hpp"
#include "database/base_repository.h"
#include "models/models.h"

namespace fs = std::filesystem;

//void run(const std::string& working_path)
void run(const Model::BinaryPath& bConf)
{
    appComponent component;

    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    auto logController = std::make_shared<Controller::loginController>(bConf);
    auto sngController = std::make_shared<Controller::songController>(bConf);
    logController->addEndpointsToRouter(router);
    sngController->addEndpointsToRouter(router);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::server::ConnectionHandler>, connectionHandler);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    oatpp::network::server::Server server(connectionProvider, connectionHandler);

    OATPP_LOGI("icarus", "Server running on port %s", connectionProvider->getProperty("port").getData());

    server.run();
}

int main(int argc, char **argv)
{
    oatpp::base::Environment::init();
    //const std::string working_path = argv[0];
    Model::BinaryPath bConf; 
    bConf.path = argv[0];

    //run(working_path);
    run(bConf);

    oatpp::base::Environment::destroy();

    return 0;
}

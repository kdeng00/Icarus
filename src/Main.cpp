#include <exception>
#include <iostream>
#include <filesystem>
#include <memory>
#include <string>

#include <mysql/mysql.h>
#include <oatpp/network/server/Server.hpp>
#include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "component/AppComponent.hpp"
#include "controller/ArtistController.hpp"
#include "controller/AlbumController.hpp"
#include "controller/CoverArtController.hpp"
#include "controller/GenreController.hpp"
#include "controller/LoginController.hpp"
#include "controller/SongController.hpp"
#include "controller/YearController.hpp"
#include "model/Models.h"

namespace fs = std::filesystem;

void run(const model::BinaryPath& bConf)
{
    component::AppComponent component;

    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    auto albumController = std::make_shared<controller::AlbumController>(bConf);
    auto artistController = std::make_shared<controller::ArtistController>(bConf);
    auto coverArtController = std::make_shared<controller::CoverArtController>(bConf);
    auto gnrController = std::make_shared<controller::GenreController>(bConf);
    auto logController = std::make_shared<controller::LoginController>(bConf);
    auto sngController = std::make_shared<controller::SongController>(bConf);
    auto yearController = std::make_shared<controller::YearController>(bConf);

    albumController->addEndpointsToRouter(router);
    artistController->addEndpointsToRouter(router);
    coverArtController->addEndpointsToRouter(router);
    gnrController->addEndpointsToRouter(router);
    logController->addEndpointsToRouter(router);
    sngController->addEndpointsToRouter(router);
    yearController->addEndpointsToRouter(router);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::server::ConnectionHandler>, connectionHandler);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    oatpp::network::server::Server server(connectionProvider, connectionHandler);

    OATPP_LOGI("icarus", "Server running on port %s", connectionProvider->getProperty("port").getData());

    server.run();
}

int main(int argc, char **argv)
{
    oatpp::base::Environment::init();
    
    model::BinaryPath bConf(argv[0]);

    run(bConf);

    oatpp::base::Environment::destroy();

    return 0;
}

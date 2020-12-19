#include <exception>
#include <iostream>
#include <filesystem>
#include <memory>
#include <string>

#include <mysql/mysql.h>
// #include <oatpp/network/server/Server.hpp>
// #include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
// #include "oatpp/web/server/HttpConnectionHandler.hpp"

#include <oatpp/web/server/HttpConnectionHandler.hpp>

// #include <oatpp/network/Server.hpp>
#include <oatpp/network/Server.hpp>
// #include <oatpp/network/server/SimpleTCPConnectionProvider.hpp>
#include <oatpp/network/tcp/server/ConnectionProvider.hpp>


#include "component/AppComponent.hpp"
#include "controller/ArtistController.hpp"
#include "controller/AlbumController.hpp"
#include "controller/CoverArtController.hpp"
#include "controller/GenreController.hpp"
#include "controller/LoginController.hpp"
#include "controller/RegisterController.hpp"
#include "controller/SongController.hpp"
#include "controller/YearController.hpp"
#include "model/Models.h"
#include "verify/Initialization.h"

namespace fs = std::filesystem;

void run(const model::BinaryPath& bConf) {
    component::AppComponent component;

    // OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    auto router = oatpp::web::server::HttpRouter::createShared();

    auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

    auto albumController = std::make_shared<controller::AlbumController>(bConf);
    auto artistController = std::make_shared<controller::ArtistController>(bConf);
    auto coverArtController = std::make_shared<controller::CoverArtController>(bConf);
    auto gnrController = std::make_shared<controller::GenreController>(bConf);
    auto logController = std::make_shared<controller::LoginController>(bConf);
    auto regController = std::make_shared<controller::RegisterController>(bConf);
    auto sngController = std::make_shared<controller::SongController>(bConf);
    auto yearController = std::make_shared<controller::YearController>(bConf);

    albumController->addEndpointsToRouter(router);
    artistController->addEndpointsToRouter(router);
    coverArtController->addEndpointsToRouter(router);
    gnrController->addEndpointsToRouter(router);
    logController->addEndpointsToRouter(router);
    regController->addEndpointsToRouter(router);
    sngController->addEndpointsToRouter(router);
    yearController->addEndpointsToRouter(router);

    // OATPP_COMPONENT(std::shared_ptr<oatpp::network::server::ConnectionHandler>, connectionHandler);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    oatpp::network::Server server(connectionProvider, connectionHandler);

    OATPP_LOGI("icarus", "Server running on port %s", connectionProvider->getProperty("port").getData());

    server.run();
}

int main(int argc, char **argv) {
    oatpp::base::Environment::init();
    
    model::BinaryPath bConf(std::move(argv[0]));

    if (argc > 1) {
        if (!verify::Initialization::skipVerification(argv[1])) {
            verify::Initialization::checkIcarus(bConf);
        } else {
            std::cout << "skiping verifyication\n";
        }
    } else {
        verify::Initialization::checkIcarus(bConf);
    }

    run(bConf);

    oatpp::base::Environment::destroy();

    return 0;
}

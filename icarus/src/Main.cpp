#include <iostream>
#include <memory>

#include <icarus_lib/icarus.h>
#include <mysql/mysql.h>
#include <oatpp/web/server/HttpConnectionHandler.hpp>
#include <oatpp/network/Server.hpp>
#include <oatpp/network/tcp/server/ConnectionProvider.hpp>


#include "component/AppComponent.hpp"
#include "controller/Controllers.h"
#include "verify/Initialization.h"

using std::cout;
using std::shared_ptr;


template<typename config>
void run(const config &b_conf)
{
    component::AppComponent component;

    auto router = oatpp::web::server::HttpRouter::createShared();

    auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);

    auto albumController = std::make_shared<controller::album_controller>(b_conf);
    auto artistController = std::make_shared<controller::artist_controller>(b_conf);
    auto coverArtController = std::make_shared<controller::cover_art_controller>(b_conf);
    auto gnrController = std::make_shared<controller::genre_controller>(b_conf);
    auto logController = std::make_shared<controller::login_controller>(b_conf);
    auto regController = std::make_shared<controller::register_controller>(b_conf);
    auto sngController = std::make_shared<controller::song_controller>(b_conf);
    auto yearController = std::make_shared<controller::year_controller>(b_conf);

    albumController->addEndpointsToRouter(router);
    artistController->addEndpointsToRouter(router);
    coverArtController->addEndpointsToRouter(router);
    gnrController->addEndpointsToRouter(router);
    logController->addEndpointsToRouter(router);
    regController->addEndpointsToRouter(router);
    sngController->addEndpointsToRouter(router);
    yearController->addEndpointsToRouter(router);

    OATPP_COMPONENT(shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    oatpp::network::Server server(connectionProvider, connectionHandler);

    OATPP_LOGI("icarus", "Server running on port %s", connectionProvider->getProperty("port").getData());

    server.run();
}

int main(int argc, char **argv)
{
    oatpp::base::Environment::init();
    
    icarus_lib::binary_path b_conf(std::move(argv[0]));

    if (argc > 1)
    {
        if (!verify::Initialization::skipVerification(argv[1]))
        {
            verify::Initialization::checkIcarus(b_conf);
        }
        else
        {
            cout << "skiping verifyication\n";
        }
    } 
    else
    {
        verify::Initialization::checkIcarus(b_conf);
    }

    run<icarus_lib::binary_path>(b_conf);

    oatpp::base::Environment::destroy();

    return 0;
}

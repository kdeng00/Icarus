#ifndef APPCOMPONENT_H_
#define APPCOMPONENT_H_

#include <memory>

#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

namespace component {
    class AppComponent {
    public:
        OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, 
                               serverConnectionProvider)([&]
        {
            return oatpp::network::tcp::server::ConnectionProvider::createShared({"127.0.0.1", appPort<v_uint16>(), oatpp::network::Address::IP_4});
        }());

        OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, httpRouter)([]
        {
            return oatpp::web::server::HttpRouter::createShared();
        }());

       OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, 
                serverConnectionHandler)([]
       {
            OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

            return oatpp::web::server::HttpConnectionHandler::createShared(router);
        }());

        OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, 
                apiObjectMapper)([] 
        {
            return oatpp::parser::json::mapping::ObjectMapper::createShared();
        }());
    private:
        template<typename Val>
        constexpr Val appPort() noexcept
        { 
            return 5002;
        }
    };
}

#endif

#ifndef APPCOMPONENT_H_
#define APPCOMPONENT_H_

#include <memory>

#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

using oatpp::network::ConnectionHandler;
using oatpp::network::tcp::server::ConnectionProvider;
using oatpp::network::ServerConnectionProvider;
using oatpp::web::server::HttpRouter;
using oatpp::web::server::HttpConnectionHandler;
using oatpp::data::mapping::ObjectMapper;

namespace component {
    class AppComponent {
    public:
        
        OATPP_CREATE_COMPONENT(std::shared_ptr<ServerConnectionProvider>, serverConnectionProvider)
            ([&]
            {
                return ConnectionProvider::createShared({"127.0.0.1", 
                                                         appPort<v_uint16>(), 
                                                         oatpp::network::Address::IP_4});
            }());

        OATPP_CREATE_COMPONENT(std::shared_ptr<HttpRouter>, httpRouter)
            ([]
            {
                return HttpRouter::createShared();
            }());

        OATPP_CREATE_COMPONENT(std::shared_ptr<ConnectionHandler>, serverConnectionHandler)
            ([]
            {
                OATPP_COMPONENT(std::shared_ptr<HttpRouter>, router);

                return HttpConnectionHandler::createShared(router);
            }());

        OATPP_CREATE_COMPONENT(std::shared_ptr<ObjectMapper>, apiObjectMapper)
            ([] 
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

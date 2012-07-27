#pragma once
#include <google/protobuf/message.h>
#include <memory>
#include "type/type.pb.h"

namespace navitia{ namespace gateway{

/**
 * Classe servant a transmettre des données entre les différents modules de la gateway
 *
 */
struct Context {
    enum Service{
        UNKNOWN,
        BAD_RESPONSE,
        QUERY //API traité par navitia
    };

    ///la réponse de navitia
    std::unique_ptr<pbnavitia::Response> pb;

    ///flag pour définir le services utilisé
    Service service;


    std::string str;



    Context() : service(UNKNOWN){}
};

}}

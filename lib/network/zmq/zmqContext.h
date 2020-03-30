//
// Created by Amin Rezaei on 3/28/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/30/20 08:13
//

#ifndef AGO_NETWORK_ZMQ_SOCKET_H
#define AGO_NETWORK_ZMQ_SOCKET_H

#include <zmq.hpp>

namespace agoNetwork {
    class zmqContext {
    protected: // protected data
        zmq::context_t _context{1};

    public: // constructors and destructors
        explicit
        zmqContext() = default;

        ~zmqContext() {
            _context.close();
        }

    protected: // protected method

    public: // public method
    };
}

#endif //AGO_NETWORK_ZMQ_SOCKET_H

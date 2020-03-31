//
// Created by Amin Rezaei on 3/28/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/31/20 15:18
//

#ifndef AGO_NETWORK_ZMQ_SOCKET_H
#define AGO_NETWORK_ZMQ_SOCKET_H

#include <zmq.hpp>

namespace agoNetwork {
	/// @brief **agoNetwork::zmqContext**
	/// is the fundamental for any class
	/// that wants to communicate with zmq sockets.
	class zmqContext {
	protected: // protected data
		/// @brief ZMQ Context witch is use for I/O communication
		zmq::context_t _context;

	public: // constructors and destructors
		/// @brief Initialize context to use 1 thread for I/O communication
		explicit
		zmqContext() noexcept;

		/// @brief Initialize context to use specified number of threads
		/// for I/O communication
		explicit
		zmqContext(unsigned int) noexcept;

		/// @brief Close the context
		~zmqContext() {
			_context.close();
		}
	};
}

#endif //AGO_NETWORK_ZMQ_SOCKET_H

//
// Created by Amin Rezaei on 3/28/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/31/20 15:18
//

#include <lib/network/zmq/zmqContext.h>

namespace agoNetwork {
	zmqContext::zmqContext(unsigned int io_thread)
	noexcept
			:_context{ static_cast<int>(io_thread) } { }

	zmqContext::zmqContext()
	noexcept
			:_context{ 1 } { }
}

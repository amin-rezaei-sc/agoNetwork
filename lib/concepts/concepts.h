//
// Created by Amin Rezaei on 3/28/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/31/20 15:20
//

#ifndef AGO_NETWORK_CONCEPTS_H
#define AGO_NETWORK_CONCEPTS_H

#include <string>
#include <vector>
#include <lib/network/socket/socket.h>

// string -> std::string
template<typename... T>
concept string = ((std::is_same<std::string, T>::value) && ...);

// routerCallback -> std::invocable
// void (
//          std::shared_ptr<agoNetwork::socket>,
//          const std::vector<std::string> &
//        )
template<typename... Callback>
concept routerCallback = (
		(std::is_invocable_r_v<
				void,
				Callback(
						const std::shared_ptr<agoNetwork::socket>&,
						const std::vector<std::string>&),
				std::shared_ptr<agoNetwork::socket>,
				const std::vector<std::string>&>
		) && ...);

// socket
template<typename... T>
concept Socket = (
		(std::is_same_v<agoNetwork::socketModel::tcp, T>
				|| std::is_same_v<agoNetwork::socketModel::ipc, T>
				|| std::is_same_v<agoNetwork::socketModel::inproc, T>
		) && ...);

// tcp
template<typename... T>
concept TcpSocket = (
		std::is_same_v<agoNetwork::socketModel::tcp, T> && ...
);

// ipc
template<typename... T>
concept IPCSocket = (
		std::is_same_v<agoNetwork::socketModel::ipc, T> && ...
);

// inproc
template<typename... T>
concept InProcSocket = (
		std::is_same_v<agoNetwork::socketModel::inproc, T> && ...
);

#endif //AGO_NETWORK_CONCEPTS_H

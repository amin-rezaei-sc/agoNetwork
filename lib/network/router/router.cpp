//
// Created by Amin Rezaei on 3/24/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/31/20 15:19
//

#include <regex>
#include <future>
#include <lib/network/router/router.h>

using namespace std::chrono_literals;

namespace agoNetwork {
	void router::
	registerSocket_(zmq::context_t& context,
			const socketModel::tcp& _socket) {
		if (not _socket.name.empty() && not _socket.address.empty()) {
			if (validateURI_(_socket.address)) {
				_tcpSocket.insert({
						_socket.name+"_.:tcp:._",
						std::make_shared<tcpSocket>(
								tcpSocket{
										_socket.name,
										_socket.address,
										socketType::router,
										context
								})
				});
			}
			else {
				throw (std::runtime_error(
						"Could not validate "
								+_socket.address
								+"\nvalid uri: ipv4:port"));
			}
		}
	}

	void router::
	registerSocket_(zmq::context_t& context,
			const socketModel::ipc& _socket)
	noexcept {
		if (not _socket.name.empty() && not _socket.address.empty()) {
			_ipcSocket.insert({
					_socket.name+"_.:ipc:._",
					std::make_shared<ipcSocket>(
							ipcSocket{
									_socket.name,
									_socket.address,
									socketType::router,
									context
							})
			});
		}
	}

	void router::
	registerSocket_(zmq::context_t& context,
			const socketModel::inproc& _socket)
	noexcept {
		if (not _socket.name.empty() && not _socket.address.empty()) {
			_inprocSocket.insert({
					_socket.name+"_.:inproc:._",
					std::make_shared<inprocSocket>(
							inprocSocket{
									_socket.name,
									_socket.address,
									socketType::router,
									context
							})
			});
		}
	}

	void router::
	bind_() noexcept {
		if (not listening_() && not bound_()) {
			auto _ = std::async(std::launch::async, [&] {
				for (const auto &[socketName, socket] : _tcpSocket) {
					try {
						socket->bind();
					}
					catch (zmq::error_t& error) {
						std::cout
								<< "Error in binding to tcp socket "
								<< socketName
								<< ", what? "
								<< error.what()
								<< std::endl;
					}
				}
			});
			auto __ = std::async(std::launch::async, [&] {
				for (const auto &[socketName, socket] : _ipcSocket) {
					try {
						socket->bind();
					}
					catch (zmq::error_t& error) {
						std::cout
								<< "Error in binding to ipc socket "
								<< socketName
								<< ", what? "
								<< error.what()
								<< std::endl;
					}
				}
			});
			auto ___ = std::async(std::launch::async, [&] {
				for (const auto &[socketName, socket] : _inprocSocket) {
					try {
						socket->bind();
					}
					catch (zmq::error_t& error) {
						std::cout
								<< "Error in binding to inproc socket "
								<< socketName
								<< ", what? "
								<< error.what()
								<< std::endl;
					}
				}
			});
			_.wait();
			__.wait();
			___.wait();
			status_(routerStatus::bound);
		}
	}

	void router::
	listen_() noexcept {
		auto _ = std::async(std::launch::async, [&] {
			listen_on_tcp_();
		});
		auto __ = std::async(std::launch::async, [&] {
			listen_on_ipc_();
		});
		auto ___ = std::async(std::launch::async, [&] {
			listen_on_inproc_();
		});
		_.wait();
		__.wait();
		___.wait();
	}

	void router::
	listen_on_tcp_() noexcept {
		if (not listeningOnTcp_()) {
			if (not _tcpSocket.empty()) {
				bind_();
				status_(routerStatus::listeningOnTcp);
				std::vector<zmq::pollitem_t> polls;
				std::vector<std::string> socketPairPoll;
				for (auto &[socketName, socket] : _tcpSocket) {
					polls.push_back(
							zmq::pollitem_t{
									static_cast<void*>(***socket),
									0,
									ZMQ_POLLIN,
									0
							}
					);
					socketPairPoll.push_back(socketName);
				}
				while (listeningOnTcp_()) {
					try {
						zmq::poll(polls);
						int socketIndex{ -1 };
						for (const auto& item : polls) {
							++socketIndex;
							if (item.revents==ZMQ_POLLIN) {
								auto _socket_name = socketPairPoll[socketIndex];
								auto _socket = _tcpSocket[_socket_name];
								auto req = _socket->receive();
								if (_tcpCallbacks.contains(_socket_name)) {
									auto[rangeBegin, rangeEnd] =
									_tcpCallbacks.equal_range(_socket_name);
									for (
											auto callback = rangeBegin;
											callback!=rangeEnd;
											++callback) {
										callback->second(_socket, req);
									}
								}
							}
						}
					}
					catch (...) { }
					std::this_thread::sleep_for(2s);
				}
			}
		}
	}

	void router::
	listen_on_ipc_() noexcept {
		if (not listeningOnIpc_()) {
			if (not _ipcSocket.empty()) {
				bind_();
				status_(routerStatus::listeningOnIpc);
				std::vector<zmq::pollitem_t> polls;
				std::vector<std::string> socketPairPoll;
				for (auto &[socketName, socket] : _ipcSocket) {
					polls.push_back(
							zmq::pollitem_t{
									static_cast<void*>(***socket),
									0,
									ZMQ_POLLIN,
									0
							}
					);
					socketPairPoll.push_back(socketName);
				}
				while (listeningOnIpc_()) {
					try {
						zmq::poll(polls);
						int socketIndex{ -1 };
						for (const auto& item : polls) {
							++socketIndex;
							if (item.revents==ZMQ_POLLIN) {
								auto _socket_name = socketPairPoll[socketIndex];
								auto _socket = _ipcSocket[_socket_name];
								auto req = _socket->receive();
								if (_ipcCallbacks.contains(_socket_name)) {
									auto[rangeBegin, rangeEnd] =
									_ipcCallbacks.equal_range(_socket_name);
									for (
											auto callback = rangeBegin;
											callback!=rangeEnd;
											++callback) {
										callback->second(_socket, req);
									}
								}
							}
						}
					}
					catch (...) { }
					std::this_thread::sleep_for(2s);
				}
			}
		}
	}

	void router::
	listen_on_inproc_() noexcept {
		if (not listeningOnInproc_()) {
			if (not _inprocSocket.empty()) {
				bind_();
				status_(routerStatus::listeningOnInproc);
				std::vector<zmq::pollitem_t> polls;
				std::vector<std::string> socketPairPoll;
				for (auto &[socketName, socket] : _inprocSocket) {
					polls.push_back(
							zmq::pollitem_t{
									static_cast<void*>(***socket),
									0,
									ZMQ_POLLIN,
									0
							}
					);
					socketPairPoll.push_back(socketName);
				}
				while (listeningOnInproc_()) {
					try {
						zmq::poll(polls);
						int socketIndex{ -1 };
						for (const auto& item : polls) {
							++socketIndex;
							if (item.revents==ZMQ_POLLIN) {
								auto _socket_name = socketPairPoll[socketIndex];
								auto _socket = _inprocSocket[_socket_name];
								auto req = _socket->receive();
								if (_inprocCallbacks.contains(_socket_name)) {
									auto[rangeBegin, rangeEnd] =
									_inprocCallbacks.equal_range(_socket_name);
									for (
											auto callback = rangeBegin;
											callback!=rangeEnd;
											++callback) {
										callback->second(_socket, req);
									}
								}
							}
						}
					}
					catch (...) { }
					std::this_thread::sleep_for(2s);
				}
			}
		}
	}

	bool router::
	validateURI_(const std::string& uri) const noexcept {
		std::regex tcpAddress{
				"^((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}"
				"([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])):([0-9]+)$"
		};
		return (std::regex_search(uri.c_str(), tcpAddress));
	}

	void router::
	status_(router::routerStatus&& status) noexcept {
		switch (_status) {
		case routerStatus::initialized ... routerStatus::listening: {
			_status = status;
			break;
		}
		case routerStatus::listeningOnTcp: {
			if (status==routerStatus::listeningOnIpc) {
				_status = routerStatus::listeningOnTcpAndIpc;
			}
			else if (status==routerStatus::listeningOnInproc) {
				_status = routerStatus::listeningOnTcpAndInproc;
			}
			break;
		}
		case routerStatus::listeningOnIpc: {
			if (status==routerStatus::listeningOnTcp) {
				_status = routerStatus::listeningOnTcpAndIpc;
			}
			else if (status==routerStatus::listeningOnInproc) {
				_status = routerStatus::listeningOnIpcAndInproc;
			}
			break;
		}
		case routerStatus::listeningOnInproc: {
			if (status==routerStatus::listeningOnTcp) {
				_status = routerStatus::listeningOnTcpAndInproc;
			}
			else if (status==routerStatus::listeningOnIpc) {
				_status = routerStatus::listeningOnIpcAndInproc;
			}
			break;
		}
		case routerStatus::listeningOnTcpAndIpc: {
			if (status==routerStatus::listeningOnInproc) {
				_status = routerStatus::listening;
			}
			break;
		}
		case routerStatus::listeningOnTcpAndInproc: {
			if (status==routerStatus::listeningOnIpc) {
				_status = routerStatus::listening;
			}
			break;
		}
		case routerStatus::listeningOnIpcAndInproc: {
			if (status==routerStatus::listeningOnTcp) {
				_status = routerStatus::listening;
			}
			break;
		}
		default: {
			_status = status;
			break;
		}
		}
	}

	bool router::
	bound_() const noexcept {
		return _status==routerStatus::bound;
	}

	bool router::
	listening_() const noexcept {
		return _status==routerStatus::listening;
	}

	bool router::
	listeningOnTcp_() const noexcept {
		return
				_status==routerStatus::listeningOnTcp
						|| _status==routerStatus::listeningOnTcpAndIpc
						|| _status==routerStatus::listeningOnTcpAndInproc;
	}

	bool router::
	listeningOnIpc_() const noexcept {
		return
				_status==routerStatus::listeningOnIpc
						|| _status==routerStatus::listeningOnTcpAndIpc
						|| _status==routerStatus::listeningOnIpcAndInproc;
	}

	bool router::
	listeningOnInproc_() const noexcept {
		return
				_status==routerStatus::listeningOnInproc
						|| _status==routerStatus::listeningOnTcpAndInproc
						|| _status==routerStatus::listeningOnIpcAndInproc;
	}

	void router::
	listen() noexcept {
		listen_();
	}

	void router::
	registerCallback_(
			const std::string& name,
			const router::tcp_callback& callback) noexcept {
		if (_tcpSocket.contains(name)) {
			_tcpCallbacks.insert({
					name,
					callback
			});
		}
	}

	void router::
	registerCallback_(
			const std::string& name,
			const router::ipc_callback& callback) noexcept {
		if (_ipcSocket.contains(name)) {
			_ipcCallbacks.insert({
					name,
					callback
			});
		}
	}

	void router::
	registerCallback_(
			const std::string& name,
			const router::inproc_callback& callback) noexcept {
		if (_inprocSocket.contains(name)) {
			_inprocCallbacks.insert({
					name,
					callback
			});
		}
	}
}

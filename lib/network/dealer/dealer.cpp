//
// Created by Amin Rezaei on 3/29/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/31/20 15:20
//

#include <regex>
#include <lib/network/dealer/dealer.h>

namespace agoNetwork {
	void dealer::
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
										socketType::dealer,
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

	void dealer::
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
									socketType::dealer,
									context
							})
			});
		}
	}

	void dealer::
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
									socketType::dealer,
									context
							})
			});
		}
	}

	bool dealer::
	validateURI_(const std::string& uri) const noexcept {
		std::regex tcpAddress{
				"^((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}"
				"([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])):([0-9]+)$"
		};
		return (std::regex_search(uri.c_str(), tcpAddress));
	}

	void dealer::
	tcpConnect_(const std::string& name) noexcept {
		_tcpSocket[name]->connect();
	}

	void dealer::
	ipcConnect_(const std::string& name) noexcept {
		_ipcSocket[name]->connect();
	}

	void dealer::
	inprocConnect_(const std::string& name) noexcept {
		_inprocSocket[name]->connect();
	}

	void dealer::
	send(const std::string& name, const std::string& message)
	noexcept {
		if (_tcpSocket.contains(name)) {
			tcpConnect_(name);
			_tcpSocket[name]->send(
					_tcpSocket[name]->address(),
					message);
		}
		else if (_ipcSocket.contains(name)) {
			ipcConnect_(name);
			_ipcSocket[name]->send(
					_ipcSocket[name]->address(),
					message);
		}
		else if (_inprocSocket.contains(name)) {
			inprocConnect_(name);
			_inprocSocket[name]->send(
					_inprocSocket[name]->address(),
					message);
		}
	}
}

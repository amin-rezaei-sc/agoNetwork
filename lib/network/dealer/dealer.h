//
// Created by Amin Rezaei on 3/29/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/31/20 15:20
//

#ifndef AGO_NETWORK_DEALER_H
#define AGO_NETWORK_DEALER_H

#include <lib/concepts/concepts.h>
#include <lib/network/zmq/zmqContext.h>

namespace agoNetwork {
	/// @brief **dealer** is the *zmq dealer* adapter
	/// which brings communication functionality.
	class dealer final : private zmqContext {
	private: // private data
		/// Maps socket name to a tcpSocket shared pointer.
		std::unordered_map
				<std::string, std::shared_ptr<tcpSocket>> _tcpSocket;
		/// Maps socket name to a ipcSocket shared pointer.
		std::unordered_map
				<std::string, std::shared_ptr<ipcSocket>> _ipcSocket;
		/// Maps socket name to a inprocSocket shared pointer.
		std::unordered_map
				<std::string, std::shared_ptr<inprocSocket>> _inprocSocket;

	public: // constructors and destructors
		/// @brief Registers sockets.
		/// The dealer constructor simply calls the
		/// dealer::registerSocket_ function
		/// and passes the dealer::_context and socket respectively.
		/// @tparam socket_t is ::Socket concept which is either
		/// agoNetwork::socketModel::tcp,
		/// agoNetwork::socketModel::ipc or
		/// agoNetwork::socketModel::inproc.
		/// @see concepts.h
		/// @param socket is socket_t parameter pack.
		template<Socket... socket_t>
		explicit
		dealer(socket_t ... socket) noexcept {
			(registerSocket_(_context, socket), ...);
		}

	private:
		/// @brief Registers tcp sockets in router::_tcpSocket.
		/// @warning This function could throw a runtime error if the specified
		/// address (URI) of the tcp socket be invalid.
		/// @see router::validateURI_
		void
		registerSocket_(zmq::context_t&, const socketModel::tcp&);

		/// @brief Registers ipc sockets in router::_ipcSocket.
		void
		registerSocket_(zmq::context_t&, const socketModel::ipc&)
		noexcept;

		/// @brief Registers inproc sockets in router::_inprocSocket.
		void
		registerSocket_(zmq::context_t&, const socketModel::inproc&)
		noexcept;

	private: // private methods
		/// @brief Make the specified tcp socket (by its name)
		/// connect to its address.
		void
		tcpConnect_(const std::string&) noexcept;
		/// @brief Make the specified ipc socket (by its name)
		/// connect to its address.
		void
		ipcConnect_(const std::string&) noexcept;
		/// @brief Make the specified inproc socket (by its name)
		/// connect to its address.
		void
		inprocConnect_(const std::string&) noexcept;

	private:
		/// @brief Validate specified uri for the tcp protocol.
		/// valid uri for the tcp protocol is <IPV4>:<PORT>
		/// @return true if uri was valid and false otherwise.
		[[nodiscard]]
		bool
		validateURI_(const std::string&) const noexcept;

	public: // public methods
		/// @brief Make the specified inproc socket (by its name)
		/// send a message to its connected pair.
		void
		send(const std::string&, const std::string&) noexcept;
	};
}

#endif //AGO_NETWORK_DEALER_H

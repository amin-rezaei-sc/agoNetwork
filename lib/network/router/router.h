//
// Created by Amin Rezaei on 3/24/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/31/20 15:18
//

#ifndef AGO_NETWORK_LIBRARY_H
#define AGO_NETWORK_LIBRARY_H

#include <iostream>
#include <lib/concepts/concepts.h>
#include <lib/network/socket/socket.h>
#include <lib/network/zmq/zmqContext.h>
#include <map>

namespace agoNetwork {
	/// @brief **router** is the *zmq router* adapter
	/// which brings communication functionality.
	class router final : private zmqContext {
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
		/// callback is a function alias which gets a shared pointer
		/// to agoNetwork::socket and a vector of strings representing
		/// client address and request message.
		using callback =
		std::function<void(const std::shared_ptr<agoNetwork::socket>&,
				const std::vector<std::string>&)>;
		/// tcp_callback is a callback
		/// which gets tcpSocket share pointer as its first parameter.
		using tcp_callback =
		std::function<void(const std::shared_ptr<agoNetwork::tcpSocket>&,
				const std::vector<std::string>&)>;
		/// ipc_callback is a callback
		/// which gets ipcSocket share pointer as its first parameter.
		using ipc_callback =
		std::function<void(const std::shared_ptr<agoNetwork::ipcSocket>&,
				const std::vector<std::string>&)>;
		/// inproc_callback is a callback
		/// which gets inprocSocket share pointer as its first parameter.
		using inproc_callback =
		std::function<void(const std::shared_ptr<agoNetwork::inprocSocket>&,
				const std::vector<std::string>&)>;
		/// Maps socket name to tcp_callback.
		std::unordered_multimap<std::string, tcp_callback> _tcpCallbacks;
		/// Maps socket name to ipc_callback.
		std::unordered_multimap<std::string, ipc_callback> _ipcCallbacks;
		/// Maps socket name to inproc_callback.
		std::unordered_multimap<std::string, inproc_callback> _inprocCallbacks;

	private: // status
		/// Represents router status.
		enum class routerStatus {
			initialized,
			bound,
			listening,
			listeningOnTcp,
			listeningOnIpc,
			listeningOnInproc,
			listeningOnTcpAndIpc,
			listeningOnTcpAndInproc,
			listeningOnIpcAndInproc,
		} _status{ routerStatus::initialized };

	public: // public data
	public: // constructors and destructors
		/// @brief Registers sockets.
		/// The router constructor simply calls the
		/// router::registerSocket_ function
		/// and passes the router::_context and socket respectively.
		/// @tparam socket_t is ::Socket concept which is either
		/// agoNetwork::socketModel::tcp,
		/// agoNetwork::socketModel::ipc or
		/// agoNetwork::socketModel::inproc.
		/// @see concepts.h
		/// @param socket is socket_t parameter pack.
		template<Socket... socket_t>
		explicit
		router(socket_t... socket) noexcept {
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

	private:
		/// @brief Registers router::tcp_callback in router::_tcpCallbacks.
		void
		registerCallback_(const std::string&, const tcp_callback&) noexcept;

		/// @brief Registers router::ipc_callback in router::_ipcCallbacks.
		void
		registerCallback_(const std::string&, const ipc_callback&) noexcept;

		/// @brief Registers router::inproc_callback in router::_inprocCallbacks.
		void
		registerCallback_(const std::string&, const inproc_callback&) noexcept;

	public:
		/// @brief Registers callbacks.
		/// @tparam routerCallback_ is ::routerCallback concept which is
		/// a function that its parameters are
		/// - agoNetwork::socket shared pointer which could be the following:
		/// 	+ agoNetwork::tcpSocket
		/// 	+ agoNetwork::ipcSocket
		/// 	+ agoNetwork::inprocSocket
		/// - std::vector of std::string
		/// @note both of the template parameters are const references.
		/// @param name Name of the registered socket
		/// @param callback_ Invocable object like a lambda
		template<routerCallback... routerCallback_>
		void
		registerCallback(
				const std::string& name,
				const routerCallback_& ... callback_)
		noexcept {
			(registerCallback_(name, callback_), ...);
		}

	private: // private methods
		/// @brief Make all the registered sockets bind to their address.
		void
		bind_() noexcept;

		/// @brief Perform listening on all the registered sockets and call the
		/// corresponded callbacks.
		void
		listen_() noexcept;

		/// @brief Perform listening on all the registered tcp sockets and call
		/// the corresponded callbacks.
		void
		listen_on_tcp_() noexcept;

		/// @brief Perform listening on all the registered ipc sockets and call
		/// the corresponded callbacks.
		void
		listen_on_ipc_() noexcept;

		/// @brief Perform listening on all the registered inproc sockets and
		/// call the corresponded callbacks.
		void
		listen_on_inproc_() noexcept;

	private:
		/// @brief Validate specified uri for the tcp protocol.
		/// valid uri for the tcp protocol is <IPV4>:<PORT>
		/// @return true if uri was valid and false otherwise.
		[[nodiscard]]
		bool
		validateURI_(const std::string&) const noexcept;

		/// @brief Setter for the router::_status.
		void
		status_(routerStatus&&) noexcept;

		/// @brief Specify whether the router is bound.
		/// @return true if the router is bound and false otherwise.
		[[nodiscard]]
		bool
		bound_() const noexcept;

		/// @brief Specify whether all sockets are listening.
		/// @return true if all sockets are listening and false otherwise.
		[[nodiscard]]
		bool
		listening_() const noexcept;

		/// @brief Specify whether tcp sockets are listening.
		/// @return true if tcp sockets are listening.
		[[nodiscard]]
		bool
		listeningOnTcp_() const noexcept;

		/// @brief Specify whether ipc sockets are listening.
		/// @return true if ipc sockets are listening.
		[[nodiscard]]
		bool
		listeningOnIpc_() const noexcept;

		/// @brief Specify whether inproc sockets are listening.
		/// @return true if inproc sockets are listening.
		[[nodiscard]]
		bool
		listeningOnInproc_() const noexcept;

	public: // public methods
		/// @brief Make all the sockets start listening.
		void
		listen() noexcept;
	};
} // namespace agoNetwork

#endif // AGO_NETWORK_LIBRARY_H

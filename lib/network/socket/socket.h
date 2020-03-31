//
// Created by Amin Rezaei on 3/29/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/31/20 15:19
//

#ifndef AGO_NETWORK_SOCKET_H
#define AGO_NETWORK_SOCKET_H

#include <string>
#include <memory>
#include <zmq.hpp>

namespace agoNetwork {
	/// @brief Represents zmq socket types.
	enum class socketType {
		request = ZMQ_REQ,
		reply = ZMQ_REP,
		dealer = ZMQ_DEALER,
		router = ZMQ_ROUTER,
	};
	/// @brief Represents communication protocols.
	enum class protocol {
		ipc,
		inproc,
		tcp
	};

	/// @brief **agoNetwork::socket** is an abstract class which
	/// brings socket information
	/// such as:
	/// - socket name
	/// - socket address
	/// - the socket itself
	/// - socket communication protocol
	/// - socket type
	/// @see agoNetwork::socketType
	/// @see agoNetwork::protocol
	class socket {
	protected: // protected data
		/// @brief Socket name.
		/// It used to specify the socket by its name.
		std::string _socketName;
		/// @brief Socket address.
		/// It used to connect or bind the socket.
		/// @see agoNetwork::router::bind_
		/// @see agoNetwork::dealer::tcpConnect_
		/// @see agoNetwork::dealer::ipcConnect_
		/// @see agoNetwork::dealer::inprocConnect_
		std::string _socketAddress;
		/// @brief The socket which is of type zmq_socket_t shared pointer.
		std::shared_ptr<zmq::socket_t> _socket;
		/// @brief Socket agoNetwork::protocol
		/// which initialized with agoNetwork::protocol::tcp.
		protocol _protocol{ protocol::tcp };
		/// @brief Socket agoNetwork::socketType
		/// which initialized with agoNetwork::socketType::router.
		socketType _socketType{ socketType::router };

	public: // constructors and destructors
		explicit
		socket() = default;

		explicit
		socket(
				std::string,
				std::string,
				protocol&&,
				socketType&&,
				zmq::context_t&
		) noexcept;

	public: // public methods
		/// @brief Overloaded operator*
		/// which is used to access to the zmq socket
		/// @return shared pointer of zmq_socket_t
		virtual std::shared_ptr<zmq::socket_t>
		operator*() const noexcept;

		/// @brief bind function should bring socket binding functionality
		virtual void
		bind() const noexcept = 0;

		/// @brief bind function should bring socket connecting functionality
		virtual void
		connect() const noexcept = 0;

		/// @brief Send a message to the specified address.
		virtual void
		send(const std::string&, const std::string&) noexcept;

		/// @brief Receives a message.
		/// @return The received message.
		virtual std::vector<std::string>
		receive() noexcept;

		/// @brief Specify the socket name.
		/// @return The socket name.
		virtual std::string
		name() noexcept;

		/// @brief Specify the socket address.
		/// @return The socket address.
		virtual std::string
		address() noexcept;
	};

	/// @brief **agoNetwork::tcpSocket**
	/// which is derived from agoNetwork::socket.
	/// Overrides all the super class functionality
	class tcpSocket final : private socket {
	public: // constructors and destructors
		explicit
		tcpSocket() = default;

		explicit
		tcpSocket(
				std::string,
				std::string,
				socketType&&,
				zmq::context_t&
		) noexcept;

	public:
		std::shared_ptr<zmq::socket_t>
		operator*() const noexcept override;

		void
		bind() const noexcept override;

		void
		connect() const noexcept override;

		void
		send(const std::string&, const std::string&) noexcept override;

		std::vector<std::string>
		receive() noexcept override;

		std::string
		name() noexcept override;

		std::string
		address() noexcept override;
	};

	/// @brief **agoNetwork::ipcSocket**
	/// which is derived from agoNetwork::socket.
	/// Overrides all the super class functionality
	class ipcSocket final : private socket {
	public: // constructors and destructors
		explicit
		ipcSocket(
				std::string,
				std::string,
				socketType&&,
				zmq::context_t&
		) noexcept;

	public:
		std::shared_ptr<zmq::socket_t>
		operator*() const noexcept override;

		void
		bind() const noexcept override;

		void
		connect() const noexcept override;

		void
		send(const std::string&, const std::string&) noexcept override;

		std::vector<std::string>
		receive() noexcept override;

		std::string
		name() noexcept override;

		std::string
		address() noexcept override;
	};

	/// @brief **agoNetwork::inprocSocket**
	/// which is derived from agoNetwork::socket.
	/// Overrides all the super class functionality
	class inprocSocket final : private socket {
	public: // constructors and destructors
		explicit
		inprocSocket(
				std::string,
				std::string,
				socketType&&,
				zmq::context_t&
		) noexcept;

	public:
		std::shared_ptr<zmq::socket_t>
		operator*() const noexcept override;

		void
		bind() const noexcept override;

		void
		connect() const noexcept override;

		void
		send(const std::string&, const std::string&) noexcept override;

		std::vector<std::string>
		receive() noexcept override;

		std::string
		name() noexcept override;

		std::string
		address() noexcept override;
	};
}

namespace agoNetwork::socketModel {
	class tcp {
	public:
		std::string name;
		std::string address;
	};

	class ipc {
	public:
		std::string name;
		std::string address;
	};

	class inproc {
	public:
		std::string name;
		std::string address;
	};
}

namespace agoNetwork::literals {
	/// @brief Used in order to distinguish tcp socket from other sockets
	/// with the same name.
	/// @return The specified name with a unique string.
	std::string
	operator "" _tcp(const char*, size_t) noexcept;

	/// @brief Used in order to distinguish ipc socket from other sockets
	/// with the same name.
	/// @return The specified name with a unique string.
	std::string
	operator "" _ipc(const char*, size_t) noexcept;

	/// @brief Used in order to distinguish inproc socket from other sockets
	/// with the same name.
	/// @return The specified name with a unique string.
	std::string
	operator "" _inproc(const char*, size_t) noexcept;
}

#endif //AGO_NETWORK_SOCKET_H

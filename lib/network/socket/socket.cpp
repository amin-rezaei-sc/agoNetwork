//
// Created by Amin Rezaei on 3/29/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/30/20 08:12
//

#include <lib/network/socket/socket.h>
#include <lib/network/zmq/zhelpers.hpp>

namespace agoNetwork {
    socket::
    socket(
            std::string socketName,
            std::string socketAddress,
            protocol &&aProtocol,
            socketType &&socket_type,
            zmq::context_t &context
    ) noexcept :
            _socketName{std::move(socketName)},
            _socketAddress{std::move(socketAddress)},
            _protocol{aProtocol},
            _socketType{socket_type},
            _socket{std::make_shared<zmq::socket_t>
                            (context, static_cast<int>(socket_type))} {}

    std::shared_ptr<zmq::socket_t> socket::
    operator*() const noexcept {
        return _socket;
    }

    void socket::
    send(const std::string &address, const std::string &string) noexcept {
        switch (_socketType) {
            case socketType::router: {
                s_sendmore(*_socket, address);
                s_sendmore(*_socket, "");
                s_send(*_socket, string);
                break;
            }
            case socketType::dealer: {
                s_sendmore(*_socket, "");
                s_send(*_socket, string);
                break;
            }
            case socketType::request ... socketType::reply: {
                s_send(*_socket, string);
                break;
            }
        }
    }

    std::vector<std::string> socket::
    receive() noexcept {
        switch (_socketType) {
            case socketType::router: {
                const auto remoteAddress = s_recv(*_socket);
                const auto emptyFrame = s_recv(*_socket);
                const auto message = s_recv(*_socket);
                return {remoteAddress, message};
            }
            case socketType::dealer: {
                const auto emptyFrame = s_recv(*_socket);
                const auto message = s_recv(*_socket);
                return {message};
            }
            case socketType::request: {
                const auto message = s_recv(*_socket);
                return {message};
            }
            case socketType::reply: {
                const auto message = s_recv(*_socket);
                return {message};
            }
            default: {
                return {};
            }
        }
    }

    tcpSocket::
    tcpSocket(
            std::string socketName,
            std::string socketAddress,
            socketType &&socket_type,
            zmq::context_t &context
    ) noexcept : socket{
            std::move(socketName),
            std::move(socketAddress),
            protocol::tcp,
            static_cast<socketType &&>(socket_type),
            context
    } {}

    void tcpSocket::
    bind() const noexcept {
        if (_socketType == socketType::router) {
            try {
                _socket->bind("tcp://" + _socketAddress);
            } catch (zmq::error_t &error) {
                std::cout << "Error in binding to tcp socket " << _socketName << " on address " << _socketAddress
                          << ", what? " << error.what() << std::endl;
            }
        }
    }

    void tcpSocket::
    connect() const noexcept {
        if (_socketType == socketType::dealer) {
            _socket->connect("tcp://" + _socketAddress);
        }
    }

    std::shared_ptr<zmq::socket_t> tcpSocket::
    operator*() const noexcept {
        return _socket;
    }

    std::vector<std::string> tcpSocket::
    receive() noexcept {
        return socket::receive();
    }

    void tcpSocket::
    send(const std::string &address, const std::string &string) noexcept {
        socket::send(address, string);
    }

    std::string tcpSocket::
    name() noexcept {
        return _socketName;
    }

    ipcSocket::
    ipcSocket(
            std::string socketName,
            std::string socketAddress,
            socketType &&socket_type,
            zmq::context_t &context
    ) noexcept : socket{
            std::move(socketName),
            std::move(socketAddress),
            protocol::ipc,
            static_cast<socketType &&>(socket_type),
            context
    } {}

    void ipcSocket::
    bind() const noexcept {
        if (_socketType == socketType::router) {
            try {
                _socket->bind("ipc://" + _socketAddress);
            } catch (zmq::error_t &error) {
                std::cout << "Error in binding to ipc socket " << _socketName << " on address " << _socketAddress
                          << ", what? " << error.what() << std::endl;
            }
        }
    }

    void ipcSocket::
    connect() const noexcept {
        if (_socketType == socketType::dealer) {
            _socket->connect("ipc://" + _socketAddress);
        }
    }

    std::shared_ptr<zmq::socket_t> ipcSocket::
    operator*() const noexcept {
        return _socket;
    }

    std::vector<std::string> ipcSocket::
    receive() noexcept {
        return socket::receive();
    }

    void ipcSocket::
    send(const std::string &address, const std::string &string) noexcept {
        socket::send(address, string);
    }

    inprocSocket::
    inprocSocket(
            std::string socketName,
            std::string socketAddress,
            socketType &&socket_type,
            zmq::context_t &context
    ) noexcept : socket{
            std::move(socketName),
            std::move(socketAddress),
            protocol::inproc,
            static_cast<socketType &&>(socket_type),
            context
    } {}

    void inprocSocket::
    bind() const noexcept {
        if (_socketType == socketType::router) {
            try {
                _socket->bind("inproc://" + _socketAddress);
            } catch (zmq::error_t &error) {
                std::cout << "Error in binding to inproc socket " << _socketName << " on address " << _socketAddress
                          << ", what? " << error.what() << std::endl;
            }
        }
    }

    void inprocSocket::
    connect() const noexcept {
        if (_socketType == socketType::dealer) {
            _socket->connect("inproc://" + _socketAddress);
        }
    }

    std::shared_ptr<zmq::socket_t> inprocSocket::
    operator*() const noexcept {
        return _socket;
    }

    std::vector<std::string> inprocSocket::
    receive() noexcept {
        return socket::receive();
    }

    void inprocSocket::
    send(const std::string &address, const std::string &string) noexcept {
        socket::send(address, string);
    }

    std::string literals::operator ""_tcp(const char *name, size_t) noexcept {
        return std::string(name) + "_.:tcp:._";
    }

    std::string literals::operator ""_ipc(const char *name, size_t) noexcept {
        return std::string(name) + "_.:ipc:._";
    }

    std::string literals::operator ""_inproc(const char *name, size_t) noexcept {
        return std::string(name) + "_.:inproc:._";
    }
}

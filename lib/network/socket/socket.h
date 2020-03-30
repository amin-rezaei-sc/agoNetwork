//
// Created by Amin Rezaei on 3/29/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/30/20 08:12
//

#ifndef AGO_NETWORK_SOCKET_H
#define AGO_NETWORK_SOCKET_H

#include <string>
#include <memory>
#include <zmq.hpp>

namespace agoNetwork {
    enum class socketType {
        request = ZMQ_REQ,
        reply = ZMQ_REP,
        dealer = ZMQ_DEALER,
        router = ZMQ_ROUTER,
    };
    enum class protocol {
        ipc,
        inproc,
        tcp
    };

    class socket {
    protected: // protected data
        std::string _socketName;
        std::string _socketAddress;
        std::shared_ptr<zmq::socket_t> _socket;
        protocol _protocol{protocol::tcp};
        socketType _socketType{socketType::router};

    public: // constructors and destructors
        explicit
        socket() = default;

        explicit
        socket(
                std::string,
                std::string,
                protocol &&,
                socketType &&,
                zmq::context_t &
        ) noexcept;

    public: // public methods
        virtual std::shared_ptr<zmq::socket_t>
        operator*() const noexcept;

        virtual void
        bind() const noexcept = 0;

        virtual void
        connect() const noexcept = 0;

        virtual void
        send(const std::string &, const std::string &) noexcept;

        virtual std::vector<std::string>
        receive() noexcept;

    };

    class tcpSocket final : private socket {
    public: // constructors and destructors
        explicit
        tcpSocket() = default;

        explicit
        tcpSocket(
                std::string,
                std::string,
                socketType &&,
                zmq::context_t &
        ) noexcept;

    public:
        std::shared_ptr<zmq::socket_t>
        operator*() const noexcept override;

        void
        bind() const noexcept override;

        void
        connect() const noexcept override;

        void
        send(const std::string &, const std::string &) noexcept override;

        std::vector<std::string>
        receive() noexcept override;

        std::string
        name() noexcept;
    };

    class ipcSocket final : private socket {
    public: // constructors and destructors
        explicit
        ipcSocket(
                std::string,
                std::string,
                socketType &&,
                zmq::context_t &
        ) noexcept;

    public:
        std::shared_ptr<zmq::socket_t>
        operator*() const noexcept override;

        void
        bind() const noexcept override;

        void
        connect() const noexcept override;

        void
        send(const std::string &, const std::string &) noexcept override;

        std::vector<std::string>
        receive() noexcept override;
    };

    class inprocSocket final : private socket {
    public: // constructors and destructors
        explicit
        inprocSocket(
                std::string,
                std::string,
                socketType &&,
                zmq::context_t &
        ) noexcept;

    public:
        std::shared_ptr<zmq::socket_t>
        operator*() const noexcept override;

        void
        bind() const noexcept override;

        void
        connect() const noexcept override;

        void
        send(const std::string &, const std::string &) noexcept override;

        std::vector<std::string>
        receive() noexcept override;
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
    std::string
    operator "" _tcp(const char *, size_t) noexcept;

    std::string
    operator "" _ipc(const char *, size_t) noexcept;

    std::string
    operator "" _inproc(const char *, size_t) noexcept;
}

#endif //AGO_NETWORK_SOCKET_H

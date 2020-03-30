//
// Created by Amin Rezaei on 3/24/20.
// Copyright (c) 2020. All rights reserved.
// Contact amin.rezaei.sc@gmail.com
// Last edit on 3/30/20 08:11
//

#ifndef AGO_NETWORK_LIBRARY_H
#define AGO_NETWORK_LIBRARY_H

#include <map>
#include <iostream>
#include <lib/network/socket/socket.h>
#include <lib/network/zmq/zmqContext.h>
#include <lib/concepts/concepts.h>

namespace agoNetwork {
    class router final : private zmqContext {
    private: // private data
        std::unordered_map<std::string, std::shared_ptr<tcpSocket>> _tcpSocket;
        std::unordered_map<std::string, std::shared_ptr<ipcSocket>> _ipcSocket;
        std::unordered_map<std::string, std::shared_ptr<inprocSocket>> _inprocSocket;
        using callback = std::function<void(const std::shared_ptr<agoNetwork::socket>&, const std::vector<std::string> &)>;
        using tcp_callback = std::function<void(const std::shared_ptr<agoNetwork::tcpSocket>&, const std::vector<std::string> &)>;
        using ipc_callback = std::function<void(const std::shared_ptr<agoNetwork::ipcSocket>&, const std::vector<std::string> &)>;
        using inproc_callback = std::function<void(const std::shared_ptr<agoNetwork::inprocSocket>&, const std::vector<std::string> &)>;
        std::unordered_multimap<std::string, tcp_callback> _tcpCallbacks;
        std::unordered_multimap<std::string, ipc_callback> _ipcCallbacks;
        std::unordered_multimap<std::string, inproc_callback> _inprocCallbacks;

    private: // status
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
        } _status{routerStatus::initialized};

    public: // public data

    public: // constructors and destructors
        template<Socket... socket_t>
        explicit
        router(socket_t... socket) noexcept {
            (registerSocket_(_context, socket), ...);
        }

    private: // registering sockets
        void
        registerSocket_(zmq::context_t &, const socketModel::tcp &) noexcept(false);

        void
        registerSocket_(zmq::context_t &, const socketModel::ipc &) noexcept;

        void
        registerSocket_(zmq::context_t &, const socketModel::inproc &) noexcept;

        // registering callbacks
    private:
        void
        registerCallback_(const std::string &, const tcp_callback &) noexcept;
        void
        registerCallback_(const std::string &, const ipc_callback &) noexcept;
        void
        registerCallback_(const std::string &, const inproc_callback &) noexcept;

    public:
        template<routerCallback... routerCallback_>
        void
        registerCallback(const std::string &name, const routerCallback_ &...callback_) noexcept {
            (registerCallback_(name, callback_), ...);
        }

    private: // private methods
        void
        bind_() noexcept;

        void
        listen_() noexcept;

        void
        listen_on_tcp_() noexcept;

        void
        listen_on_ipc_() noexcept;

        void
        listen_on_inproc_() noexcept;

    private:
        [[nodiscard]]
        bool
        validateURI_(const std::string &) const noexcept;

        void
        status_(routerStatus &&) noexcept;

        [[nodiscard]]
        bool
        bound_() const noexcept;

        [[nodiscard]]
        bool
        listening_() const noexcept;

        [[nodiscard]]
        bool
        listeningOnTcp_() const noexcept;

        [[nodiscard]]
        bool
        listeningOnIpc_() const noexcept;

        [[nodiscard]]
        bool
        listeningOnInproc_() const noexcept;

    public: // public methods
        void
        listen() noexcept;
    };
}

#endif //AGO_NETWORK_LIBRARY_H

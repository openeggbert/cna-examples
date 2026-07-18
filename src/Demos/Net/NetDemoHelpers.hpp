// SPDX-License-Identifier: MIT
#pragma once

#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionEndReason.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionState.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "Microsoft/Xna/Framework/Net/SendDataOptions.hpp"

namespace CnaExamples::Demos::Net {

using Microsoft::Xna::Framework::Net::NetworkSession;
using Microsoft::Xna::Framework::Net::NetworkSessionEndReason;
using Microsoft::Xna::Framework::Net::NetworkSessionState;
using Microsoft::Xna::Framework::Net::NetworkSessionType;
using Microsoft::Xna::Framework::Net::SendDataOptions;

// NetworkSession::Create()/Find()/Join() all hand back a caller-owned raw
// pointer (see NetworkSession's own class doc comment); every demo screen
// that owns one calls this from UnloadContent() rather than repeating the
// Dispose()+delete+null sequence inline.
inline void EndSession(NetworkSession*& session) {
    if (session != nullptr) {
        session->Dispose();
        delete session;
        session = nullptr;
    }
}

inline const char* SessionStateName(NetworkSessionState state) {
    switch (state) {
        case NetworkSessionState::Lobby:  return "Lobby";
        case NetworkSessionState::Playing: return "Playing";
        case NetworkSessionState::Ended:  return "Ended";
    }
    return "?";
}

inline const char* SessionTypeName(NetworkSessionType type) {
    switch (type) {
        case NetworkSessionType::Local:                 return "Local";
        case NetworkSessionType::SystemLink:             return "SystemLink";
        case NetworkSessionType::PlayerMatch:            return "PlayerMatch";
        case NetworkSessionType::Ranked:                 return "Ranked";
        case NetworkSessionType::LocalWithLeaderboards:  return "LocalWithLeaderboards";
    }
    return "?";
}

inline const char* EndReasonName(NetworkSessionEndReason reason) {
    switch (reason) {
        case NetworkSessionEndReason::ClientSignedOut:  return "ClientSignedOut";
        case NetworkSessionEndReason::HostEndedSession: return "HostEndedSession";
        case NetworkSessionEndReason::RemovedByHost:    return "RemovedByHost";
        case NetworkSessionEndReason::Disconnected:     return "Disconnected";
    }
    return "?";
}

inline const char* SendDataOptionsName(SendDataOptions options) {
    switch (options) {
        case SendDataOptions::None:            return "None";
        case SendDataOptions::Reliable:        return "Reliable";
        case SendDataOptions::InOrder:         return "InOrder";
        case SendDataOptions::ReliableInOrder: return "ReliableInOrder";
        case SendDataOptions::Chat:            return "Chat";
    }
    return "?";
}

} // namespace CnaExamples::Demos::Net

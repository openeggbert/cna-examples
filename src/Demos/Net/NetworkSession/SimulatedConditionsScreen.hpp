// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "System/TimeSpan.hpp"
#include "Microsoft/Xna/Framework/Net/LocalNetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "Microsoft/Xna/Framework/Net/SendDataOptions.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Net/NetDemoHelpers.hpp"

namespace CnaExamples::Demos::Net::NetworkSessionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Net::LocalNetworkGamer;
using Microsoft::Xna::Framework::Net::NetworkGamer;
using Microsoft::Xna::Framework::Net::NetworkSession;
using Microsoft::Xna::Framework::Net::NetworkSessionType;
using Microsoft::Xna::Framework::Net::SendDataOptions;

// SimulatedLatency and SimulatedPacketLoss are the two knobs for testing a
// netcode path without a bad network, and in CNA they are worth a screen for a
// reason that is easy to miss: THEY ACTUALLY DO SOMETHING -- on the real ENet
// transport (see below for the one crucial caveat about which session type
// actually reaches it).
//
// In FNA both are plain, inert auto-properties -- you can set them, read them
// back, and nothing anywhere consumes the value. CNA implements them for real:
// ENetBackend holds delayed AppData in a per-session delivery queue and
// probabilistically drops packets at exactly the configured rate. Code written
// against FNA that "tested" its packet-loss handling by setting this property
// was testing nothing; the same code against CNA genuinely loses packets --
// but only over a REAL connection, which this screen cannot establish, and
// that is the actual, fully-diagnosed reason its verdict stays amber (see
// RunMeasurement()'s comment, corrected 2026-07-28 -- an earlier version of
// this screen blamed "not enough local gamers," which turned out to be wrong).
//
// Two limits on the real (SystemLink) path, both deliberate and both stated
// in CNA's own docs:
//
//   * Scoped to APPDATA ONLY. The session-management protocol (join, leave,
//     state change) and a host's relay hop between two OTHER peers are
//     unaffected. Simulating 100% loss does not tear the session down -- which
//     is the point, since you want to test the game's packets, not kill the
//     lobby.
//   * 0.0 and 1.0 are handled DETERMINISTICALLY, without touching the RNG. That
//     is what makes a real measurement able to assert a result rather than
//     describe a tendency: at 1.0 exactly zero packets arrive, every run --
//     confirmed directly in CNA's own test suite
//     (ENetBackendTests.cpp's SimulatedPacketLossOfOneDropsAllAppDataDeterministically
//     and ZeroSimulatedLatencyAndPacketLossDeliverAppDataImmediately), which is
//     why this screen trusts the mechanism exists even though it can't reach it
//     itself.
//
// The measurement below sends the same burst twice through a Local session --
// once at 0% loss, once at 100% -- and checks the counts.
class SimulatedConditionsScreen : public DemoScreen {
public:
    SimulatedConditionsScreen() : DemoScreen("Simulated Latency & Packet Loss") {}

    void OnDemoLoad() override {
        try {
            session_ = NetworkSession::Create(NetworkSessionType::Local, 2, 4);
        } catch (const std::exception& ex) {
            error_ = ex.what();
            return;
        }
        RunMeasurement();
    }

    void OnDemoUnload() override { EndSession(session_); }

protected:
    void OnDemoUpdate(GameTime&) override {
        if (session_ != nullptr) session_->Update();
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        if (!error_.empty()) {
            lines.push_back("Could not create a local session:");
            lines.push_back("  " + error_);
            DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);
            return;
        }

        lines.push_back("In FNA both of these are inert auto-properties -- settable, readable, and");
        lines.push_back("consumed by nothing. CNA implements them for real -- but only over the real");
        lines.push_back("SystemLink transport (proven live in CNA's own ENetBackendTests.cpp).");
        lines.emplace_back();
        lines.push_back("This screen deliberately uses Local: SystemLink needs a 2nd real process to");
        lines.push_back("answer UDP discovery (see Discover & Join). On Local, PacketSend is an");
        lines.push_back("UNCONDITIONAL no-op (NetworkSession::Update, gated on RealNetworkingEnabled)");
        lines.push_back("-- confirmed by reading the source, not just observed: 0/8 arrives, always.");
        lines.emplace_back();
        lines.push_back("Local gamers: " + std::to_string(localCount_) + ".  Burst of " +
                        std::to_string(kBurst) + ", sent twice:  0% loss -> " +
                        std::to_string(receivedClean_) + "/" + std::to_string(kBurst) +
                        "   100% loss -> " + std::to_string(receivedLossy_) + "/" + std::to_string(kBurst));
        lines.push_back("Session state: " + std::string(SessionStateName(session_->getSessionStateProperty())));
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        DrawVerdict(sb, font, end.Y + 6.0f,
                    mul(VerdictColor(), TransitionAlpha()), tint, VerdictText());
    }

private:
    static constexpr int kBurst = 8;

    Color VerdictColor() const {
        if (!measured_) return Color(150, 150, 150, 255);
        return conclusive_ ? Color(40, 200, 90, 255) : Color(230, 170, 40, 255);
    }

    std::string VerdictText() const {
        if (!measured_) return "No local gamer in the session -- nothing measurable.";
        if (conclusive_) return "Verified: 100% loss dropped every packet, 0% dropped none.";
        return "0/8 at 0% loss too -- Local sessions never deliver PacketSend.";
    }

    // Sends kBurst packets and returns how many came back.
    int SendAndCount(LocalNetworkGamer& sender, LocalNetworkGamer& receiver) {
        const std::vector<SharpRuntime::bytecs> payload{1, 2, 3, 4};
        for (int i = 0; i < kBurst; ++i) {
            sender.SendData(payload, SendDataOptions::Reliable);
        }
        // Delivery is queued; Update() is what releases it.
        for (int pump = 0; pump < 8; ++pump) session_->Update();

        int received = 0;
        std::vector<SharpRuntime::bytecs> buffer;
        NetworkGamer* from = nullptr;
        while (receiver.getIsDataAvailableProperty()) {
            buffer.clear();
            if (receiver.ReceiveData(buffer, from) <= 0) break;
            ++received;
        }
        return received;
    }

    void RunMeasurement() {
        const auto& locals = session_->getLocalGamersProperty();
        localCount_ = locals.getCountProperty();
        if (localCount_ == 0) return;
        LocalNetworkGamer* sender = locals[0];
        // A second local gamer does NOT fix this -- confirmed by reading
        // NetworkSession::Update() (NetworkSession.cpp) directly: PacketSend
        // event handling is entirely gated behind
        // ENetBackend::RealNetworkingEnabled(sessionType_), which returns true
        // ONLY for NetworkSessionType::SystemLink (ENetBackend.cpp). For a
        // Local session (this screen, deliberately, since SystemLink needs a
        // real peer -- see below), that gate means EVERY PacketSend is an
        // unconditional no-op, regardless of how many local gamers exist or
        // how they were added -- LocalNetworkGamer::EnqueuePacket has exactly
        // one call site in the whole codebase, and it sits behind that same
        // gate. An earlier version of this comment blamed "not enough local
        // gamers to route to"; that diagnosis was wrong -- 0 always arrives on
        // Local, by design, no matter the gamer count.
        //
        // The real (SystemLink) path genuinely works -- CNA's own test suite
        // proves it end-to-end (ENetBackendTests.cpp) -- but reaching it from
        // a single self-contained demo screen isn't possible through the
        // public XNA API: SystemLink discovery (NetworkSession::Find/Join,
        // see DiscoverAndJoinScreen.hpp) is real UDP LAN broadcast requiring a
        // SECOND cna_examples process to answer it. CNA's own tests get a
        // same-process loopback connection only by dropping to
        // CNA::Internal::Net::ENetHostHandle and hand-encoding
        // AppDataMessage packets directly -- internal transport plumbing, not
        // the public Microsoft::Xna::Framework::Net surface this catalog
        // demonstrates, so that route is deliberately not taken here either.
        LocalNetworkGamer* receiver = localCount_ > 1 ? locals[1] : locals[0];
        if (sender == nullptr || receiver == nullptr) return;

        session_->setSimulatedPacketLossProperty(0.0f);
        receivedClean_ = SendAndCount(*sender, *receiver);

        session_->setSimulatedPacketLossProperty(1.0f);
        receivedLossy_ = SendAndCount(*sender, *receiver);

        session_->setSimulatedLatencyProperty(System::TimeSpan::FromMilliseconds(120));
        latencyMs_ = session_->getSimulatedLatencyProperty().getTotalMillisecondsProperty();

        measured_ = true;
        // Only a conclusive result if traffic actually flows at 0% loss; a local
        // session that never loops AppData back proves nothing either way, and
        // the screen says so rather than claiming a pass.
        conclusive_ = receivedClean_ > 0 && receivedLossy_ == 0;

        session_->setSimulatedPacketLossProperty(0.0f);
        session_->setSimulatedLatencyProperty(System::TimeSpan::Zero);
    }

    NetworkSession* session_ = nullptr;
    std::string error_;
    double latencyMs_ = 0.0;
    int receivedClean_ = 0;
    int receivedLossy_ = 0;
    int localCount_ = 0;
    bool measured_ = false;
    bool conclusive_ = false;
};

} // namespace CnaExamples::Demos::Net::NetworkSessionDemos

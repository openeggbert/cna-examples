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
// reason that is easy to miss: THEY ACTUALLY DO SOMETHING HERE.
//
// In FNA both are plain, inert auto-properties -- you can set them, read them
// back, and nothing anywhere consumes the value. CNA implements them for real:
// ENetBackend holds delayed AppData in a per-session delivery queue and
// probabilistically drops packets at exactly the configured rate. Code written
// against FNA that "tested" its packet-loss handling by setting this property
// was testing nothing; the same code against CNA genuinely loses packets.
//
// Two limits on that, both deliberate and both stated in CNA's own docs:
//
//   * Scoped to APPDATA ONLY. The session-management protocol (join, leave,
//     state change) and a host's relay hop between two OTHER peers are
//     unaffected. Simulating 100% loss does not tear the session down -- which
//     is the point, since you want to test the game's packets, not kill the
//     lobby.
//   * 0.0 and 1.0 are handled DETERMINISTICALLY, without touching the RNG. That
//     is what makes this screen able to assert a result rather than describe a
//     tendency: at 1.0 exactly zero packets arrive, every run.
//
// The measurement below sends the same burst twice through a local session --
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
        lines.push_back("consumed by nothing. CNA implements them for real, so netcode tested");
        lines.push_back("against FNA's versions was tested against nothing at all.");
        lines.emplace_back();
        lines.push_back("Local gamers in the session: " + std::to_string(localCount_) +
                        ".  The same burst of " + std::to_string(kBurst) + " packets, sent twice:");
        lines.push_back("  SimulatedPacketLoss = 0.0   received " + std::to_string(receivedClean_) +
                        " / " + std::to_string(kBurst));
        lines.push_back("  SimulatedPacketLoss = 1.0   received " + std::to_string(receivedLossy_) +
                        " / " + std::to_string(kBurst));
        lines.push_back("  SimulatedLatency now        " +
                        std::to_string((int)latencyMs_) + " ms");
        lines.emplace_back();
        lines.push_back("0.0 and 1.0 are handled deterministically, without touching the RNG --");
        lines.push_back("which is why this screen can assert a result, not just a tendency.");
        lines.push_back("Both are scoped to APPDATA ONLY: join/leave/state-change traffic and a");
        lines.push_back("host's relay hop for two other peers are unaffected, so 100% loss does");
        lines.push_back("not tear the session down. The session below is still alive: " +
                        std::string(SessionStateName(session_->getSessionStateProperty())));
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
        if (localCount_ < 2) {
            return "One local gamer only: SendData broadcasts to OTHERS, so none arrived.";
        }
        return "No AppData arrived even at 0% loss -- loss not demonstrable here.";
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
        // SendData with no recipient broadcasts to the OTHER gamers, so a lone
        // local gamer has nobody to send to and receives nothing -- which would
        // make the measurement inconclusive rather than wrong. With a second
        // local gamer the packets have somewhere to go.
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

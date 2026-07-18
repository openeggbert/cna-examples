// SPDX-License-Identifier: MIT
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "GameStateManagement/GameScreen.hpp"

#include "Demos/Input/Keyboard/LiveKeyStateScreen.hpp"
#include "Demos/Input/Keyboard/PressedKeysListScreen.hpp"
#include "Demos/Input/Keyboard/KeyTransitionLogScreen.hpp"
#include "Demos/Input/Keyboard/ModifiersAndLocksScreen.hpp"
#include "Demos/Input/Keyboard/TextInputScreen.hpp"
#include "Demos/Input/Keyboard/ScancodeKeycodeNamesScreen.hpp"
#include "Demos/Input/Keyboard/StateEqualityHashScreen.hpp"
#include "Demos/Input/Keyboard/PlayerIndexSlotsScreen.hpp"
#include "Demos/Input/Keyboard/SingleKeyQueryScreen.hpp"
#include "Demos/Input/Keyboard/KeyHoldDurationScreen.hpp"

#include "Demos/Input/Mouse/LivePositionButtonsScreen.hpp"
#include "Demos/Input/Mouse/ScrollWheelScreen.hpp"
#include "Demos/Input/Mouse/SetPositionScreen.hpp"
#include "Demos/Input/Mouse/CursorShapesScreen.hpp"
#include "Demos/Input/Mouse/ClickedEventScreen.hpp"
#include "Demos/Input/Mouse/RelativeModeScreen.hpp"
#include "Demos/Input/Mouse/GlobalPositionWarpScreen.hpp"
#include "Demos/Input/Mouse/CaptureScreen.hpp"
#include "Demos/Input/Mouse/StateEqualityHashScreen.hpp"
#include "Demos/Input/Mouse/ButtonTransitionLogScreen.hpp"

#include "Demos/Input/Gamepad/ConnectionAndCapabilitiesScreen.hpp"
#include "Demos/Input/Gamepad/LiveButtonsGridScreen.hpp"
#include "Demos/Input/Gamepad/DPadThumbsticksScreen.hpp"
#include "Demos/Input/Gamepad/TriggersScreen.hpp"
#include "Demos/Input/Gamepad/DeadZoneModesScreen.hpp"
#include "Demos/Input/Gamepad/VibrationScreen.hpp"
#include "Demos/Input/Gamepad/PowerInfoScreen.hpp"
#include "Demos/Input/Gamepad/PlayerIndexEXTScreen.hpp"
#include "Demos/Input/Gamepad/DeviceInfoScreen.hpp"
#include "Demos/Input/Gamepad/StateEqualityHashScreen.hpp"

#include "Demos/Input/Touch/LiveTouchesScreen.hpp"
#include "Demos/Input/Touch/CapabilitiesAndDisplayScreen.hpp"
#include "Demos/Input/Touch/TapAndDoubleTapScreen.hpp"
#include "Demos/Input/Touch/HoldAndFlickScreen.hpp"
#include "Demos/Input/Touch/DragGesturesScreen.hpp"
#include "Demos/Input/Touch/PinchGestureScreen.hpp"
#include "Demos/Input/Touch/AllGesturesLogScreen.hpp"
#include "Demos/Input/Touch/GestureQueueScreen.hpp"
#include "Demos/Input/Touch/TouchLocationQueryScreen.hpp"
#include "Demos/Input/Touch/PeakTouchesScreen.hpp"

#include "Demos/Input/Other/JoystickEnumerationScreen.hpp"
#include "Demos/Input/Other/JoystickLiveStateScreen.hpp"
#include "Demos/Input/Other/JoystickStateEqualityScreen.hpp"
#include "Demos/Input/Other/JoystickHotplugScreen.hpp"
#include "Demos/Input/Other/SensorsScreen.hpp"
#include "Demos/Input/Other/PowerScreen.hpp"
#include "Demos/Input/Other/HapticsScreen.hpp"
#include "Demos/Input/Other/InputDeviceEnumerationScreen.hpp"
#include "Demos/Input/Other/InputDeviceHotplugScreen.hpp"
#include "Demos/Input/Other/OtherSummaryScreen.hpp"

#include "Demos/Audio/SoundEffect/PlayToneScreen.hpp"
#include "Demos/Audio/SoundEffect/MasterVolumeAndSettingsScreen.hpp"
#include "Demos/Audio/SoundEffectInstance/PlaybackControlScreen.hpp"
#include "Demos/Audio/SoundEffectInstance/VolumePitchPanScreen.hpp"
#include "Demos/Audio/SoundEffectInstance/LoopingScreen.hpp"
#include "Demos/Audio/Audio3D/Apply3DScreen.hpp"
#include "Demos/Audio/Audio3D/DopplerDistanceScreen.hpp"
#include "Demos/Audio/DynamicSoundEffectInstance/StreamingSineWaveScreen.hpp"
#include "Demos/Audio/Microphone/MicrophoneEnumerationScreen.hpp"
#include "Demos/Audio/Microphone/MicrophoneCaptureScreen.hpp"

#include "Demos/Devices/Sensors/AccelerometerScreen.hpp"
#include "Demos/Devices/Sensors/GyroscopeScreen.hpp"
#include "Demos/Devices/Sensors/CompassScreen.hpp"
#include "Demos/Devices/Sensors/MotionScreen.hpp"
#include "Demos/Devices/Vibration/VibrateScreen.hpp"
#include "Demos/Devices/Camera/CameraCaptureScreen.hpp"
#include "Demos/Devices/SystemAndDisplay/SystemInfoScreen.hpp"
#include "Demos/Devices/SystemAndDisplay/DisplayInfoScreen.hpp"
#include "Demos/Devices/SystemAndDisplay/LocaleScreen.hpp"
#include "Demos/Devices/Power/PowerInfoScreen.hpp"
#include "Demos/Devices/DesktopIntegration/ClipboardScreen.hpp"
#include "Demos/Devices/DesktopIntegration/MessageBoxScreen.hpp"
#include "Demos/Devices/DesktopIntegration/FileDialogScreen.hpp"
#include "Demos/Devices/DesktopIntegration/UrlLauncherScreen.hpp"
#include "Demos/Devices/DesktopIntegration/SystemTrayScreen.hpp"

#include "Demos/Net/NetworkSession/LocalSessionLifecycleScreen.hpp"
#include "Demos/Net/NetworkSession/SystemLinkHostScreen.hpp"
#include "Demos/Net/NetworkSession/DiscoverAndJoinScreen.hpp"
#include "Demos/Net/NetworkSession/PacketRoundTripScreen.hpp"
#include "Demos/Net/NetworkSession/SessionPropertiesAndEventsScreen.hpp"
#include "Demos/Net/NetworkGamer/GamerRosterScreen.hpp"
#include "Demos/Net/NetworkGamer/NetworkMachineScreen.hpp"
#include "Demos/Net/GamerServices/SignedInGamersScreen.hpp"
#include "Demos/Net/GamerServices/ProfilePresencePrivilegesScreen.hpp"
#include "Demos/Net/GamerServices/AchievementsScreen.hpp"
#include "Demos/Net/GamerServices/FriendsAndGamerCardScreen.hpp"
#include "Demos/Net/GamerServices/GuideOverlayScreen.hpp"
#include "Demos/Net/Leaderboards/LeaderboardReaderScreen.hpp"
#include "Demos/Net/Leaderboards/LeaderboardWriterScreen.hpp"

namespace CnaExamples::Navigation {

using CnaExamples::GameStateManagement::GameScreen;

// A single runnable demonstration within a Category. `create` is a factory
// so building the catalog never constructs a screen (and its content) until
// the user actually navigates to it.
struct DemoEntry {
    std::string title;
    std::string description;
    std::function<std::shared_ptr<GameScreen>()> create;
};

// A group of related demos within an Area (e.g. Input's "Keyboard" category).
struct CategoryEntry {
    std::string title;
    std::vector<DemoEntry> demos;
};

// A top-level CNA subsystem shown on the Home screen (e.g. "Input").
struct AreaEntry {
    std::string title;
    std::vector<CategoryEntry> categories;
};

// DemoEntry helper: builds an entry whose factory default-constructs T.
template <typename T>
DemoEntry MakeDemo(std::string title, std::string description) {
    return DemoEntry{std::move(title), std::move(description),
                     [] { return std::make_shared<T>(); }};
}

inline std::vector<DemoEntry> BuildKeyboardDemos() {
    using namespace CnaExamples::Demos::Input::KeyboardDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LiveKeyStateScreen>(
        "Live Key State", "IsKeyDown() on a curated keyboard-shaped grid"));
    demos.push_back(MakeDemo<PressedKeysListScreen>(
        "Pressed Keys List", "GetPressedKeys() live listing"));
    demos.push_back(MakeDemo<KeyTransitionLogScreen>(
        "Press/Release Log", "Edge-detected down/up events over time"));
    demos.push_back(MakeDemo<ModifiersAndLocksScreen>(
        "Modifiers & Locks", "GetModStateEXT() -- Shift/Ctrl/Alt/Caps/Num/Scroll"));
    demos.push_back(MakeDemo<TextInputScreen>(
        "Text Input", "TextInputEXT -- composed, IME-aware character events"));
    demos.push_back(MakeDemo<ScancodeKeycodeNamesScreen>(
        "Scancode vs. Keycode", "Physical vs. layout-dependent key naming (EXT)"));
    demos.push_back(MakeDemo<StateEqualityHashScreen>(
        "State Equals()/GetHashCode()", "Snapshot comparison correctness"));
    demos.push_back(MakeDemo<PlayerIndexSlotsScreen>(
        "GetState(PlayerIndex)", "The 4-slot local-multiplayer keyboard API"));
    demos.push_back(MakeDemo<SingleKeyQueryScreen>(
        "IsKeyDown / operator[]", "The single-key query surface, side by side"));
    demos.push_back(MakeDemo<KeyHoldDurationScreen>(
        "Hold Duration & Repeat", "Building typematic repeat on IsKeyDown + GameTime"));
    return demos;
}

inline std::vector<DemoEntry> BuildMouseDemos() {
    using namespace CnaExamples::Demos::Input::MouseDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LivePositionButtonsScreen>(
        "Live Position & Buttons", "GetState() -- position and all five button states"));
    demos.push_back(MakeDemo<ScrollWheelScreen>(
        "Scroll Wheel", "Cumulative value + derived per-frame delta, vertical and horizontal"));
    demos.push_back(MakeDemo<SetPositionScreen>(
        "SetPosition()", "The write side of the Mouse API -- warping the cursor"));
    demos.push_back(MakeDemo<CursorShapesScreen>(
        "Cursor Shapes", "MouseCursor's stock shape gallery (EXT)"));
    demos.push_back(MakeDemo<ClickedEventScreen>(
        "ClickedEXT Event", "Event-driven clicks vs. polling ButtonState (EXT)"));
    demos.push_back(MakeDemo<RelativeModeScreen>(
        "Relative Mode", "Absolute position vs. per-frame motion delta (EXT)"));
    demos.push_back(MakeDemo<GlobalPositionWarpScreen>(
        "Global Position & Warp", "Desktop-space coordinates, independent of the window (EXT)"));
    demos.push_back(MakeDemo<CaptureScreen>(
        "Capture", "Keep receiving events once the cursor leaves the window (EXT)"));
    demos.push_back(MakeDemo<StateEqualityHashScreen>(
        "State Equals()/GetHashCode()", "Snapshot comparison correctness"));
    demos.push_back(MakeDemo<ButtonTransitionLogScreen>(
        "Button Press/Release Log", "Edge-detected transitions from polled ButtonState"));
    return demos;
}

inline std::vector<DemoEntry> BuildGamepadDemos() {
    using namespace CnaExamples::Demos::Input::GamepadDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ConnectionAndCapabilitiesScreen>(
        "Connection & Capabilities", "IsConnected + GetCapabilities() across all 4 player slots"));
    demos.push_back(MakeDemo<LiveButtonsGridScreen>(
        "Live Buttons Grid", "IsButtonDown() on every digital button"));
    demos.push_back(MakeDemo<DPadThumbsticksScreen>(
        "DPad & Thumbsticks", "The DPad and analog stick positions, live"));
    demos.push_back(MakeDemo<TriggersScreen>(
        "Triggers", "Analog trigger values + GamePad::TriggerThreshold"));
    demos.push_back(MakeDemo<DeadZoneModesScreen>(
        "Dead Zone Modes", "None vs. IndependentAxes vs. Circular, side by side"));
    demos.push_back(MakeDemo<VibrationScreen>(
        "Vibration", "SetVibration() + SetTriggerVibrationEXT()"));
    demos.push_back(MakeDemo<PowerInfoScreen>(
        "Power Info", "Battery/charge state (EXT)"));
    demos.push_back(MakeDemo<PlayerIndexEXTScreen>(
        "Player Index LED", "The controller's own player-number LED (EXT)"));
    demos.push_back(MakeDemo<DeviceInfoScreen>(
        "Device Info", "GUID/path/serial/firmware/Steam handle/connection/touchpads (EXT)"));
    demos.push_back(MakeDemo<StateEqualityHashScreen>(
        "State Equals()/GetHashCode()", "Snapshot comparison correctness"));
    return demos;
}

inline std::vector<DemoEntry> BuildTouchDemos() {
    using namespace CnaExamples::Demos::Input::TouchDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LiveTouchesScreen>(
        "Live Touches", "Raw per-finger state from GetState(), up to MAX_TOUCHES"));
    demos.push_back(MakeDemo<CapabilitiesAndDisplayScreen>(
        "Capabilities & Display", "GetCapabilities() + the display geometry gestures use"));
    demos.push_back(MakeDemo<TapAndDoubleTapScreen>(
        "Tap & DoubleTap", "The two simplest discrete gestures"));
    demos.push_back(MakeDemo<HoldAndFlickScreen>(
        "Hold & Flick", "A stationary hold vs. a fast released drag"));
    demos.push_back(MakeDemo<DragGesturesScreen>(
        "Drag Gestures", "Horizontal/Vertical/Free drag + DragComplete"));
    demos.push_back(MakeDemo<PinchGestureScreen>(
        "Pinch Gesture", "The only gesture using both fingers' Position2/Delta2"));
    demos.push_back(MakeDemo<AllGesturesLogScreen>(
        "All Gestures Log", "Every GestureType enabled at once"));
    demos.push_back(MakeDemo<GestureQueueScreen>(
        "Gesture Queue", "IsGestureAvailable()/ReadGesture()'s drain-the-queue contract"));
    demos.push_back(MakeDemo<TouchLocationQueryScreen>(
        "TouchLocation Equals()/ToString()", "Snapshot comparison correctness"));
    demos.push_back(MakeDemo<PeakTouchesScreen>(
        "Peak Simultaneous Touches", "TouchDeviceExistsEXT + a derived peak-touch-count metric"));
    return demos;
}

inline std::vector<DemoEntry> BuildOtherDemos() {
    using namespace CnaExamples::Demos::Input::OtherDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<JoystickEnumerationScreen>(
        "Joystick Enumeration", "Raw joystick/wheel/flight-stick enumeration + capabilities (EXT)"));
    demos.push_back(MakeDemo<JoystickLiveStateScreen>(
        "Joystick Live State", "Unmapped raw axes/buttons/hats/trackballs (EXT)"));
    demos.push_back(MakeDemo<JoystickStateEqualityScreen>(
        "Joystick State ==/!=", "Snapshot comparison correctness (EXT)"));
    demos.push_back(MakeDemo<JoystickHotplugScreen>(
        "Joystick Hot-plug Events", "Connected/DisconnectedEXT (EXT)"));
    demos.push_back(MakeDemo<SensorsScreen>(
        "Host Device Sensors", "The machine's own accelerometer/gyroscope (EXT)"));
    demos.push_back(MakeDemo<PowerScreen>(
        "Host Power/Battery", "XNA has no power API at all -- a pure CNA extension (EXT)"));
    demos.push_back(MakeDemo<HapticsScreen>(
        "Haptics", "Force-feedback device enumeration + capability checks (EXT)"));
    demos.push_back(MakeDemo<InputDeviceEnumerationScreen>(
        "Input Device Enumeration", "Every mouse/keyboard/touch device by id (EXT)"));
    demos.push_back(MakeDemo<InputDeviceHotplugScreen>(
        "Device Hot-plug Events", "Mouse/Keyboard Connected/DisconnectedEXT (EXT)"));
    demos.push_back(MakeDemo<OtherSummaryScreen>(
        "Summary", "An at-a-glance dashboard across every subsystem above"));
    return demos;
}

inline std::vector<DemoEntry> BuildSoundEffectDemos() {
    using namespace CnaExamples::Demos::Audio::SoundEffectDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<PlayToneScreen>(
        "Play a Tone", "SoundEffect::Play() -- a generated sine wave, no WAV asset needed"));
    demos.push_back(MakeDemo<MasterVolumeAndSettingsScreen>(
        "Static Settings", "MasterVolume/DistanceScale/DopplerScale/SpeedOfSound + byte<->duration math"));
    return demos;
}

inline std::vector<DemoEntry> BuildSoundEffectInstanceDemos() {
    using namespace CnaExamples::Demos::Audio::SoundEffectInstanceDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<PlaybackControlScreen>(
        "Playback Control", "Play/Pause/Resume/Stop + the State property"));
    demos.push_back(MakeDemo<VolumePitchPanScreen>(
        "Volume/Pitch/Pan", "Live-adjustable while a looping tone plays"));
    demos.push_back(MakeDemo<LoopingScreen>(
        "Looping", "IsLooped -- play-once vs. repeat-until-stopped"));
    return demos;
}

inline std::vector<DemoEntry> BuildAudio3DDemos() {
    using namespace CnaExamples::Demos::Audio::Audio3DDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<Apply3DScreen>(
        "Apply3D", "AudioListener + a movable AudioEmitter recompute Volume/Pan live"));
    demos.push_back(MakeDemo<DopplerDistanceScreen>(
        "Doppler & Distance Scale", "Emitter velocity -> a real closed-form Doppler pitch shift"));
    return demos;
}

inline std::vector<DemoEntry> BuildDynamicSoundEffectInstanceDemos() {
    using namespace CnaExamples::Demos::Audio::DynamicSoundEffectInstanceDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<StreamingSineWaveScreen>(
        "Streaming Sine Wave", "Real-time synthesis via BufferNeeded + SubmitBuffer()"));
    return demos;
}

inline std::vector<DemoEntry> BuildMicrophoneDemos() {
    using namespace CnaExamples::Demos::Audio::MicrophoneDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<MicrophoneEnumerationScreen>(
        "Enumeration", "Microphone::All / Default + per-device properties"));
    demos.push_back(MakeDemo<MicrophoneCaptureScreen>(
        "Capture", "Start/Stop + GetData() + the BufferReady event"));
    return demos;
}

inline std::vector<DemoEntry> BuildSensorsDemos() {
    using namespace CnaExamples::Demos::Devices::SensorsDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<AccelerometerScreen>(
        "Accelerometer", "Real XNA 4.0/WP7 API -- try this on Android"));
    demos.push_back(MakeDemo<GyroscopeScreen>(
        "Gyroscope", "Real XNA 4.0/WP7 API -- try this on Android"));
    demos.push_back(MakeDemo<CompassScreen>(
        "Compass", "Real XNA 4.0/WP7 API + the Calibrate event -- try this on Android"));
    demos.push_back(MakeDemo<MotionScreen>(
        "Motion (fused)", "Attitude + DeviceAcceleration + DeviceRotationRate + Gravity"));
    return demos;
}

inline std::vector<DemoEntry> BuildVibrationDemos() {
    using namespace CnaExamples::Demos::Devices::VibrationDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<VibrateScreen>(
        "VibrateController", "The device's vibration motor -- try this on Android"));
    return demos;
}

inline std::vector<DemoEntry> BuildCameraDemos() {
    using namespace CnaExamples::Demos::Devices::CameraDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<CameraCaptureScreen>(
        "Live Capture", "CNA::Devices::Camera -- works with a desktop webcam too"));
    return demos;
}

inline std::vector<DemoEntry> BuildSystemAndDisplayDemos() {
    using namespace CnaExamples::Demos::Devices::SystemAndDisplayDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<SystemInfoScreen>(
        "System Info", "CPU core count + system RAM"));
    demos.push_back(MakeDemo<DisplayInfoScreen>(
        "Display Info", "Content scale + safe area, beyond what GameWindow covers"));
    demos.push_back(MakeDemo<LocaleScreen>(
        "Locale", "The user's OS-level preferred language(s)/region(s)"));
    return demos;
}

inline std::vector<DemoEntry> BuildPowerDemos() {
    using namespace CnaExamples::Demos::Devices::PowerDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<PowerInfoScreen>(
        "PowerInfo", "Device battery/power status (CNA::Devices)"));
    return demos;
}

inline std::vector<DemoEntry> BuildDesktopIntegrationDemos() {
    using namespace CnaExamples::Demos::Devices::DesktopIntegrationDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ClipboardScreen>(
        "Clipboard", "Read/write the system clipboard's text"));
    demos.push_back(MakeDemo<MessageBoxScreen>(
        "MessageBox", "Native modal OS dialogs (blocks while open)"));
    demos.push_back(MakeDemo<FileDialogScreen>(
        "FileDialog", "Native async open-file/open-folder dialogs"));
    demos.push_back(MakeDemo<UrlLauncherScreen>(
        "UrlLauncher", "Opens a URL in the system's default handler"));
    demos.push_back(MakeDemo<SystemTrayScreen>(
        "SystemTray", "A real notification-area icon with a menu (desktop only)"));
    return demos;
}

inline std::vector<DemoEntry> BuildNetworkSessionDemos() {
    using namespace CnaExamples::Demos::Net::NetworkSessionDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LocalSessionLifecycleScreen>(
        "Local Lifecycle", "StartGame/EndGame/RemoveGamer -- NetworkSessionState transitions"));
    demos.push_back(MakeDemo<SystemLinkHostScreen>(
        "SystemLink Host", "A real UDP-backed session -- run a 2nd instance to join it"));
    demos.push_back(MakeDemo<DiscoverAndJoinScreen>(
        "Discover & Join", "Find() -- real LAN/loopback UDP discovery, then Join()"));
    demos.push_back(MakeDemo<PacketRoundTripScreen>(
        "Packet Round-Trip", "PacketWriter/PacketReader -- and a real Write(Color)/ReadColor() mismatch"));
    demos.push_back(MakeDemo<SessionPropertiesAndEventsScreen>(
        "Properties & Events", "NetworkSessionProperties + GamerJoined/GamerLeft/HostChanged"));
    return demos;
}

inline std::vector<DemoEntry> BuildNetworkGamerDemos() {
    using namespace CnaExamples::Demos::Net::NetworkGamerDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<GamerRosterScreen>(
        "Roster", "AllGamers/LocalGamers/RemoteGamers/PreviousGamers + per-gamer state"));
    demos.push_back(MakeDemo<NetworkMachineScreen>(
        "NetworkMachine", "Gamer::Machine -- RemoveFromSession() always throws (matches FNA)"));
    return demos;
}

inline std::vector<DemoEntry> BuildGamerServicesDemos() {
    using namespace CnaExamples::Demos::Net::GamerServicesDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<SignedInGamersScreen>(
        "Signed-In Gamers", "The 4 stub SignedInGamers + the real SignedIn/SignedOut events"));
    demos.push_back(MakeDemo<ProfilePresencePrivilegesScreen>(
        "Profile/Presence/Privileges", "GetProfile() + live-adjustable Presence + Privileges"));
    demos.push_back(MakeDemo<AchievementsScreen>(
        "Achievements", "AwardAchievement/GetAchievements + Achievement::GetPicture() always throws"));
    demos.push_back(MakeDemo<FriendsAndGamerCardScreen>(
        "Friends & GamerCard", "GetFriends() (always empty) + a manually-built demo roster"));
    demos.push_back(MakeDemo<GuideOverlayScreen>(
        "Guide Overlay", "The static Guide surface -- fake-async, no-op, or throws, by member"));
    return demos;
}

inline std::vector<DemoEntry> BuildLeaderboardsDemos() {
    using namespace CnaExamples::Demos::Net::LeaderboardsDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LeaderboardReaderScreen>(
        "LeaderboardReader", "LeaderboardIdentity + Read() -- always throws (stub API surface)"));
    demos.push_back(MakeDemo<LeaderboardWriterScreen>(
        "LeaderboardWriter", "LeaderboardEntry (real value type) + GetLeaderboard() -- always throws"));
    return demos;
}

// Builds the full Home -> Area -> Category -> Demo data set. This is the
// single place new areas/categories/demos get registered as they are
// implemented; see plan.md section 8 for what is intentionally still empty.
inline std::vector<AreaEntry> BuildAreaCatalog() {
    return {
        AreaEntry{"Input", {
            CategoryEntry{"Keyboard", BuildKeyboardDemos()},
            CategoryEntry{"Mouse", BuildMouseDemos()},
            CategoryEntry{"Gamepad", BuildGamepadDemos()},
            CategoryEntry{"Touch", BuildTouchDemos()},
            CategoryEntry{"Other", BuildOtherDemos()},
        }},
        AreaEntry{"Audio", {
            CategoryEntry{"SoundEffect", BuildSoundEffectDemos()},
            CategoryEntry{"SoundEffectInstance", BuildSoundEffectInstanceDemos()},
            CategoryEntry{"3D Audio", BuildAudio3DDemos()},
            CategoryEntry{"DynamicSoundEffectInstance", BuildDynamicSoundEffectInstanceDemos()},
            CategoryEntry{"Microphone", BuildMicrophoneDemos()},
        }},
        AreaEntry{"Devices", {
            CategoryEntry{"Sensors", BuildSensorsDemos()},
            CategoryEntry{"Vibration", BuildVibrationDemos()},
            CategoryEntry{"Camera", BuildCameraDemos()},
            CategoryEntry{"System & Display", BuildSystemAndDisplayDemos()},
            CategoryEntry{"Power", BuildPowerDemos()},
            CategoryEntry{"Desktop Integration", BuildDesktopIntegrationDemos()},
        }},
        AreaEntry{"Net", {
            CategoryEntry{"NetworkSession", BuildNetworkSessionDemos()},
            CategoryEntry{"NetworkGamer", BuildNetworkGamerDemos()},
            CategoryEntry{"GamerServices", BuildGamerServicesDemos()},
            CategoryEntry{"Leaderboards", BuildLeaderboardsDemos()},
        }},
        AreaEntry{"Media", {}},
        AreaEntry{"2D Graphics", {}},
        AreaEntry{"3D Graphics", {}},
    };
}

} // namespace CnaExamples::Navigation

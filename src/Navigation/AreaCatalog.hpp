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
#include "Demos/Input/Gamepad/MotionAndLightBarScreen.hpp"
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
#include "Demos/Input/Other/SentinelsAndFactoriesScreen.hpp"

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
#include "Demos/Audio/Xact/XactEngineScreen.hpp"
#include "Demos/Audio/Xact/XactCueScreen.hpp"

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
#include "Demos/Net/NetworkSession/SimulatedConditionsScreen.hpp"
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

#include "Demos/Media/Song/LoadAndPlayScreen.hpp"
#include "Demos/Media/Song/VolumeMuteRepeatShuffleScreen.hpp"
#include "Demos/Media/Song/QueueNavigationScreen.hpp"
#include "Demos/Media/Song/EventsScreen.hpp"
#include "Demos/Media/Song/UnsupportedFormatScreen.hpp"
#include "Demos/Media/Song/VisualizationScreen.hpp"
#include "Demos/Media/Video/LoadAndPlayScreen.hpp"
#include "Demos/Media/Video/PlaybackControlScreen.hpp"
#include "Demos/Media/Video/MultiTrackEXTScreen.hpp"
#include "Demos/Media/MediaLibrary/CatalogAccessScreen.hpp"
#include "Demos/Media/MediaLibrary/SongMetadataScreen.hpp"
#include "Demos/Media/MediaLibrary/AlbumArtistGenreScreen.hpp"
#include "Demos/Media/MediaLibrary/PlaylistScreen.hpp"
#include "Demos/Media/Pictures/PictureBrowserScreen.hpp"
#include "Demos/Media/Pictures/PictureAlbumTreeScreen.hpp"
#include "Demos/Media/Pictures/SavePictureScreen.hpp"
#include "Demos/Media/Pictures/PictureTokenScreen.hpp"
#include "Demos/Framework/GameLoop/FixedVsVariableTimeStepScreen.hpp"
#include "Demos/Framework/GameLoop/TargetElapsedTimeScreen.hpp"
#include "Demos/Framework/GameLoop/IsRunningSlowlyScreen.hpp"
#include "Demos/Framework/GameLoop/SuppressDrawScreen.hpp"
#include "Demos/Framework/GameComponents/ComponentLifecycleScreen.hpp"
#include "Demos/Framework/GameComponents/UpdateDrawOrderScreen.hpp"
#include "Demos/Framework/GameComponents/CollectionEventsScreen.hpp"
#include "Demos/Framework/GameComponents/EnabledVisibleScreen.hpp"
#include "Demos/Framework/Services/GameServiceContainerScreen.hpp"
#include "Demos/Framework/Services/FrameworkDispatcherScreen.hpp"
#include "Demos/Framework/Services/LaunchParametersScreen.hpp"
#include "Demos/Framework/Window/TitleAndClientBoundsScreen.hpp"
#include "Demos/Framework/Window/ClientSizeChangedScreen.hpp"
#include "Demos/Framework/Window/OrientationScreen.hpp"
#include "Demos/Framework/DeviceManager/ResolutionAndFullScreenScreen.hpp"
#include "Demos/Framework/DeviceManager/VSyncAndMultiSamplingScreen.hpp"
#include "Demos/Framework/DeviceManager/DeviceEventsScreen.hpp"
#include "Demos/Math/Vectors/Vector2OperationsScreen.hpp"
#include "Demos/Math/Vectors/Vector3OperationsScreen.hpp"
#include "Demos/Math/Vectors/InterpolationScreen.hpp"
#include "Demos/Math/Vectors/TransformAndDistanceScreen.hpp"
#include "Demos/Math/Vectors/MathHelperScreen.hpp"
#include "Demos/Math/MatrixQuaternion/MatrixCompositionScreen.hpp"
#include "Demos/Math/MatrixQuaternion/DecomposeAndInvertScreen.hpp"
#include "Demos/Math/MatrixQuaternion/QuaternionRotationScreen.hpp"
#include "Demos/Math/MatrixQuaternion/SlerpVsLerpScreen.hpp"
#include "Demos/Math/Geometry/BoundingVolumesScreen.hpp"
#include "Demos/Math/Geometry/RayIntersectionScreen.hpp"
#include "Demos/Math/Geometry/FrustumCullingScreen.hpp"
#include "Demos/Math/Curves/CurveTangentsScreen.hpp"
#include "Demos/Math/Curves/CurveLoopTypeScreen.hpp"
#include "Demos/Math/Color/ColorConversionsScreen.hpp"
#include "Demos/Math/Color/PackedVectorGalleryScreen.hpp"
#include "Demos/Content/Basics/LoadAndCacheScreen.hpp"
#include "Demos/Content/Basics/ResolutionOrderScreen.hpp"
#include "Demos/Content/Manifest/ContentManifestScreen.hpp"
#include "Demos/Content/Xnb/XnbFixturesScreen.hpp"
#include "Demos/Content/Errors/ContentLoadExceptionScreen.hpp"
#include "Demos/Content/Cnj/CnjEnvelopeScreen.hpp"
#include "Demos/Content/Cnj/CustomCnjLoaderScreen.hpp"
#include "Demos/Storage/Device/StorageDeviceScreen.hpp"
#include "Demos/Storage/Container/SaveGameRoundTripScreen.hpp"
#include "Demos/Diagnostics/Logging/LoggerScreen.hpp"
#include "Demos/Diagnostics/Platform/PlatformInfoScreen.hpp"
#include "Demos/Diagnostics/Capabilities/GraphicsCapabilityScreen.hpp"
#include "Demos/Diagnostics/Adapter/GraphicsAdapterScreen.hpp"

#include "Demos/Graphics2D/DrawingBasics/PositionDrawScreen.hpp"
#include "Demos/Graphics2D/DrawingBasics/DestinationRectangleScreen.hpp"
#include "Demos/Graphics2D/DrawingBasics/SourceRectangleScreen.hpp"
#include "Demos/Graphics2D/DrawingBasics/RotationOriginScreen.hpp"
#include "Demos/Graphics2D/DrawingBasics/ScaleAndEffectsScreen.hpp"
#include "Demos/Graphics2D/SortModes/DeferredSortScreen.hpp"
#include "Demos/Graphics2D/SortModes/ImmediateSortScreen.hpp"
#include "Demos/Graphics2D/SortModes/TextureSortScreen.hpp"
#include "Demos/Graphics2D/SortModes/BackToFrontSortScreen.hpp"
#include "Demos/Graphics2D/SortModes/FrontToBackSortScreen.hpp"
#include "Demos/Graphics2D/DrawString/BasicTextScreen.hpp"
#include "Demos/Graphics2D/DrawString/TransformedTextScreen.hpp"
#include "Demos/Graphics2D/DrawString/FlippedTextScreen.hpp"
#include "Demos/Graphics2D/DrawString/StringBuilderScreen.hpp"
#include "Demos/Graphics2D/BeginEndState/NestedBeginThrowsScreen.hpp"
#include "Demos/Graphics2D/BeginEndState/DrawOutsideBeginThrowsScreen.hpp"
#include "Demos/Graphics2D/BeginEndState/BlendStateLeakScreen.hpp"
#include "Demos/Graphics2D/BeginEndState/TransformMatrixScreen.hpp"
#include "Demos/Graphics2D/Texture2DBasics/ProceduralCreationScreen.hpp"
#include "Demos/Graphics2D/Texture2DBasics/SetDataGetDataRoundTripScreen.hpp"
#include "Demos/Graphics2D/Texture2DBasics/SurfaceFormatMatrixScreen.hpp"
#include "Demos/Graphics2D/Texture2DBasics/PropertiesAndDisposeScreen.hpp"
#include "Demos/Graphics2D/SaveAsReload/SaveAsPngRoundTripScreen.hpp"
#include "Demos/Graphics2D/SaveAsReload/SaveAsJpegRoundTripScreen.hpp"
#include "Demos/Graphics2D/SpriteFont/MeasureStringScreen.hpp"
#include "Demos/Graphics2D/SpriteFont/DefaultCharacterFallbackScreen.hpp"
#include "Demos/Graphics2D/SpriteFont/LineSpacingAndSpacingScreen.hpp"
#include "Demos/Graphics2D/SpriteFont/HandBuiltFontScreen.hpp"
#include "Demos/Graphics2D/BlendState/BlendModeComparisonScreen.hpp"
#include "Demos/Graphics2D/BlendState/PremultipliedAlphaGotchaScreen.hpp"
#include "Demos/Graphics2D/SamplerState/FilterComparisonScreen.hpp"
#include "Demos/Graphics2D/SamplerState/AddressModeScreen.hpp"
#include "Demos/Graphics2D/ViewportScissor/ViewportInspectorScreen.hpp"
#include "Demos/Graphics2D/ViewportScissor/ScissorClippingScreen.hpp"
#include "Demos/Graphics2D/ViewportScissor/ViewportScissorResetGotchaScreen.hpp"
#include "Demos/Graphics2D/RenderToTextureBasics/DrawIntoRenderTargetScreen.hpp"
#include "Demos/Graphics2D/RenderToTextureBasics/MultiTargetRoundTripScreen.hpp"
#include "Demos/Graphics2D/ScreenTransition/FadeTransitionScreen.hpp"
#include "Demos/Graphics2D/DisposeSafety/DisposeWhileBoundThrowsScreen.hpp"

#include "Demos/Graphics3D/VertexTypes/PositionColorScreen.hpp"
#include "Demos/Graphics3D/VertexTypes/PositionTextureScreen.hpp"
#include "Demos/Graphics3D/VertexTypes/PositionNormalTextureScreen.hpp"
#include "Demos/Graphics3D/PrimitiveTypes/TriangleListVsStripScreen.hpp"
#include "Demos/Graphics3D/PrimitiveTypes/LineAndPointScreen.hpp"
#include "Demos/Graphics3D/Buffers/ImmediateVsBufferedScreen.hpp"
#include "Demos/Graphics3D/Buffers/DynamicVertexBufferScreen.hpp"
#include "Demos/Graphics3D/Buffers/VertexDeclarationGotchaScreen.hpp"
#include "Demos/Graphics3D/BasicRendering/VertexColorCubeScreen.hpp"
#include "Demos/Graphics3D/BasicRendering/TexturedCubeScreen.hpp"
#include "Demos/Graphics3D/BasicRendering/AlphaBlendCubeScreen.hpp"
#include "Demos/Graphics3D/Lighting/DefaultLightingScreen.hpp"
#include "Demos/Graphics3D/Lighting/DirectionalLightsToggleScreen.hpp"
#include "Demos/Graphics3D/Lighting/AmbientAndSpecularScreen.hpp"
#include "Demos/Graphics3D/Fog/DistanceFogScreen.hpp"
#include "Demos/Graphics3D/AlphaTestEffect/AlphaCutoffScreen.hpp"
#include "Demos/Graphics3D/AlphaTestEffect/AlphaFunctionScreen.hpp"
#include "Demos/Graphics3D/DualTextureEffect/DualTextureBlendScreen.hpp"
#include "Demos/Graphics3D/EnvironmentMapEffect/ReflectiveCubeScreen.hpp"
#include "Demos/Graphics3D/EnvironmentMapEffect/EnvironmentMapTuningScreen.hpp"
#include "Demos/Graphics3D/SkinnedEffect/TwoBoneSkinningScreen.hpp"
#include "Demos/Graphics3D/CustomShader/CustomGlslShaderScreen.hpp"
#include "Demos/Graphics3D/CustomShader/InvalidShaderScreen.hpp"
#include "Demos/Graphics3D/DepthAndCulling/DepthStencilComparisonScreen.hpp"
#include "Demos/Graphics3D/DepthAndCulling/CullModeScreen.hpp"
#include "Demos/Graphics3D/DepthAndCulling/FillModeWireframeScreen.hpp"
#include "Demos/Graphics3D/CameraAndProjection/OrbitingCameraScreen.hpp"
#include "Demos/Graphics3D/CameraAndProjection/PerspectiveVsOrthographicScreen.hpp"
#include "Demos/Graphics3D/ModelGroup/ProceduralModelScreen.hpp"
#include "Demos/Graphics3D/ModelGroup/ModelBoneHierarchyScreen.hpp"
#include "Demos/Graphics3D/TexturesAndQueries/Texture3DVolumeScreen.hpp"
#include "Demos/Graphics3D/TexturesAndQueries/TextureCubeFacesScreen.hpp"
#include "Demos/Graphics3D/EffectReflection/ReflectionSurfaceScreen.hpp"
#include "Demos/Graphics3D/EffectReflection/EffectParameterScreen.hpp"
#include "Demos/Graphics3D/EffectReflection/EffectCloneScreen.hpp"
#include "Demos/Graphics3D/TexturesAndQueries/RenderTargetCubeScreen.hpp"
#include "Demos/Graphics3D/TexturesAndQueries/OcclusionQueryScreen.hpp"

namespace CnaExamples::Navigation {

using CnaExamples::GameStateManagement::GameScreen;

// A single runnable demonstration within a Category. `create` is a factory
// so building the catalog never constructs a screen (and its content) until
// the user actually navigates to it.
struct DemoEntry {
    std::string title;
    std::string description;

    // The CNA/XNA symbols this demo actually exercises, e.g.
    // {"MediaLibrary::Songs", "Song::Album"}. One list drives three things --
    // the footer line on the demo screen, the search index (SearchScreen), and
    // the coverage question "which API does this demo prove works?" -- so it is
    // answered in one place instead of three.
    //
    // Populated per area as areas are built or revisited; older entries that
    // predate the field simply have none yet.
    std::vector<std::string> apis;

    std::function<std::shared_ptr<GameScreen>()> create;
};

// A group of related demos within an Area (e.g. Input's "Keyboard" category).
struct CategoryEntry {
    std::string title;
    std::vector<DemoEntry> demos;
};

// A group of related Categories within an Area that has enough of them to
// warrant an extra navigation level (e.g. 2D Graphics -> "SpriteBatch" group
// -> {Drawing Basics, Sort Modes, ...} categories -> demos). Areas that
// don't need this (Input, Audio, Devices, Net, Media) leave
// AreaEntry::groups empty and populate AreaEntry::categories directly
// instead; AreaScreen picks whichever one is non-empty -- see its own
// comment for the exact rule.
struct GroupEntry {
    std::string title;
    std::vector<CategoryEntry> categories;
};

// A top-level CNA subsystem shown on the Home screen (e.g. "Input").
// Exactly one of `groups`/`categories` should be populated per Area (not
// both) -- see GroupEntry's doc comment.
struct AreaEntry {
    std::string title;
    std::vector<CategoryEntry> categories;
    std::vector<GroupEntry> groups;
};

// DemoEntry helper: builds an entry whose factory default-constructs T.
//
// The factory also hands the entry's `apis` list and breadcrumb to the screen
// it creates, so a DemoScreen can render both without having to know how it was
// reached. T must derive from DemoScreen for that to compile, which is exactly
// the constraint we want on a demo registration.
template <typename T>
DemoEntry MakeDemo(std::string title, std::string description,
                   std::vector<std::string> apis = {}) {
    DemoEntry entry;
    entry.title = std::move(title);
    entry.description = std::move(description);
    entry.apis = std::move(apis);

    std::vector<std::string> apisForFactory = entry.apis;
    entry.create = [apisForFactory] {
        auto screen = std::make_shared<T>();
        screen->SetApis(apisForFactory);
        return screen;
    };
    return entry;
}

// Marks every demo in a list as needing a graphics capability, so a backend
// that lacks it shows an explanation instead of running code that throws.
//
// Applied per category at the assembly site rather than inside each screen: CNA
// picks its backend at compile time, whole categories share a requirement, and
// this way a new demo dropped into a gated category inherits the gate instead
// of having to remember it.
inline std::vector<DemoEntry> Requiring(CNA::GraphicsCapability capability,
                                        std::vector<DemoEntry> demos) {
    for (auto& demo : demos) {
        auto inner = demo.create;
        demo.create = [inner, capability] {
            auto screen = inner();
            if (auto* demoScreen = dynamic_cast<Demos::DemoScreen*>(screen.get())) {
                demoScreen->SetRequiredCapability(capability);
            }
            return screen;
        };
    }
    return demos;
}

// Total demos underneath a Category / Group / Area, for the "(N)" counts shown
// on the menus above them.
inline int CountDemos(const CategoryEntry& category) {
    return (int)category.demos.size();
}

inline int CountDemos(const GroupEntry& group) {
    int total = 0;
    for (const auto& category : group.categories) total += CountDemos(category);
    return total;
}

inline int CountDemos(const AreaEntry& area) {
    int total = 0;
    for (const auto& category : area.categories) total += CountDemos(category);
    for (const auto& group : area.groups) total += CountDemos(group);
    return total;
}

// "Title  (12)" -- the label a parent menu shows for a child that contains
// demos. Kept here so Home/Area/Group screens format it identically.
inline std::string WithCount(const std::string& title, int count) {
    return title + "   (" + std::to_string(count) + ")";
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
    demos.push_back(MakeDemo<MotionAndLightBarScreen>(
        "Motion Sensors & Light Bar",
        "GetGyroEXT/GetAccelerometerEXT return bool; SetLightBarEXT reports nothing",
        {"GamePad"}));
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
    demos.push_back(MakeDemo<SentinelsAndFactoriesScreen>(
        "Sentinels & Factories",
        "FindById's Invalid sentinel, TouchPanel::NO_FINGER, GamePadButtons::FromButtonArray",
        {"TouchCollection", "TouchPanel", "GamePadButtons"}));
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

inline std::vector<DemoEntry> BuildXactDemos() {
    using namespace CnaExamples::Demos::Audio::XactDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<XactEngineScreen>(
        "Engine & Banks", "AudioEngine + WaveBank + SoundBank, and the order they must open in",
        {"AudioEngine", "WaveBank", "SoundBank", "AudioEngine::Update"}));
    demos.push_back(MakeDemo<XactCueScreen>(
        "Cues & Categories", "A Cue's full seven-flag state machine, and category-wide volume",
        {"SoundBank::GetCue", "Cue::Play", "AudioCategory::SetVolume",
         "AudioEngine::GetGlobalVariable"}));
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
    demos.push_back(MakeDemo<SimulatedConditionsScreen>(
        "Simulated Latency & Packet Loss",
        "Inert auto-properties in FNA; really implemented here, and measured",
        {"NetworkSession"}));
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

inline std::vector<DemoEntry> BuildSongDemos() {
    using namespace CnaExamples::Demos::Media::SongDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LoadAndPlayScreen>(
        "Load & Play", "Song's direct-from-file NOXNA constructor + transport controls",
        {"Song", "MediaPlayer::Play", "MediaPlayer::PlayPosition"}));
    demos.push_back(MakeDemo<VolumeMuteRepeatShuffleScreen>(
        "Volume/Mute/Repeat/Shuffle", "MediaPlayer's live-adjustable playback settings",
        {"MediaPlayer::Volume", "MediaPlayer::IsMuted", "MediaPlayer::IsRepeating",
         "MediaPlayer::IsShuffled"}));
    demos.push_back(MakeDemo<QueueNavigationScreen>(
        "Queue Navigation", "Play(SongCollection, index) + MoveNext()/MovePrevious()",
        {"SongCollection", "MediaQueue", "MediaPlayer::MoveNext"}));
    demos.push_back(MakeDemo<EventsScreen>(
        "MediaPlayer Events", "ActiveSongChanged/MediaStateChanged, pumped via FrameworkDispatcher",
        {"MediaPlayer::ActiveSongChanged", "MediaPlayer::MediaStateChanged",
         "FrameworkDispatcher"}));
    demos.push_back(MakeDemo<UnsupportedFormatScreen>(
        "Unsupported Format", "A real .opus file -- constructs fine, Play() silently no-ops",
        {"Song", "MediaPlayer::State"}));
    demos.push_back(MakeDemo<VisualizationScreen>(
        "Visualization", "Live FFT spectrum + waveform from the real post-mix tap",
        {"MediaPlayer::IsVisualizationEnabled", "MediaPlayer::GetVisualizationData",
         "VisualizationData"}));
    return demos;
}

inline std::vector<DemoEntry> BuildVideoDemos() {
    using namespace CnaExamples::Demos::Media::VideoDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LoadAndPlayScreen>(
        "Load & Play", "A real FFmpeg-decoded clip, live GetTexture() every frame",
        {"Video", "VideoPlayer::Play", "VideoPlayer::GetTexture"}));
    demos.push_back(MakeDemo<PlaybackControlScreen>(
        "Playback Control", "Play/Pause/Resume/Stop + IsLooped + live Volume/IsMuted",
        {"VideoPlayer::Pause", "VideoPlayer::IsLooped", "VideoPlayer::Volume"}));
    demos.push_back(MakeDemo<MultiTrackEXTScreen>(
        "Multi-Track (EXT)", "SetAudioTrackEXT()/SetVideoTrackEXT() -- CNA extensions",
        {"VideoPlayer::SetAudioTrackEXT", "VideoPlayer::SetVideoTrackEXT"}));
    return demos;
}

inline std::vector<DemoEntry> BuildMediaLibraryDemos() {
    using namespace CnaExamples::Demos::Media::MediaLibraryDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<CatalogAccessScreen>(
        "Catalog Access", "Index the bundled demo library or the real OS folders, live",
        {"MediaLibrary", "MediaSource", "MediaLibrary::IsDisposed"}));
    demos.push_back(MakeDemo<SongMetadataScreen>(
        "Song Metadata", "Name/Artist/Album/Genre/Duration/TrackNumber/Rating from real tags",
        {"MediaLibrary::Songs", "Song::Album", "Song::Artist", "Song::TrackNumber"}));
    demos.push_back(MakeDemo<AlbumArtistGenreScreen>(
        "Album/Artist/Genre", "The three grouping views derived from the same song tags",
        {"AlbumCollection", "ArtistCollection", "GenreCollection", "Album::HasArt"}));
    demos.push_back(MakeDemo<PlaylistScreen>(
        "Playlists", "Real .m3u parsing, entry order preserved, playable via MediaPlayer",
        {"PlaylistCollection", "Playlist::Songs", "Playlist::Duration"}));
    return demos;
}

inline std::vector<DemoEntry> BuildPictureDemos() {
    using namespace CnaExamples::Demos::Media::PictureDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<PictureBrowserScreen>(
        "Browse", "Picture metadata next to the decoded image it describes",
        {"MediaLibrary::Pictures", "Picture::Width", "Texture2D::FromStream"}));
    demos.push_back(MakeDemo<PictureAlbumTreeScreen>(
        "Album Tree", "Walk RootPictureAlbum -> Albums -> Pictures, with Parent back-refs",
        {"MediaLibrary::RootPictureAlbum", "PictureAlbum::Albums", "PictureAlbum::Parent"}));
    demos.push_back(MakeDemo<SavePictureScreen>(
        "SavePicture", "Write a generated BMP into the library; lazy \"Saved Pictures\" creation",
        {"MediaLibrary::SavePicture", "MediaLibrary::SavedPictures"}));
    demos.push_back(MakeDemo<PictureTokenScreen>(
        "Tokens & Identity", "GetPictureFromToken round trip vs Equals/GetHashCode",
        {"MediaLibrary::GetPictureFromToken", "Picture::Equals", "Picture::Date"}));
    return demos;
}

inline std::vector<DemoEntry> BuildGameLoopDemos() {
    using namespace CnaExamples::Demos::Framework::GameLoopDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<FixedVsVariableTimeStepScreen>(
        "Fixed vs Variable Time Step", "Toggle IsFixedTimeStep and watch the frame-delta spread",
        {"Game::IsFixedTimeStep", "GameTime::ElapsedGameTime"}));
    demos.push_back(MakeDemo<TargetElapsedTimeScreen>(
        "TargetElapsedTime", "60/30/10 Hz ticks; TotalGameTime keeps real-world speed",
        {"Game::TargetElapsedTime", "GameTime::TotalGameTime"}));
    demos.push_back(MakeDemo<IsRunningSlowlyScreen>(
        "IsRunningSlowly", "Burn real CPU in Update until the loop falls behind",
        {"GameTime::IsRunningSlowly"}));
    demos.push_back(MakeDemo<SuppressDrawScreen>(
        "SuppressDraw & ResetElapsedTime", "Update keeps counting while Draw is skipped",
        {"Game::SuppressDraw", "Game::ResetElapsedTime"}));
    return demos;
}

inline std::vector<DemoEntry> BuildGameComponentsDemos() {
    using namespace CnaExamples::Demos::Framework::GameComponentsDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ComponentLifecycleScreen>(
        "Lifecycle", "Add/remove a live GameComponent; when Initialize actually runs",
        {"GameComponent", "Game::Components", "GameComponentCollection::Contains"}));
    demos.push_back(MakeDemo<UpdateDrawOrderScreen>(
        "Update/Draw Order", "Swap DrawOrder and read the real draw sequence back",
        {"DrawableGameComponent::DrawOrder", "GameComponent::UpdateOrder"}));
    demos.push_back(MakeDemo<CollectionEventsScreen>(
        "Collection Events", "ComponentAdded/ComponentRemoved, logged by the handlers",
        {"GameComponentCollection::ComponentAdded", "GameComponentCollection::ComponentRemoved"}));
    demos.push_back(MakeDemo<EnabledVisibleScreen>(
        "Enabled & Visible", "Enabled gates Update, Visible gates Draw -- counters diverge",
        {"GameComponent::Enabled", "DrawableGameComponent::Visible", "EnabledChanged"}));
    return demos;
}

inline std::vector<DemoEntry> BuildServicesDemos() {
    using namespace CnaExamples::Demos::Framework::ServicesDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<GameServiceContainerScreen>(
        "GameServiceContainer", "Register/resolve your own service; the built-in device service",
        {"Game::Services", "GameServiceContainer::AddService", "IGraphicsDeviceService"}));
    demos.push_back(MakeDemo<FrameworkDispatcherScreen>(
        "FrameworkDispatcher", "The pump Game::Update calls for you, and why it matters",
        {"FrameworkDispatcher::Update"}));
    demos.push_back(MakeDemo<LaunchParametersScreen>(
        "LaunchParameters", "This process's own command line, parsed into key/value pairs",
        {"Game::LaunchParameters", "LaunchParameters::ContainsKey"}));
    return demos;
}

inline std::vector<DemoEntry> BuildWindowDemos() {
    using namespace CnaExamples::Demos::Framework::WindowDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<TitleAndClientBoundsScreen>(
        "Title & ClientBounds", "Change the real OS window title; ClientBounds vs viewport",
        {"GameWindow::Title", "GameWindow::ClientBounds", "GameWindow::ScreenDeviceName"}));
    demos.push_back(MakeDemo<ClientSizeChangedScreen>(
        "ClientSizeChanged", "Resize the window and watch the event log fill in",
        {"GameWindow::ClientSizeChanged", "GameWindow::AllowUserResizing"}));
    demos.push_back(MakeDemo<OrientationScreen>(
        "Display Orientation", "CurrentOrientation, derived from the window's aspect on desktop",
        {"GameWindow::CurrentOrientation", "DisplayOrientation", "SupportedOrientations"}));
    return demos;
}

inline std::vector<DemoEntry> BuildDeviceManagerDemos() {
    using namespace CnaExamples::Demos::Framework::DeviceManagerDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ResolutionAndFullScreenScreen>(
        "Resolution & FullScreen", "Preferred* is a request; nothing happens until ApplyChanges()",
        {"GraphicsDeviceManager::PreferredBackBufferWidth", "ApplyChanges", "IsFullScreen"}));
    demos.push_back(MakeDemo<VSyncAndMultiSamplingScreen>(
        "VSync & MultiSampling", "Measure the frame rate with v-sync on and off",
        {"SynchronizeWithVerticalRetrace", "PreferMultiSampling", "GraphicsCapability"}));
    demos.push_back(MakeDemo<DeviceEventsScreen>(
        "Device Events", "Force a real device reset and log all five manager events",
        {"PreparingDeviceSettings", "DeviceReset", "DeviceResetting", "DeviceCreated"}));
    return demos;
}

inline std::vector<DemoEntry> BuildMathVectorsDemos() {
    using namespace CnaExamples::Demos::Math::VectorsDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<Vector2OperationsScreen>(
        "Vector2 Operations", "Add/Dot/Normalize/Reflect, drawn as well as printed",
        {"Vector2::Dot", "Vector2::Normalize", "Vector2::Reflect", "Vector2::Length"}));
    demos.push_back(MakeDemo<Vector3OperationsScreen>(
        "Vector3 Cross, Dot & Reflect", "Cross's handedness, computed live rather than claimed",
        {"Vector3::Cross", "Vector3::Dot", "Vector3::Reflect"}));
    demos.push_back(MakeDemo<InterpolationScreen>(
        "Interpolation", "Lerp/SmoothStep/CatmullRom/Hermite plotted against t",
        {"MathHelper::Lerp", "MathHelper::SmoothStep", "MathHelper::CatmullRom",
         "MathHelper::Hermite", "MathHelper::Barycentric"}));
    demos.push_back(MakeDemo<TransformAndDistanceScreen>(
        "Transform & Distance", "Transform vs TransformNormal -- the bug that hides at the origin",
        {"Vector3::Transform", "Vector3::TransformNormal", "Vector2::DistanceSquared"}));
    demos.push_back(MakeDemo<MathHelperScreen>(
        "MathHelper", "WrapAngle, Clamp, and why 0.1f + 0.2f != 0.3f",
        {"MathHelper::WrapAngle", "MathHelper::Clamp", "MathHelper::WithinEpsilon"}));
    return demos;
}

inline std::vector<DemoEntry> BuildMatrixQuaternionDemos() {
    using namespace CnaExamples::Demos::Math::MatrixQuaternionDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<MatrixCompositionScreen>(
        "Composition Order", "R*T and T*R from the same operands, drawn side by side",
        {"Matrix::CreateRotationZ", "Matrix::CreateTranslation", "Vector3::Transform"}));
    demos.push_back(MakeDemo<DecomposeAndInvertScreen>(
        "Decompose, Invert & Determinant", "Including a shear that cannot be decomposed at all",
        {"Matrix::Decompose", "Matrix::Invert", "Matrix::Determinant", "Matrix::Transpose"}));
    demos.push_back(MakeDemo<QuaternionRotationScreen>(
        "Rotation Basics", "Unit length, matrix round trip, Conjugate == Inverse",
        {"Quaternion::CreateFromAxisAngle", "Quaternion::Conjugate", "Quaternion::Inverse"}));
    demos.push_back(MakeDemo<SlerpVsLerpScreen>(
        "Slerp vs Lerp", "Measured angular velocity, constant for one and not the other",
        {"Quaternion::Slerp", "Quaternion::Lerp"}));
    return demos;
}

inline std::vector<DemoEntry> BuildGeometryDemos() {
    using namespace CnaExamples::Demos::Math::GeometryDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<BoundingVolumesScreen>(
        "Bounding Volumes", "Intersects (bool) vs Contains (ContainmentType), all three states",
        {"BoundingBox::Contains", "BoundingSphere::CreateFromPoints", "ContainmentType"}));
    demos.push_back(MakeDemo<RayIntersectionScreen>(
        "Ray Intersection", "Distance along the ray, and why the direction must be unit length",
        {"Ray::Intersects", "BoundingSphere", "BoundingBox", "Plane"}));
    demos.push_back(MakeDemo<FrustumCullingScreen>(
        "Frustum Culling", "An 11x11 grid culled against a rotating camera, live counts",
        {"BoundingFrustum", "BoundingFrustum::Contains", "BoundingFrustum::GetCorners"}));
    return demos;
}

inline std::vector<DemoEntry> BuildCurvesDemos() {
    using namespace CnaExamples::Demos::Math::CurvesDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<CurveTangentsScreen>(
        "Tangents", "Identical keys, three tangent rules, three very different shapes",
        {"Curve::ComputeTangents", "CurveTangent", "Curve::Evaluate", "CurveKey"}));
    demos.push_back(MakeDemo<CurveLoopTypeScreen>(
        "Pre/Post Loop", "What a Curve returns outside the range its keys cover",
        {"Curve::PreLoop", "Curve::PostLoop", "CurveLoopType"}));
    return demos;
}

inline std::vector<DemoEntry> BuildColorDemos() {
    using namespace CnaExamples::Demos::Math::ColorDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ColorConversionsScreen>(
        "Conversions", "Premultiplied vs straight alpha, and why the bug hides at alpha 1",
        {"Color::FromNonPremultiplied", "Color::ToVector4", "Color::PackedValue"}));
    demos.push_back(MakeDemo<PackedVectorGalleryScreen>(
        "PackedVector Gallery", "17 formats round-tripped; the error column is the whole story",
        {"IPackedVector", "Bgr565", "NormalizedByte4", "HalfVector4", "Rgba1010102"}));
    return demos;
}

inline std::vector<DemoEntry> BuildContentBasicsDemos() {
    using namespace CnaExamples::Demos::Content::BasicsDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LoadAndCacheScreen>(
        "Load & Cache", "Timed loads showing the cache, and why identity cannot be compared",
        {"ContentManager::Load", "ContentManager::Unload", "ContentManager::RootDirectory"}));
    demos.push_back(MakeDemo<ResolutionOrderScreen>(
        "Asset Name Resolution", "An asset name is not a filename: .xnb, literal, then .cnj",
        {"ContentManager::Load", "ContentManager::RootDirectory"}));
    return demos;
}

inline std::vector<DemoEntry> BuildContentManifestDemos() {
    using namespace CnaExamples::Demos::Content::ManifestDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ContentManifestScreen>(
        "Manifest", "Every discoverable asset, plus which .xnb readers this build has",
        {"ContentManager::GetContentManifest", "ContentManager::RefreshContentManifest",
         "ContentManifestEntry", "GetXnbReaderUsageSummary"}));
    return demos;
}

inline std::vector<DemoEntry> BuildContentXnbDemos() {
    using namespace CnaExamples::Demos::Content::XnbDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<XnbFixturesScreen>(
        "XNB Fixtures", "Real MonoGame-produced .xnb, including an LZX-compressed one",
        {"ContentManager::Load", "Texture2DReader", "LzxDecoder"}));
    return demos;
}

inline std::vector<DemoEntry> BuildContentErrorsDemos() {
    using namespace CnaExamples::Demos::Content::ErrorsDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ContentLoadExceptionScreen>(
        "Load Failures", "Missing, wrong-type and unsupported content, with real messages",
        {"ContentLoadException", "ContentManager::Load"}));
    return demos;
}

inline std::vector<DemoEntry> BuildStorageDeviceDemos() {
    using namespace CnaExamples::Demos::Storage::DeviceDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<StorageDeviceScreen>(
        "StorageDevice", "XNA's fake-async selector, per PlayerIndex, and where saves land",
        {"StorageDevice::BeginShowSelector", "StorageDevice::EndShowSelector",
         "StorageDevice::FreeSpace", "StorageDevice::GetStorageRootEXT"}));
    return demos;
}

inline std::vector<DemoEntry> BuildStorageContainerDemos() {
    using namespace CnaExamples::Demos::Storage::ContainerDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<SaveGameRoundTripScreen>(
        "Save Game Round Trip", "Write, read back and delete a save that survives a restart",
        {"StorageContainer::CreateFile", "StorageContainer::OpenFile",
         "StorageContainer::FileExists", "StorageContainer::DeleteFile"}));
    return demos;
}

inline std::vector<DemoEntry> BuildDiagnosticsLoggingDemos() {
    using namespace CnaExamples::Demos::Diagnostics::LoggingDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<LoggerScreen>(
        "Logger", "All seven levels emitted live, and what the filter lets through",
        {"CNA::Logger::Log", "CNA::Logger::SetMinimumLevel", "CNA::LogLevel", "CNA::LogCategory"}));
    return demos;
}

inline std::vector<DemoEntry> BuildDiagnosticsPlatformDemos() {
    using namespace CnaExamples::Demos::Diagnostics::PlatformDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<PlatformInfoScreen>(
        "Platform & Build", "Which platform, backend and optional subsystems this binary has",
        {"CNA::getCurrentPlatform", "CNA::getCurrentGraphicsBackendName", "CNA_DEVICES"}));
    return demos;
}

inline std::vector<DemoEntry> BuildDiagnosticsCapabilitiesDemos() {
    using namespace CnaExamples::Demos::Diagnostics::CapabilitiesDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<GraphicsCapabilityScreen>(
        "Graphics Capabilities", "All eight GraphicsCapability values, queried live",
        {"GraphicsDevice::SupportsCapability", "CNA::GraphicsCapability",
         "CNA::GraphicsBackendType"}));
    return demos;
}

inline std::vector<DemoEntry> BuildDiagnosticsAdapterDemos() {
    using namespace CnaExamples::Demos::Diagnostics::AdapterDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<GraphicsAdapterScreen>(
        "Adapter & Display Modes", "What could be asked for, next to what the device got",
        {"GraphicsAdapter::DefaultAdapter", "GraphicsAdapter::SupportedDisplayModes",
         "DisplayMode", "PresentationParameters"}));
    return demos;
}

inline std::vector<DemoEntry> BuildContentCnjDemos() {
    using namespace CnaExamples::Demos::Content::CnjDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<CnjEnvelopeScreen>(
        "Envelope", "cnjVersion/type/sourceFile, parsed live -- including an invalid one",
        {"CnjEnvelope", "ParseCnjEnvelope", "cnjVersion"}));
    demos.push_back(MakeDemo<CustomCnjLoaderScreen>(
        "Custom Loaders", "Two .cnj \"type\" names, one C++ struct, no reader class to write",
        {"ContentManager::RegisterCnjLoader", "CnjLoaderFn", "ContentManager::Load"}));
    return demos;
}

inline std::vector<DemoEntry> BuildDrawingBasicsDemos() {
    using namespace CnaExamples::Demos::Graphics2D::DrawingBasicsDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<PositionDrawScreen>(
        "Position Draw", "Draw(texture, Vector2 position, Color) + the NOXNA float x,y overload"));
    demos.push_back(MakeDemo<DestinationRectangleScreen>(
        "Destination Rectangle", "Stretching a texture to fill a Rectangle, 3 sizes side by side"));
    demos.push_back(MakeDemo<SourceRectangleScreen>(
        "Source Rectangle", "Cropping one cell out of a 4x4 sprite-sheet atlas"));
    demos.push_back(MakeDemo<RotationOriginScreen>(
        "Rotation & Origin", "Spin-in-place vs. orbit -- the same rotation, two different origins"));
    demos.push_back(MakeDemo<ScaleAndEffectsScreen>(
        "Scale & SpriteEffects", "float vs. Vector2 scale + FlipHorizontally/FlipVertically"));
    return demos;
}

inline std::vector<DemoEntry> BuildSortModesDemos() {
    using namespace CnaExamples::Demos::Graphics2D::SortModesDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<DeferredSortScreen>(
        "Deferred", "The default -- submission order preserved, depth ignored"));
    demos.push_back(MakeDemo<ImmediateSortScreen>(
        "Immediate", "Each Draw() flushes as its own GPU call, not queued"));
    demos.push_back(MakeDemo<TextureSortScreen>(
        "Texture", "Sorted by raw Texture2D* pointer value -- literally, not a smart heuristic"));
    demos.push_back(MakeDemo<BackToFrontSortScreen>(
        "BackToFront", "Sorted by layerDepth descending -- largest depth drawn first"));
    demos.push_back(MakeDemo<FrontToBackSortScreen>(
        "FrontToBack", "Sorted by layerDepth ascending -- opposite top layer from BackToFront"));
    return demos;
}

inline std::vector<DemoEntry> BuildDrawStringDemos() {
    using namespace CnaExamples::Demos::Graphics2D::DrawStringDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<BasicTextScreen>(
        "Basics", "DrawString() + MeasureString() side by side"));
    demos.push_back(MakeDemo<TransformedTextScreen>(
        "Transform", "Rotation/origin/scale on text, animated live"));
    demos.push_back(MakeDemo<FlippedTextScreen>(
        "SpriteEffects & Newline Quirk", "Whole-string mirroring + a real MeasureString(\"\\n\") gotcha"));
    demos.push_back(MakeDemo<StringBuilderScreen>(
        "StringBuilder Overload", "The same DrawString()/MeasureString() surface, fed a StringBuilder"));
    return demos;
}

inline std::vector<DemoEntry> BuildBeginEndStateDemos() {
    using namespace CnaExamples::Demos::Graphics2D::BeginEndStateDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<NestedBeginThrowsScreen>(
        "Nested Begin() Throws", "Begin() twice without an intervening End()"));
    demos.push_back(MakeDemo<DrawOutsideBeginThrowsScreen>(
        "Draw Outside Begin() Throws", "Draw()/DrawString() with no open Begin() session"));
    demos.push_back(MakeDemo<BlendStateLeakScreen>(
        "BlendState Leaks Past End()", "A real, documented gotcha -- End() never restores device state"));
    demos.push_back(MakeDemo<TransformMatrixScreen>(
        "Begin() Transform Matrix", "The full 7-arg Begin() overload -- an orbiting-camera matrix"));
    return demos;
}

inline std::vector<DemoEntry> BuildTexture2DBasicsDemos() {
    using namespace CnaExamples::Demos::Graphics2D::Texture2DBasicsDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ProceduralCreationScreen>(
        "Procedural Creation", "CreateFromPixels (NOXNA) vs. Texture2D(w,h)+SetData (XNA-idiomatic)"));
    demos.push_back(MakeDemo<SetDataGetDataRoundTripScreen>(
        "SetData/GetData Round-Trip", "Write a known pattern, read it back, verify byte-for-byte"));
    demos.push_back(MakeDemo<PropertiesAndDisposeScreen>(
        "Properties & Dispose", "Width/Height/Bounds + the IsDisposed/HasBackend lifecycle"));
    demos.push_back(MakeDemo<SurfaceFormatMatrixScreen>(
        "SurfaceFormat Matrix",
        "All 19 formats constructed for real -- there is no SupportsFormat() query",
        {"Texture2D", "SurfaceFormat"}));
    return demos;
}

inline std::vector<DemoEntry> BuildSaveAsReloadDemos() {
    using namespace CnaExamples::Demos::Graphics2D::SaveAsReloadDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<SaveAsPngRoundTripScreen>(
        "SaveAsPng Round-Trip", "A real PNG file, written then reloaded from disk -- not a stub"));
    demos.push_back(MakeDemo<SaveAsJpegRoundTripScreen>(
        "SaveAsJpeg Round-Trip", "Same pattern -- honestly notes JPEG's lossy compression"));
    return demos;
}

inline std::vector<DemoEntry> BuildSpriteFontDemos() {
    using namespace CnaExamples::Demos::Graphics2D::SpriteFontDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<MeasureStringScreen>(
        "MeasureString", "Real measured width/height for short, long, and multi-line strings"));
    demos.push_back(MakeDemo<DefaultCharacterFallbackScreen>(
        "DefaultCharacter Fallback", "With a fallback set vs. unset -- one throws, one doesn't"));
    demos.push_back(MakeDemo<LineSpacingAndSpacingScreen>(
        "LineSpacing & Spacing", "Live-adjustable, on the app's own shared menu font"));
    demos.push_back(MakeDemo<HandBuiltFontScreen>(
        "Hand-Built From Scratch", "A brand-new SpriteFont via its public NOXNA constructor"));
    return demos;
}

inline std::vector<DemoEntry> BuildBlendStateDemos() {
    using namespace CnaExamples::Demos::Graphics2D::BlendStateDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<BlendModeComparisonScreen>(
        "Mode Comparison", "Opaque/AlphaBlend/Additive/NonPremultiplied on overlapping quads"));
    demos.push_back(MakeDemo<PremultipliedAlphaGotchaScreen>(
        "Premultiplied Alpha", "The classic mismatched-pairing gotcha, shown correct vs. wrong"));
    return demos;
}

inline std::vector<DemoEntry> BuildSamplerStateDemos() {
    using namespace CnaExamples::Demos::Graphics2D::SamplerStateDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<FilterComparisonScreen>(
        "Filter Comparison", "PointClamp (crisp) vs. LinearClamp (smooth) on an upscaled texture"));
    demos.push_back(MakeDemo<AddressModeScreen>(
        "Address Mode", "Wrap (tiles) vs. Clamp (smears) via an oversized source rectangle"));
    return demos;
}

inline std::vector<DemoEntry> BuildViewportScissorDemos() {
    using namespace CnaExamples::Demos::Graphics2D::ViewportScissorDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ViewportInspectorScreen>(
        "Inspector", "Real live Viewport X/Y/Width/Height/AspectRatio, temporarily resizable"));
    demos.push_back(MakeDemo<ScissorClippingScreen>(
        "Scissor Clipping", "GraphicsDevice.ScissorRectangle + RasterizerState, set directly"));
    demos.push_back(MakeDemo<ViewportScissorResetGotchaScreen>(
        "Reset on RT Switch", "A real gotcha -- SetRenderTarget silently resets both"));
    return demos;
}

inline std::vector<DemoEntry> BuildRenderToTextureBasicsDemos() {
    using namespace CnaExamples::Demos::Graphics2D::RenderToTextureBasicsDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<DrawIntoRenderTargetScreen>(
        "Draw Into & Sample", "Render into a RenderTarget2D once, then draw it like any texture"));
    demos.push_back(MakeDemo<MultiTargetRoundTripScreen>(
        "Multi-Target Round Trip", "Switching between 2 render targets and the backbuffer"));
    return demos;
}

inline std::vector<DemoEntry> BuildScreenTransitionDemos() {
    using namespace CnaExamples::Demos::Graphics2D::ScreenTransitionDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<FadeTransitionScreen>(
        "Fade Transition Effect", "A live, animated render-target-as-post-effect-surface demo"));
    return demos;
}

inline std::vector<DemoEntry> BuildDisposeSafetyDemos() {
    using namespace CnaExamples::Demos::Graphics2D::DisposeSafetyDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<DisposeWhileBoundThrowsScreen>(
        "Dispose While Bound Throws", "Disposing a RenderTarget2D that's still the active target"));
    return demos;
}

inline std::vector<DemoEntry> BuildVertexTypesDemos() {
    using namespace CnaExamples::Demos::Graphics3D::VertexTypesDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<PositionColorScreen>(
        "VertexPositionColor", "Position + Color only -- a flat-colored, unlit spinning cube"));
    demos.push_back(MakeDemo<PositionTextureScreen>(
        "VertexPositionTexture", "Position + UV only -- an unlit, procedurally-textured cube"));
    demos.push_back(MakeDemo<PositionNormalTextureScreen>(
        "VertexPositionNormalTexture", "Position + Normal + UV -- the vertex type BasicEffect needs to be lit"));
    return demos;
}

inline std::vector<DemoEntry> BuildPrimitiveTypesDemos() {
    using namespace CnaExamples::Demos::Graphics3D::PrimitiveTypesDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<TriangleListVsStripScreen>(
        "TriangleList vs. TriangleStrip", "The same 4-triangle band, 12 explicit verts vs. 6 shared verts"));
    demos.push_back(MakeDemo<LineAndPointScreen>(
        "LineList / LineStrip / PointListEXT", "The same square's corners drawn 3 different primitive ways"));
    return demos;
}

inline std::vector<DemoEntry> BuildBuffersDemos() {
    using namespace CnaExamples::Demos::Graphics3D::BuffersDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ImmediateVsBufferedScreen>(
        "Immediate vs. Buffered Draw", "The same cube via DrawUserIndexedPrimitives vs. VertexBuffer+IndexBuffer"));
    demos.push_back(MakeDemo<DynamicVertexBufferScreen>(
        "DynamicVertexBuffer Ripple", "A 12x12 grid re-uploaded via SetData(..., Discard) every frame"));
    demos.push_back(MakeDemo<VertexDeclarationGotchaScreen>(
        "VertexDeclaration Stride Gotcha", "A confirmed real bug: an unrecognized stride silently drops color"));
    return demos;
}

inline std::vector<DemoEntry> BuildBasicRenderingDemos() {
    using namespace CnaExamples::Demos::Graphics3D::BasicRenderingDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<VertexColorCubeScreen>(
        "VertexColorEnabled", "Live toggle proving VertexColorEnabled gates vertex color vs. DiffuseColor"));
    demos.push_back(MakeDemo<TexturedCubeScreen>(
        "Live Texture Swap", "Cycling BasicEffect.Texture between 3 procedural textures at runtime"));
    demos.push_back(MakeDemo<AlphaBlendCubeScreen>(
        "Alpha Blending", "A translucent cube (adjustable Alpha) over an opaque one, BlendState::AlphaBlend"));
    return demos;
}

inline std::vector<DemoEntry> BuildLightingDemos() {
    using namespace CnaExamples::Demos::Graphics3D::LightingDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<DefaultLightingScreen>(
        "EnableDefaultLighting", "The stock 3-point key/fill/back lighting rig, with a live on/off toggle"));
    demos.push_back(MakeDemo<DirectionalLightsToggleScreen>(
        "3 Directional Lights", "DirectionalLight0/1/2 (red/green/blue), independently toggleable"));
    demos.push_back(MakeDemo<AmbientAndSpecularScreen>(
        "Ambient & Specular", "Live-adjustable AmbientLightColor and SpecularPower on a lit cube"));
    return demos;
}

inline std::vector<DemoEntry> BuildFogDemos() {
    using namespace CnaExamples::Demos::Graphics3D::FogDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<DistanceFogScreen>(
        "Distance Fog", "5 cubes receding into the distance, fading toward FogColor via FogStart/FogEnd"));
    return demos;
}

inline std::vector<DemoEntry> BuildAlphaTestEffectDemos() {
    using namespace CnaExamples::Demos::Graphics3D::AlphaTestEffectDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<AlphaCutoffScreen>(
        "Reference Alpha Cutoff", "A radial alpha-gradient quad, ReferenceAlpha adjustable live"));
    demos.push_back(MakeDemo<AlphaFunctionScreen>(
        "AlphaFunction", "The same gradient quad, cycling Greater/Less/GreaterEqual/Always"));
    return demos;
}

inline std::vector<DemoEntry> BuildDualTextureEffectDemos() {
    using namespace CnaExamples::Demos::Graphics3D::DualTextureEffectDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<DualTextureBlendScreen>(
        "2-Layer Blend", "A cube with 2 independent textures (Texture + Texture2) blended per pixel"));
    return demos;
}

inline std::vector<DemoEntry> BuildEnvironmentMapEffectDemos() {
    using namespace CnaExamples::Demos::Graphics3D::EnvironmentMapEffectDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ReflectiveCubeScreen>(
        "Reflective Cube", "A rotating cube fully reflecting a 6-color procedural TextureCube"));
    demos.push_back(MakeDemo<EnvironmentMapTuningScreen>(
        "Amount & Fresnel", "Live-adjustable EnvironmentMapAmount and FresnelFactor on the same cube map"));
    return demos;
}

inline std::vector<DemoEntry> BuildSkinnedEffectDemos() {
    using namespace CnaExamples::Demos::Graphics3D::SkinnedEffectDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<TwoBoneSkinningScreen>(
        "Two-Bone Bend", "A hinged strip, SetBoneTransforms animating a live bend every frame"));
    return demos;
}

inline std::vector<DemoEntry> BuildCustomShaderDemos() {
    using namespace CnaExamples::Demos::Graphics3D::CustomShaderDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<CustomGlslShaderScreen>(
        "Custom GLSL Shader", "A hand-authored ShaderEffect (time-pulsed color) on a real cube"));
    demos.push_back(MakeDemo<InvalidShaderScreen>(
        "Invalid Shader Handling", "A deliberately broken shader -- IsEffectValid() surfaces the failure safely"));
    return demos;
}

inline std::vector<DemoEntry> BuildDepthAndCullingDemos() {
    using namespace CnaExamples::Demos::Graphics3D::DepthAndCullingDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<DepthStencilComparisonScreen>(
        "DepthStencilState: Default vs. None", "2 overlapping cubes; toggling depth testing changes what occludes what"));
    demos.push_back(MakeDemo<CullModeScreen>(
        "RasterizerState: CullMode", "A spinning quad, cycling CullNone/CullClockwise/CullCounterClockwise"));
    demos.push_back(MakeDemo<FillModeWireframeScreen>(
        "RasterizerState: FillMode", "Solid vs. WireFrame toggle on a lit cube"));
    return demos;
}

inline std::vector<DemoEntry> BuildCameraAndProjectionDemos() {
    using namespace CnaExamples::Demos::Graphics3D::CameraAndProjectionDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<OrbitingCameraScreen>(
        "Orbiting View Matrix", "The camera itself orbits via a live-recomputed CreateLookAt View matrix"));
    demos.push_back(MakeDemo<PerspectiveVsOrthographicScreen>(
        "Perspective vs. Orthographic", "The same depth-staggered scene, side by side under both projections"));
    return demos;
}

inline std::vector<DemoEntry> BuildModelGroupDemos() {
    using namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ProceduralModelScreen>(
        "Procedural Construction", "A real Model/ModelMesh/ModelMeshPart graph built entirely by hand, no XNB"));
    demos.push_back(MakeDemo<ModelBoneHierarchyScreen>(
        "ModelBone Hierarchy", "A parent/child bone pair -- the child inherits the parent's own transform"));
    return demos;
}

inline std::vector<DemoEntry> BuildTexturesAndQueriesDemos() {
    using namespace CnaExamples::Demos::Graphics3D::TexturesAndQueriesDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<Texture3DVolumeScreen>(
        "Texture3D Volume & Sub-Box",
        "A volume texture written whole, then one axis-aligned sub-box, verified by GetData",
        {"Texture3D", "SurfaceFormat"}));
    demos.push_back(MakeDemo<TextureCubeFacesScreen>(
        "TextureCube Six Faces",
        "Six faces written and read back per CubeMapFace, laid out as the unfolded cross",
        {"TextureCube", "CubeMapFace"}));
    demos.push_back(MakeDemo<RenderTargetCubeScreen>(
        "RenderTargetCube Rendered Faces",
        "Each cube face bound and rendered separately, then read back",
        {"RenderTargetCube", "CubeMapFace"}));
    demos.push_back(MakeDemo<OcclusionQueryScreen>(
        "OcclusionQuery Occluded vs Visible",
        "Pixels surviving the depth test, measured behind an occluder and without one",
        {"OcclusionQuery"}));
    return demos;
}

inline std::vector<DemoEntry> BuildEffectReflectionDemos() {
    using namespace CnaExamples::Demos::Graphics3D::EffectReflectionDemos;
    std::vector<DemoEntry> demos;
    demos.push_back(MakeDemo<ReflectionSurfaceScreen>(
        "What Is Actually Exposed",
        "Parameters/Techniques measured live -- and why Parameters[\"World\"] is null here",
        {"Effect", "EffectParameterCollection", "EffectTechnique"}));
    demos.push_back(MakeDemo<EffectParameterScreen>(
        "Parameter Classes, Types & Values",
        "Class vs Type, and the silent SetValue/SetValueTranspose mismatch",
        {"EffectParameter", "EffectParameterClass", "EffectParameterType"}));
    demos.push_back(MakeDemo<EffectCloneScreen>(
        "Clone: What Is and Is Not Copied",
        "A raw owning pointer, copied state, and why ShaderEffect clones differ",
        {"Effect"}));
    return demos;
}

// Builds the full Home -> Area -> Category -> Demo data set. This is the
// single place new areas/categories/demos get registered as they are
// implemented; see plan.md section 8 for what is intentionally still empty.
inline std::vector<AreaEntry> BuildAreaCatalog() {
    return {
        AreaEntry{"Framework", {
            CategoryEntry{"Game Loop", BuildGameLoopDemos()},
            CategoryEntry{"Game Components", BuildGameComponentsDemos()},
            CategoryEntry{"Services & Dispatcher", BuildServicesDemos()},
            CategoryEntry{"Window", BuildWindowDemos()},
            CategoryEntry{"Device Manager", BuildDeviceManagerDemos()},
        }},
        AreaEntry{"Math", {
            CategoryEntry{"Vectors", BuildMathVectorsDemos()},
            CategoryEntry{"Matrix & Quaternion", BuildMatrixQuaternionDemos()},
            CategoryEntry{"Geometry", BuildGeometryDemos()},
            CategoryEntry{"Curves", BuildCurvesDemos()},
            CategoryEntry{"Color & Packed Vectors", BuildColorDemos()},
        }},
        AreaEntry{"Content", {
            CategoryEntry{"ContentManager Basics", BuildContentBasicsDemos()},
            CategoryEntry{"Manifest", BuildContentManifestDemos()},
            CategoryEntry{"CNJ Format", BuildContentCnjDemos()},
            CategoryEntry{"XNB Format", BuildContentXnbDemos()},
            CategoryEntry{"Errors", BuildContentErrorsDemos()},
        }},
        AreaEntry{"Storage", {
            CategoryEntry{"StorageDevice", BuildStorageDeviceDemos()},
            CategoryEntry{"StorageContainer", BuildStorageContainerDemos()},
        }},
        AreaEntry{"Diagnostics", {
            CategoryEntry{"Logging", BuildDiagnosticsLoggingDemos()},
            CategoryEntry{"Platform & Build", BuildDiagnosticsPlatformDemos()},
            CategoryEntry{"Backend & Capabilities", BuildDiagnosticsCapabilitiesDemos()},
            CategoryEntry{"Adapter & Display", BuildDiagnosticsAdapterDemos()},
        }},
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
            CategoryEntry{"XACT", BuildXactDemos()},
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
        AreaEntry{"Media", {
            CategoryEntry{"Song", BuildSongDemos()},
            CategoryEntry{"Video", BuildVideoDemos()},
            CategoryEntry{"MediaLibrary", BuildMediaLibraryDemos()},
            CategoryEntry{"Pictures", BuildPictureDemos()},
        }},
        AreaEntry{"2D Graphics", {}, {
            GroupEntry{"SpriteBatch", {
                CategoryEntry{"Drawing Basics", BuildDrawingBasicsDemos()},
                CategoryEntry{"Sort Modes", BuildSortModesDemos()},
                CategoryEntry{"DrawString", BuildDrawStringDemos()},
                CategoryEntry{"Begin/End & State", BuildBeginEndStateDemos()},
            }},
            GroupEntry{"Textures & Fonts", {
                CategoryEntry{"Texture2D Basics", BuildTexture2DBasicsDemos()},
                CategoryEntry{"SaveAs & Reload", BuildSaveAsReloadDemos()},
                CategoryEntry{"SpriteFont", BuildSpriteFontDemos()},
            }},
            GroupEntry{"Device State & Blending", {
                CategoryEntry{"BlendState", BuildBlendStateDemos()},
                CategoryEntry{"SamplerState", BuildSamplerStateDemos()},
                CategoryEntry{"Viewport & Scissor", BuildViewportScissorDemos()},
            }},
            GroupEntry{"Render Targets", {
                CategoryEntry{"Render-to-Texture Basics", BuildRenderToTextureBasicsDemos()},
                CategoryEntry{"Screen Transition", BuildScreenTransitionDemos()},
                CategoryEntry{"Dispose Safety", BuildDisposeSafetyDemos()},
            }},
        }},
        // Every category here needs the 3D pipeline, which SDL_RENDERER, DX3
        // and CANVAS do not have -- see Requiring()'s own comment.
        AreaEntry{"3D Graphics", {}, {
            GroupEntry{"Primitives & Vertex Types", {
                CategoryEntry{"Vertex Types", Requiring(CNA::GraphicsCapability::ThreeD, BuildVertexTypesDemos())},
                CategoryEntry{"Primitive Types", Requiring(CNA::GraphicsCapability::ThreeD, BuildPrimitiveTypesDemos())},
                CategoryEntry{"Buffers", Requiring(CNA::GraphicsCapability::ThreeD, BuildBuffersDemos())},
            }},
            GroupEntry{"BasicEffect & Lighting", {
                CategoryEntry{"Basic Rendering", Requiring(CNA::GraphicsCapability::ThreeD, BuildBasicRenderingDemos())},
                CategoryEntry{"Lighting", Requiring(CNA::GraphicsCapability::ThreeD, BuildLightingDemos())},
                CategoryEntry{"Fog", Requiring(CNA::GraphicsCapability::ThreeD, BuildFogDemos())},
            }},
            GroupEntry{"Effects Gallery", {
                CategoryEntry{"AlphaTestEffect", Requiring(CNA::GraphicsCapability::ThreeD, BuildAlphaTestEffectDemos())},
                CategoryEntry{"DualTextureEffect", Requiring(CNA::GraphicsCapability::ThreeD, BuildDualTextureEffectDemos())},
                CategoryEntry{"EnvironmentMapEffect", Requiring(CNA::GraphicsCapability::ThreeD, BuildEnvironmentMapEffectDemos())},
                CategoryEntry{"SkinnedEffect", Requiring(CNA::GraphicsCapability::ThreeD, BuildSkinnedEffectDemos())},
                CategoryEntry{"Custom Shader", Requiring(CNA::GraphicsCapability::ThreeD, BuildCustomShaderDemos())},
            }},
            GroupEntry{"Device State, Camera & Model", {
                CategoryEntry{"Depth & Culling", Requiring(CNA::GraphicsCapability::ThreeD, BuildDepthAndCullingDemos())},
                CategoryEntry{"Camera & Projection", Requiring(CNA::GraphicsCapability::ThreeD, BuildCameraAndProjectionDemos())},
                CategoryEntry{"Model", Requiring(CNA::GraphicsCapability::ThreeD, BuildModelGroupDemos())},
            }},
            GroupEntry{"Textures, Effects & Queries", {
                CategoryEntry{"Volume & Cube Textures", Requiring(CNA::GraphicsCapability::ThreeD, BuildTexturesAndQueriesDemos())},
                CategoryEntry{"Effect Reflection", Requiring(CNA::GraphicsCapability::ThreeD, BuildEffectReflectionDemos())},
            }},
        }},
    };
}

} // namespace CnaExamples::Navigation

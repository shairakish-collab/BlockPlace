// RandomBlockPlacementMod.cpp
//
// Built against the real LiteLDev/preloader-android SDK (confirmed via
// official docs: https://levilaunchroid.levimc.org/guide/developer).
//
// STATUS:
//   - Manifest, lifecycle, config, and the in-game "R" toggle button are
//     built against the real, documented API and should be correct.
//   - The actual interception of block placement is still a stub. That
//     requires a Signature (per the SDK's Signature API) pointing at the
//     internal engine function that handles block placement in
//     libminecraftpe.so, for your specific Minecraft version. That's not
//     something I can fabricate - it has to come from actually scanning
//     your installed Minecraft binary (e.g. with Ghidra/IDA) or a
//     published signature for that function/version, neither of which
//     I have access to here. Search TODO markers below.

#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <pl/Config.hpp>
#include <pl/Mod.hpp>
#include <pl/ModMenu.hpp>
#include <pl/memory/Hook.hpp>
#include <pl/memory/Signature.hpp>

struct RbpConfig {
    int  version = 1;
    bool enabled = true; // whether random placement is currently active
};

class RandomBlockPlacementMod {
public:
    static RandomBlockPlacementMod& instance();

    RandomBlockPlacementMod();

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    bool load();
    bool enable();
    bool disable();
    bool unload();

private:
    ll::mod::NativeMod& mSelf;
    std::optional<pl::config::ConfigFile<RbpConfig>> mConfig;

    void registerToggleButton();
    void toggleEnabled();

    // --- the unresolved piece, see file header ---
    bool installPlacementHook();
    void removePlacementHook();
};

PL_REGISTER_MOD(RandomBlockPlacementMod, RandomBlockPlacementMod::instance())

RandomBlockPlacementMod& RandomBlockPlacementMod::instance() {
    static RandomBlockPlacementMod mod;
    return mod;
}

RandomBlockPlacementMod::RandomBlockPlacementMod() : mSelf(*ll::mod::NativeMod::current()) {}

bool RandomBlockPlacementMod::load() {
    auto& self = getSelf();
    std::filesystem::create_directories(self.getConfigDir());

    mConfig.emplace();
    if (!mConfig->load()) {
        self.getLogger().error("failed to load config");
        return false;
    }

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    self.getLogger().info("Loaded {}", self.getName());
    return true;
}

bool RandomBlockPlacementMod::enable() {
    auto& self = getSelf();

    bool menuOk = pl::modmenu::ModuleBuilder("random_block_placement.main",
                                              "Random Block Placement")
        .modId(self.getId())
        .description("Sneak + place, or hit the R button, to place a random "
                      "block from your hotbar instead.")
        .defaultEnabled(mConfig->value().enabled)
        .registerModule();

    registerToggleButton();

    if (!installPlacementHook()) {
        self.getLogger().warn(
            "placement hook not installed yet - see source comments; the "
            "R button and config will work, but placement isn't "
            "intercepted until the real Signature is filled in.");
    }

    return menuOk;
}

bool RandomBlockPlacementMod::disable() {
    removePlacementHook();
    return true;
}

bool RandomBlockPlacementMod::unload() {
    return true;
}

void RandomBlockPlacementMod::registerToggleButton() {
    // androidKeyCode(46) == android.view.KeyEvent.KEYCODE_R.
    // Matches the original Fabric mod's default "R" keybind.
    pl::modmenu::ButtonBuilder("random_block_placement.toggle", "Randomize")
        .moduleId("random_block_placement.main")
        .modId(getSelf().getId())
        .label("R")
        .androidKeyCode(46)
        .behavior(pl::modmenu::ButtonBehavior::Click)
        .registerButton();
}

void RandomBlockPlacementMod::toggleEnabled() {
    auto& cfg = mConfig->value();
    cfg.enabled = !cfg.enabled;
    mConfig->save();
    getSelf().getLogger().info("Random placement now {}", cfg.enabled ? "ON" : "OFF");
}

// ---------------------------------------------------------------------
// The unresolved piece.
// ---------------------------------------------------------------------
//
// Real shape once you have the signature (mirrors LeviLamina's Hook.h
// pattern, which preloader-android's pl::memory::Hook is modeled on):
//
//   LL_AUTO_TYPED_INSTANCE_HOOK(
//       BlockPlacementHook,
//       pl::memory::HookPriority::Normal,
//       "<mangled-or-pattern-signature-for-the-place-block-function>",
//       bool, /* return type, adjust to match real signature */
//       void* thisPtr, /* + real parameters: player, block pos, face, etc */
//   ) {
//       if (!RandomBlockPlacementMod::instance().isEnabled()) {
//           return origin(thisPtr, ...);
//       }
//       // pick a random placeable block from the player's hotbar and
//       // place that instead - real game-state access (inventory,
//       // dimension, block permutation) also needs the real client-side
//       // type headers, which come from the same signature/typeinfo
//       // work as the hook target itself.
//       ...
//   }
//
// TODO: obtain the real signature for the block-placement function in
// your target Minecraft version (via the SDK's Signature API + a
// disassembler on your installed libminecraftpe.so), then replace the
// two stub functions below with a real LL_AUTO_TYPED_INSTANCE_HOOK.

bool RandomBlockPlacementMod::installPlacementHook() {
    // TODO: resolve real signature, install hook. Returning false so the
    // mod is honest about not being functional yet rather than silently
    // pretending to work.
    return false;
}

void RandomBlockPlacementMod::removePlacementHook() {
    // TODO: unhook once installPlacementHook() does something real.
}

# Random Block Placement (native)

Built against the real SDK: https://github.com/LiteLDev/preloader-android
Docs: https://levilaunchroid.levimc.org/guide/developer

## What actually works in this scaffold
- `manifest.json` - real `preload-native` schema.
- Mod lifecycle (`load`/`enable`/`disable`/`unload`) via `PL_REGISTER_MOD`.
- Typed config (`config/config.json` + schema) for the enabled/disabled state.
- A real in-game Mod Menu entry, plus a floating "R" button bound to
  `androidKeyCode(46)` (Android's `KEYCODE_R`) - matches the original
  Fabric mod's default keybind.
- A GitHub Actions workflow that fetches the SDK via CMake `FetchContent`
  and builds a real `.levipack` artifact on every push.

## What's still a stub
`installPlacementHook()` / `removePlacementHook()` in
`src/RandomBlockPlacementMod.cpp` don't do anything yet. Actually
intercepting block placement needs a **Signature** (see the SDK's
Signature API docs) pointing at the internal engine function in
`libminecraftpe.so` that handles block placement, for your specific
Minecraft version. That has to come from reverse-engineering your
installed Minecraft binary (Ghidra/IDA) or a published signature for
that function - neither of which is something that can be fabricated
safely. Guessing here risks a genuine crash on real devices.

Once you (or the community) have that signature, replace the two stub
functions with a real `LL_AUTO_TYPED_INSTANCE_HOOK` per the Hook API
docs - the comment block in the source shows the intended shape.

## Building
Push to a repo with the included `.github/workflows/build.yml`, or
locally with the Android NDK:

```
cmake -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24
cmake --build build
```

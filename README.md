# GTA5 City Unlock 2.0 — GTA SA Android 2.10 ARM64

AML native mod for the supplied GTA SA Android 2.10 ARM64 `libGTASA.so`.

This version uses three layers:

1. NOPs the only direct call found in `CGameLogic::Update()` to `SetPlayerWantedLevelForForbiddenTerritories(bool)` at RVA `0x003CD660`.
2. Hooks `CGameLogic::SetPlayerWantedLevelForForbiddenTerritories(bool)` at RVA `0x003CE724` and suppresses its body.
3. Hooks `CGameLogic::IsPlayerAllowedToGoInThisDirection(...)` at RVA `0x003CEF9C` and returns true.

Install the built `libGTA5CityUnlock.so` directly in:

`Android_unprotected/data/com.rockstargames.gtasa/mods/`

Do not replace `libGTASA.so` and do not modify/repack the APK.

Important: the RVAs were derived from the supplied 2.10 ARM64 library. If the installed game uses a different library build, the offsets must be rechecked.

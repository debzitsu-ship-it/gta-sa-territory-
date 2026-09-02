# GTA5 City Unlock — AML 1.0

Target: GTA San Andreas Android 2.10 ARM64/AArch64.

## What it does

Disables the game's automatic wanted-level routine for forbidden territories. This is intended to let the player travel into the other cities without the automatic 4-star wanted trigger.

Normal wanted levels from missions, police actions, etc. are not intentionally changed by this mod.

## Target verified in libGTASA.so

`CGameLogic::SetPlayerWantedLevelForForbiddenTerritories(bool)`

Address in the supplied 2.10 ARM64 library:

`0x003CE724`

The function body was inspected and contains calls to `CPlayerPed::SetWantedLevel(4)` at `0x003CE844` and `0x003CE8F0`. The AML hook suppresses the entire routine instead of patching the game library.

## Install

Build the GitHub Actions workflow. Put the resulting:

`libGTA5CityUnlock.so`

into the AML mods directory used by your GTA SA 2.10 setup.

For an Android_unprotected AML setup, use the same `mods` location where your other AML `.so` mods are loaded.

## Notes

This mod is specifically for the supplied GTA SA 2.10 ARM64 library. Do not use the `.so` on a different GTA SA version/build without re-checking the target address.

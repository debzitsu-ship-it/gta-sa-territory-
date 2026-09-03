# GTA5 City Unlock 4.0

Target: GTA San Andreas Android 2.10 ARM64.

This version targets two separate systems in the supplied libGTASA.so:

1. `CGameLogic::SetPlayerWantedLevelForForbiddenTerritories(bool)`
   RVA `0x003CE724`

2. Binary IPL barrier groups:
   - `barriers1`
   - `barriers2`

CIplStore functions used:
- `FindIplSlot` RVA `0x0033B568`
- `RequestIplAndIgnore` RVA `0x0033D20C`
- `RemoveIplAndIgnore` RVA `0x0033D26C`

Install the resulting `libGTA5CityUnlock.so` directly in:

Android_unprotected/data/com.rockstargames.gtasa/mods/

Do not replace libGTASA.so.

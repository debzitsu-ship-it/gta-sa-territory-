#include <mod/amlmod.h>
#include <mod/logger.h>
#include <cstdint>

MYMOD(
    net.debzitsu.gta5cityunlock,
    GTA5 City Unlock,
    2.0,
    Debzitsu
)

NEEDGAME(com.rockstargames.gtasa)

BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.3.0)
END_DEPLIST()

namespace
{
    // GTA SA Android 2.10 ARM64 RVAs from the supplied libGTASA.so.
    constexpr uintptr_t kGameLogicUpdateCall = 0x003CD660;
    constexpr uintptr_t kForbiddenWantedFunction = 0x003CE724;
    constexpr uintptr_t kDirectionCheck = 0x003CEF9C;

    DECL_HOOKv(SetPlayerWantedLevelForForbiddenTerritories, bool)
    {
        // Deliberately do not call the original routine: it is the routine
        // that raises the automatic forbidden-territory wanted level.
        logger->Info("GTA5 City Unlock: forbidden-territory wanted trigger blocked");
    }

    DECL_HOOKb(IsPlayerAllowedToGoInThisDirection, void* self, float x, float y, float z, float extra)
    {
        // Allow the player through the game's directional territory gate.
        // The original function's CVector is three floats, which are passed
        // in floating-point registers on AArch64; this hook intentionally
        // ignores those values.
        (void)self;
        (void)x;
        (void)y;
        (void)z;
        (void)extra;
        return true;
    }
}

ON_MOD_PRELOAD()
{
    logger->SetTag("GTA5 City Unlock");
    logger->Info("GTA5 City Unlock 2.0: preload");
}

ON_MOD_LOAD()
{
    uintptr_t game = aml->GetLib("libGTASA.so");
    if (!game)
    {
        logger->Error("GTA5 City Unlock: libGTASA.so not found");
        return;
    }

    logger->Info("GTA5 City Unlock 2.0: libGTASA.so=%p", (void*)game);

    // 1) Remove the sole direct call in CGameLogic::Update() that invokes
    //    SetPlayerWantedLevelForForbiddenTerritories().
    aml->PlaceNOP(game + kGameLogicUpdateCall, 1);

    // 2) Also hook the target routine itself as a second layer.
    HOOK(SetPlayerWantedLevelForForbiddenTerritories,
         game + kForbiddenWantedFunction);

    // 3) Allow movement through the game's directional territory gate.
    HOOK(IsPlayerAllowedToGoInThisDirection,
         game + kDirectionCheck);

    logger->Info("GTA5 City Unlock 2.0: patches/hooks installed");
}

ON_MOD_UNLOAD()
{
    logger->Info("GTA5 City Unlock 2.0: unloaded");
}

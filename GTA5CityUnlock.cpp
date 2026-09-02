#include <mod/amlmod.h>
#include <mod/logger.h>

#include <cstdint>

MYMOD(net.debzitsu.gta5cityunlock, GTA5 City Unlock, 1.0, Debzitsu)
NEEDGAME(com.rockstargames.gtasa)

BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.3.0)
END_DEPLIST()

namespace {

// GTA SA 2.10 ARM64 / AArch64
// CGameLogic::SetPlayerWantedLevelForForbiddenTerritories(bool)
constexpr uintptr_t OFF_FORBIDDEN_TERRITORY_WANTED = 0x003CE724;

}

// The target is a void CGameLogic function with one bool argument.
// We intentionally do NOT call the original function: its body contains
// the calls that assign 4 wanted stars for forbidden territories.
DECL_HOOKv(SetPlayerWantedLevelForForbiddenTerritories, bool forbiddenTerritory)
{
    (void)forbiddenTerritory;
    logger->Info("City Unlock: blocked forbidden-territory wanted check");
    return;
}

ON_MOD_PRELOAD()
{
    logger->SetTag("GTA5 City Unlock");
    logger->Info("GTA5 City Unlock 1.0: preload");
}

ON_MOD_LOAD()
{
    logger->Info("GTA5 City Unlock 1.0: loading");

    const uintptr_t pGame = aml->GetLib("libGTASA.so");
    if (!pGame) {
        logger->Error("GTA5 City Unlock: libGTASA.so not found");
        return;
    }

    const uintptr_t target = pGame + OFF_FORBIDDEN_TERRITORY_WANTED;
    logger->Info("GTA5 City Unlock: target=%p", reinterpret_cast<void*>(target));

    HOOK(SetPlayerWantedLevelForForbiddenTerritories, target);

    logger->Info("GTA5 City Unlock: hook installed; forbidden-territory wanted trigger disabled");
}

ON_MOD_UNLOAD()
{
    logger->Info("GTA5 City Unlock: unloaded");
}


#include <mod/amlmod.h>
#include <mod/logger.h>

#include <cstdint>

MYMOD(
    net.debzitsu.gta5cityunlock,
    GTA5 City Unlock,
    1.0,
    Debzitsu
)

NEEDGAME(com.rockstargames.gtasa)

BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.3.0)
END_DEPLIST()

namespace
{
    // GTA SA 2.10 ARM64
    constexpr uintptr_t OFF_FORBIDDEN_TERRITORY =
        0x003CE724;

    uintptr_t pGame = 0;

    /*
     * CGameLogic::SetPlayerWantedLevelForForbiddenTerritories(bool)
     *
     * We deliberately do NOT call the original function.
     *
     * This prevents the automatic wanted-level assignment caused
     * by entering a forbidden territory.
     *
     * Normal wanted-level mechanics are not hooked here.
     */
    DECL_HOOKv(
        SetPlayerWantedLevelForForbiddenTerritories,
        bool
    )
    {
        logger->Info(
            "GTA5 City Unlock: forbidden-territory wanted trigger blocked"
        );

        return;
    }
}

ON_MOD_PRELOAD()
{
    logger->SetTag("GTA5 City Unlock");
    logger->Info("GTA5 City Unlock: preload");
}

ON_MOD_LOAD()
{
    logger->Info("GTA5 City Unlock: loading");

    pGame = aml->GetLib("libGTASA.so");

    if (!pGame)
    {
        logger->Error(
            "GTA5 City Unlock: libGTASA.so not found"
        );
        return;
    }

    logger->Info(
        "GTA5 City Unlock: libGTASA.so base = %p",
        reinterpret_cast<void*>(pGame)
    );

    const uintptr_t target =
        pGame + OFF_FORBIDDEN_TERRITORY;

    logger->Info(
        "GTA5 City Unlock: target = %p",
        reinterpret_cast<void*>(target)
    );

    HOOK(
        SetPlayerWantedLevelForForbiddenTerritories,
        target
    );

    logger->Info(
        "GTA5 City Unlock: hook installed"
    );
}

ON_MOD_UNLOAD()
{
    logger->Info(
        "GTA5 City Unlock: unloaded"
    );
}

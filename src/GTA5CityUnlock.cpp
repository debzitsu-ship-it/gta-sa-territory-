#include <amlmod.h>

#include <android/log.h>
#include <cstdint>

#define LOG_TAG "GTA5CityUnlock"

#define LOGI(...) \
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define LOGE(...) \
    __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

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
    constexpr uintptr_t OFF_FORBIDDEN_TERRITORY = 0x003CE724;

    uintptr_t pGame = 0;

    /*
     * CGameLogic::SetPlayerWantedLevelForForbiddenTerritories(bool)
     *
     * The original function automatically applies the wanted level
     * when the player enters a forbidden territory.
     *
     * We intentionally do not call the original function.
     */
    DECL_HOOKv(
        SetPlayerWantedLevelForForbiddenTerritories,
        bool
    )
    {
        LOGI("Forbidden-territory wanted trigger blocked");
        return;
    }
}

ON_MOD_PRELOAD()
{
    LOGI("GTA5 City Unlock: preload");
}

ON_MOD_LOAD()
{
    LOGI("GTA5 City Unlock: loading");

    pGame = aml->GetLib("libGTASA.so");

    if (!pGame)
    {
        LOGE("GTA5 City Unlock: libGTASA.so not found");
        return;
    }

    LOGI(
        "libGTASA.so base = %p",
        reinterpret_cast<void*>(pGame)
    );

    const uintptr_t target =
        pGame + OFF_FORBIDDEN_TERRITORY;

    LOGI(
        "Forbidden-territory target = %p",
        reinterpret_cast<void*>(target)
    );

    HOOK(
        SetPlayerWantedLevelForForbiddenTerritories,
        target
    );

    LOGI("GTA5 City Unlock: hook installed");
}

ON_MOD_UNLOAD()
{
    LOGI("GTA5 City Unlock: unloaded");
}
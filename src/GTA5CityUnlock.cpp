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
    2.0,
    Debzitsu
)

NEEDGAME(com.rockstargames.gtasa)

BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.3.0)
END_DEPLIST()

namespace
{
    /*
     * GTA SA Android 2.10 ARM64
     *
     * These RVAs are from the libGTASA.so supplied for this project.
     */

    constexpr uintptr_t OFF_FORBIDDEN_TERRITORY =
        0x003CE724;

    /*
     * Territory-check code identified around the
     * forbidden-territory wanted-level logic.
     *
     * This is kept separate from the wanted-level hook.
     */

    DECL_HOOKv(
        SetPlayerWantedLevelForForbiddenTerritories,
        bool
    )
    {
        LOGI("CityUnlock: forbidden-territory wanted request blocked");
        return;
    }
}

ON_MOD_PRELOAD()
{
    LOGI("GTA5 City Unlock 2.0: preload");
}

ON_MOD_LOAD()
{
    LOGI("GTA5 City Unlock 2.0: loading");

    uintptr_t game = aml->GetLib("libGTASA.so");

    if (!game)
    {
        LOGE("CityUnlock: libGTASA.so not found");
        return;
    }

    LOGI(
        "CityUnlock: libGTASA.so base = %p",
        reinterpret_cast<void*>(game)
    );

    uintptr_t target =
        game + OFF_FORBIDDEN_TERRITORY;

    LOGI(
        "CityUnlock: forbidden-territory target = %p",
        reinterpret_cast<void*>(target)
    );

    HOOK(
        SetPlayerWantedLevelForForbiddenTerritories,
        target
    );

    LOGI("GTA5 City Unlock 2.0: hook installed");
}

ON_MOD_UNLOAD()
{
    LOGI("GTA5 City Unlock 2.0: unloaded");
}

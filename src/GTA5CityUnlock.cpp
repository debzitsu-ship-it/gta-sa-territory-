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
    1.1,
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

    DECL_HOOKv(
        SetPlayerWantedLevelForForbiddenTerritories,
        bool
    )
    {
        // Suppress the automatic wanted level caused
        // by entering a forbidden territory.
        LOGI("Forbidden territory wanted trigger suppressed");
        return;
    }
}

ON_MOD_PRELOAD()
{
    LOGI("GTA5 City Unlock 1.1: preload");
}

ON_MOD_LOAD()
{
    LOGI("GTA5 City Unlock 1.1: loading");

    uintptr_t game = aml->GetLib("libGTASA.so");

    if (!game)
    {
        LOGE("GTA5 City Unlock: libGTASA.so not found");
        return;
    }

    LOGI(
        "libGTASA.so base = %p",
        reinterpret_cast<void*>(game)
    );

    const uintptr_t target =
        game + OFF_FORBIDDEN_TERRITORY;

    LOGI(
        "Forbidden-territory target = %p",
        reinterpret_cast<void*>(target)
    );

    HOOK(
        SetPlayerWantedLevelForForbiddenTerritories,
        target
    );

    LOGI("GTA5 City Unlock 1.1: hook installed");
}

ON_MOD_UNLOAD()
{
    LOGI("GTA5 City Unlock 1.1: unloaded");
}

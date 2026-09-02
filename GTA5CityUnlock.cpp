#include <amlmod.h>
#include <android/log.h>
#include <cstdint>

#define LOG_TAG "GTA5CityUnlock"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

MYMOD(net.debzitsu.gta5cityunlock, GTA5 City Unlock, 1.1, Debzitsu)
NEEDGAME(com.rockstargames.gtasa)

BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.3.0)
END_DEPLIST()

namespace {
constexpr uintptr_t OFF_FORBIDDEN_TERRITORY = 0x003CE724;

DECL_HOOKv(SetPlayerWantedLevelForForbiddenTerritories, bool)
{
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
    if (!game) {
        LOGE("GTA5 City Unlock: libGTASA.so not found");
        return;
    }

    uintptr_t target = game + OFF_FORBIDDEN_TERRITORY;
    LOGI("GTA5 City Unlock: target=%p", reinterpret_cast<void*>(target));

    HOOK(SetPlayerWantedLevelForForbiddenTerritories, target);

    LOGI("GTA5 City Unlock 1.1: hook installed");
}

ON_MOD_UNLOAD()
{
    LOGI("GTA5 City Unlock 1.1: unloaded");
}

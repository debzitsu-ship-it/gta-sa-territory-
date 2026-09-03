#include <amlmod.h>
#include <android/log.h>
#include <cstdint>

#define LOG_TAG "GTA5CityUnlock"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

MYMOD(
    net.debzitsu.gta5cityunlock,
    GTA5 City Unlock,
    4.0,
    Debzitsu
)

NEEDGAME(com.rockstargames.gtasa)

BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.3.0)
END_DEPLIST()

namespace
{
    // GTA SA Android 2.10 ARM64
    constexpr uintptr_t OFF_FORBIDDEN_TERRITORY =
        0x003CE724;

    constexpr uintptr_t OFF_FIND_IPL_SLOT =
        0x0033B568;

    constexpr uintptr_t OFF_REQUEST_IPL_AND_IGNORE =
        0x0033D20C;

    constexpr uintptr_t OFF_REMOVE_IPL_AND_IGNORE =
        0x0033D26C;

    using FindIplSlotFn = int (*)(const char*);
    using RemoveIplAndIgnoreFn = void (*)(int);

    int gBarriers1 = -1;
    int gBarriers2 = -1;

    DECL_HOOKv(RequestIplAndIgnore, int iplSlot)
    {
        if (iplSlot == gBarriers1 || iplSlot == gBarriers2)
        {
            LOGI(
                "CityUnlock: blocked barrier IPL request: %d",
                iplSlot
            );
            return;
        }

        RequestIplAndIgnore(iplSlot);
    }
}

ON_MOD_PRELOAD()
{
    LOGI("GTA5 City Unlock 4.0: preload");
}

ON_MOD_LOAD()
{
    LOGI("GTA5 City Unlock 4.0: loading");

    const uintptr_t game =
        aml->GetLib("libGTASA.so");

    if (!game)
    {
        LOGE("CityUnlock: libGTASA.so not found");
        return;
    }

    LOGI(
        "CityUnlock: libGTASA.so base = %p",
        reinterpret_cast<void*>(game)
    );

    /*
     * Disable the automatic 4-star forbidden-territory check.
     */
    aml->PlaceRET(
        game + OFF_FORBIDDEN_TERRITORY
    );

    LOGI(
        "CityUnlock: forbidden-territory routine patched"
    );

    /*
     * Find the two island barrier IPL slots.
     */
    auto FindIplSlot =
        reinterpret_cast<FindIplSlotFn>(
            game + OFF_FIND_IPL_SLOT
        );

    gBarriers1 =
        FindIplSlot("barriers1");

    gBarriers2 =
        FindIplSlot("barriers2");

    LOGI(
        "CityUnlock: barriers1 slot = %d",
        gBarriers1
    );

    LOGI(
        "CityUnlock: barriers2 slot = %d",
        gBarriers2
    );

    /*
     * Remove both barrier IPLs immediately.
     */
    auto RemoveIplAndIgnore =
        reinterpret_cast<RemoveIplAndIgnoreFn>(
            game + OFF_REMOVE_IPL_AND_IGNORE
        );

    if (gBarriers1 >= 0)
    {
        RemoveIplAndIgnore(gBarriers1);
        LOGI("CityUnlock: barriers1 removed");
    }

    if (gBarriers2 >= 0)
    {
        RemoveIplAndIgnore(gBarriers2);
        LOGI("CityUnlock: barriers2 removed");
    }

    /*
     * Prevent the game/scripts from requesting
     * those barrier IPLs again.
     */
    HOOK(
        RequestIplAndIgnore,
        game + OFF_REQUEST_IPL_AND_IGNORE
    );

    LOGI(
        "GTA5 City Unlock 4.0: barrier protection installed"
    );
}

ON_MOD_UNLOAD()
{
    LOGI("GTA5 City Unlock 4.0: unloaded");
}
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
    // GTA SA 2.10 ARM64 - exact libGTASA.so supplied for this project.
    constexpr uintptr_t OFF_FORBIDDEN_TERRITORY =
        0x003CE724;

    // CIplStore functions identified in the supplied libGTASA.so.
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
            LOGI("CityUnlock: blocked request for barrier IPL slot %d", iplSlot);
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

    const uintptr_t game = aml->GetLib("libGTASA.so");

    if (!game)
    {
        LOGE("CityUnlock: libGTASA.so not found");
        return;
    }

    LOGI("CityUnlock: libGTASA.so base = %p",
         reinterpret_cast<void*>(game));

    /*
     * 1. Permanently suppress the forbidden-territory wanted routine.
     *
     * The function itself checks the city-unlock stat and assigns
     * wanted level 4 when the player enters a locked city.
     */
    aml->PlaceRET(game + OFF_FORBIDDEN_TERRITORY);

    LOGI("CityUnlock: forbidden-territory routine patched");

    /*
     * 2. Find the actual binary IPL slots used by the map.
     *
     * The game's object groups are named BARRIERS1 and BARRIERS2
     * in the script layer; CIplStore::FindIplSlot performs a
     * case-insensitive lookup.
     */
    auto FindIplSlot =
        reinterpret_cast<FindIplSlotFn>(game + OFF_FIND_IPL_SLOT);

    gBarriers1 = FindIplSlot("barriers1");
    gBarriers2 = FindIplSlot("barriers2");

    LOGI("CityUnlock: barriers1 slot = %d", gBarriers1);
    LOGI("CityUnlock: barriers2 slot = %d", gBarriers2);

    /*
     * 3. Remove the barrier IPLs immediately.
     *
     * RemoveIplAndIgnore unloads the IPL and marks it so the normal
     * streaming/request path does not immediately bring it back.
     */
    auto RemoveIplAndIgnore =
        reinterpret_cast<RemoveIplAndIgnoreFn>(
            game + OFF_REMOVE_IPL_AND_IGNORE
        );

    if (gBarriers1 >= 0)
        RemoveIplAndIgnore(gBarriers1);

    if (gBarriers2 >= 0)
        RemoveIplAndIgnore(gBarriers2);

    /*
     * 4. Block future requests for those same IPLs.
     *
     * This covers script-side REQUEST_IPL calls after our initial
     * removal.
     */
    HOOK(
        RequestIplAndIgnore,
        game + OFF_REQUEST_IPL_AND_IGNORE
    );

    LOGI("GTA5 City Unlock 4.0: barrier IPL protection installed");
}

ON_MOD_UNLOAD()
{
    LOGI("GTA5 City Unlock 4.0: unloaded");
}

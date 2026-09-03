#include <amlmod.h>
#include <android/log.h>
#include <cstdint>

#define LOG_TAG "GTA5CityUnlock"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

MYMOD(
    net.debzitsu.gta5cityunlock,
    GTA5 City Unlock,
    3.0,
    Debzitsu
)

NEEDGAME(com.rockstargames.gtasa)

BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.3.0)
END_DEPLIST()

namespace {
    // Exact RVAs from the supplied GTA SA 2.10 ARM64 libGTASA.so.
    constexpr uintptr_t OFF_SET_WANTED_FOR_FORBIDDEN = 0x003CE724;
    constexpr uintptr_t OFF_GET_STAT_VALUE            = 0x004FADBC;
    constexpr uintptr_t OFF_SET_STAT_VALUE            = 0x004FAA40;
    constexpr uintptr_t OFF_ENTITY_RENDER             = 0x004CDC90; // CEntity::Remove is here; Render is below.
    constexpr uintptr_t OFF_ENTITY_RENDER_REAL        = 0x004CD2B0; // CEntity::IsVisible; kept for reference only.
    constexpr uintptr_t OFF_COBJECT_RENDER            = 0x0053E0F0;
    constexpr uintptr_t OFF_CPHYSICAL_TEST_COLLISION  = 0x004E532C;

    // Model-index globals in the supplied binary.
    constexpr uintptr_t OFF_MI_ROADWORKBARRIER1 = 0x00886204;
    constexpr uintptr_t OFF_MI_BARRIER1          = 0x00886262;

    // GTA SA stat 181 is the islands-unlocked stat.
    constexpr unsigned short STAT_CITY_UNLOCKED = 181;

    uint16_t gBarrier1 = 0xFFFF;
    uint16_t gRoadworkBarrier1 = 0xFFFF;

    static inline uint16_t GetModelIndex(void* self) {
        // CEntity::m_nModelIndex is 0x22 in the SA entity layout.
        return *reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(self) + 0x22);
    }

    static inline bool IsIslandBarrier(void* self) {
        if (!self) return false;
        const uint16_t model = GetModelIndex(self);
        return model == gBarrier1 || model == gRoadworkBarrier1;
    }

    // Block the game's forbidden-territory 4-star routine.
    DECL_HOOKv(SetPlayerWantedLevelForForbiddenTerritories, bool)
    {
        return;
    }

    // Make the game believe all islands are unlocked wherever this stat is queried.
    DECL_HOOK(float, GetStatValue, unsigned short stat)
    {
        if (stat == STAT_CITY_UNLOCKED)
            return 4.0f;

        return GetStatValue(stat);
    }

    // Keep scripts from changing the islands-unlocked stat back to a locked value.
    DECL_HOOKv(SetStatValue, unsigned short stat, float value)
    {
        if (stat == STAT_CITY_UNLOCKED) {
            SetStatValue(stat, 4.0f);
            return;
        }

        SetStatValue(stat, value);
    }

    // Island barricades are CObject instances. Suppress their rendering.
    DECL_HOOKv(CObjectRender, void* self)
    {
        if (IsIslandBarrier(self))
            return;

        CObjectRender(self);
    }

    // Suppress collision against the barrier objects.
    DECL_HOOKb(CPhysicalTestCollision, void* self, bool applySpeed)
    {
        if (IsIslandBarrier(self))
            return false;

        return CPhysicalTestCollision(self, applySpeed);
    }
}

ON_MOD_PRELOAD()
{
    LOGI("GTA5 City Unlock 3.0: preload");
}

ON_MOD_LOAD()
{
    LOGI("GTA5 City Unlock 3.0: loading");

    uintptr_t game = aml->GetLib("libGTASA.so");
    if (!game) {
        LOGE("libGTASA.so not found");
        return;
    }

    // These globals contain the actual runtime model IDs after game initialization.
    gRoadworkBarrier1 = *reinterpret_cast<uint16_t*>(game + OFF_MI_ROADWORKBARRIER1);
    gBarrier1 = *reinterpret_cast<uint16_t*>(game + OFF_MI_BARRIER1);

    LOGI("Barrier model IDs: MI_BARRIER1=%u MI_ROADWORKBARRIER1=%u",
         static_cast<unsigned>(gBarrier1),
         static_cast<unsigned>(gRoadworkBarrier1));

    // Force the islands-unlocked state and prevent later script changes from relocking it.
    HOOK(GetStatValue, game + OFF_GET_STAT_VALUE);
    HOOK(SetStatValue, game + OFF_SET_STAT_VALUE);

    // Extra protection against the explicit forbidden-territory wanted routine.
    HOOK(SetPlayerWantedLevelForForbiddenTerritories,
         game + OFF_SET_WANTED_FOR_FORBIDDEN);

    // Remove the physical/visual barrier objects without touching unrelated objects.
    HOOK(CObjectRender, game + OFF_COBJECT_RENDER);
    HOOK(CPhysicalTestCollision, game + OFF_CPHYSICAL_TEST_COLLISION);

    LOGI("GTA5 City Unlock 3.0: all hooks installed");
}

ON_MOD_UNLOAD()
{
    LOGI("GTA5 City Unlock 3.0: unloaded");
}

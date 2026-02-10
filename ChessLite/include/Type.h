#pragma once
#include <CoreLib/CoreID.h>
#include <CoreLib/IDManager.h>
#include <SDLCoreLib/Types/Types.h>

using CoreAppIDManager = IDManager<uint32_t, SDLCORE_INVALID_ID>;

template<typename Tag>
using CoreAppID = CoreID<uint32_t, SDLCORE_INVALID_ID, Tag>;

struct LayerEventSubscriptionTag {};

using LayerEventSubscriptionID = CoreAppID<LayerEventSubscriptionTag>;
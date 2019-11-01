#pragma once

#include <vector>
#include <assert.h>

#include "Config.hpp"

VTB_BEGIN

namespace Trace
{
    enum class Level { None = 0, Warning, Error, Info, Diag, Verbose };
    enum class Severity { None = 0, Warning = 1, Invalid = 2, Error = 4, All = Warning | Invalid | Error };

    void Create(VkInstance, Level = Level::Verbose, Severity = Severity::All);
    void ToFile(const char *prefix);
    void Destroy();

    // true if last call succeeded
    bool Succeeded();
    void SetSourcePoint(const char *file, int line, const char *function = 0, const char *call = 0);
    const char *ResultToString(VkResult);

    extern VkResult LastResult;
};

bool CheckValidationLayerSupport(const std::vector<const char *> &validationLayers);

#if !defined(VTB_TRACK_SOURCE_ON) || defined(DEBUG) || defined(VTB_DEBUG)
#   define VTB_TRACK_SOURCE_ON 1
#endif

#if VTB_TRACK_SOURCE_ON
#   define VTB_TRACK_SOURCE(call) \
        vtb::Trace::SetSourcePoint(__FILE__, __LINE__, __FUNCTION__, #call)
#else
#   define VTB_TRACK_SOURCE(call)
#endif

#define VTB_CALLV(call) \
    do \
    { \
        VTB_TRACK_SOURCE(call); \
        (call); \
    } while (0)

#define VTB_CALL(call) \
    do \
    { \
        VTB_TRACK_SOURCE(call); \
        vtb::Trace::LastResult = (call); \
        assert(vtb::Trace::LastResult == VK_SUCCESS); \
    } while (0)

VTB_END

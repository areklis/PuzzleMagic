#pragma once

#include "CoreMinimal.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

#if PLATFORM_ANDROID
#include <sys/system_properties.h>
#endif

namespace PuzzleLite
{
	// Phones and tablets get a lightweight scene: a pre-rendered backdrop picture and flat 2D sprites in place of the
	// real-time 3D scenery. A PC can try it with -lite; on Android `adb shell setprop debug.puzzle.lite 0` switches
	// it off again (for comparing frame rates).
	inline bool IsLite()
	{
		static const bool bLite = []() -> bool
		{
			if (FParse::Param(FCommandLine::Get(), TEXT("lite")))
			{
				return true;
			}
#if PLATFORM_ANDROID
			char Value[PROP_VALUE_MAX] = {};
			if (__system_property_get("debug.puzzle.lite", Value) > 0 && Value[0] == '0')
			{
				return false;
			}
			return true;
#elif PLATFORM_IOS
			return true;
#else
			return false;
#endif
		}();
		return bLite;
	}
}

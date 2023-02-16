#pragma once

#include <stdarg.h>
#include <stdio.h>

#include "Math/Color.h"
#include "Containers/UnrealString.h"
#include "Engine/Engine.h"

static void LogMessage(FColor colour, float time, const char* fmt, ...);

//#define info(...)  LogMessage(FColor::Green,  15.f, __VA_ARGS__)
//#define warn(...)  LogMessage(FColor::Yellow, 15.f, __VA_ARGS__)
#define err(...)   LogMessage(FColor::Red,    15.f, __VA_ARGS__)
//#define debug(...) LogMessage(FColor::Blue,   15.f, __VA_ARGS__)

static inline void LogMessage(FColor colour, float time, const char* fmt, ...) {
	char buf[500];
	va_list va;
	va_start(va, fmt);
	int len = vsnprintf(buf, sizeof(buf), fmt, va);
	if (len >= sizeof(buf)) {
		len = sizeof(buf) - 1;
	}
	buf[len] = '\0';
	va_end(va);

	FString msg(buf);
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, time, colour, msg);
}
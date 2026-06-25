#pragma once

#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"

DECLARE_LOG_CATEGORY_EXTERN(ReLog, Display, All);

#define RE_LOG(Format, ...) \
UE_LOGFMT(ReLog, Display, Format, ##__VA_ARGS__)

#define RE_WARN(Format, ...) \
UE_LOGFMT(ReLog, Warning, Format, ##__VA_ARGS__)
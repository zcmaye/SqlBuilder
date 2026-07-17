#pragma once

#ifdef SQLBUILDER_USE_OPTIONAL
#include <optional>
#define SUPPORT_OPTIONAL
#endif

#ifdef SQLBUILDER_USE_OATPP
#include "common/core/Types.hpp"
#define SUPPORT_OATPP
#endif

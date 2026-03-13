#pragma once


#define  SUPPORT_OPTIONAL	/*!是否支持std::optional*/
//#define  SUPPORT_OATPP		/*!是否支持std::optional*/

#ifdef SUPPORT_OPTIONAL
	#include <optional>
#endif

#ifdef  SUPPORT_OATPP
	#include "common/core/Types.hpp"
#endif

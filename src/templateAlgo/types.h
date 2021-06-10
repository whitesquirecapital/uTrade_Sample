#pragma once

#include "../wscCommon/util.h"
#include "../wscCommon/sysZTime.h"
#include "../wscCommon/util.h"

namespace wsc
{

    struct common
    {
        static std::string appConfigFilePath;
    };

    struct appConfig
    {
        static bool tickToOrderLatencyFlag;
        static int smConsumerInterval;
        static int minValidObLevel;
    };

    struct StrategyInput
    {

        int maxPos = 0;

    };

}

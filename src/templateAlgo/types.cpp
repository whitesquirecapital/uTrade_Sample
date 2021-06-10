#include "types.h"

//========================================= 39K Implementation =========================================

namespace wsc
{
    // common
    std::string common::appConfigFilePath = "";

    //AppConfig
    bool appConfig::tickToOrderLatencyFlag = false;
    int appConfig::smConsumerInterval = 0;
    int appConfig::minValidObLevel = 0;

}

#include "template.h"
#include "apiConstants.h"
#include "../wscCommon/ini.hpp"

namespace SampleTemplate
{

    // Template class constructor
    // It Recieves the Parameter Structure from the Bid Driver Function
    // Where it Type Casts them to user Params type for FILL_PARAMS Macro to work
    // In this, typically strategy creates instruments for different symbols (legs), subscribes to market data (tbt / snapshot), and create order wrappers for bidding / hedging legs
    Template::Template(API2::StrategyParameters *params) : API2::SGContext(params, "Template"),
                                                           _terminateCheck(false)
    {
        DEBUG_PRINT;
        //Set Parameters
        API2::UserParams *customParams = (API2::UserParams *)params->getInfo();
        if (!setInternalParameters(customParams, _userParams))
        {
            DEBUG_MESSAGE(reqQryDebugLog(), "Parameters not set from front end");
            terminateStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
            return;
        }

        try
        {
            initSetUp();
        }
        catch (API2::MarketDataSubscriptionFailedException &e)
        {
            DEBUG_PRINT << e.what();
            DEBUG_MESSAGE(reqQryDebugLog(), "TBT subscription Failed");
            terminateStrategyComment(API2::CONSTANTS::
                                         RSP_StrategyComment_STRATEGY_ERROR_STATE);
            // return;
        }
        catch (API2::InstrumentNotFoundException &e)
        {
            DEBUG_PRINT << e.what();
            DEBUG_MESSAGE(reqQryDebugLog(), "Instrument Not Found");
            terminateStrategyComment(API2::CONSTANTS::
                                         RSP_StrategyComment_STRATEGY_ERROR_STATE);
            // return;
        }
        catch (std::exception &e)
        {
            DEBUG_PRINT << e.what();
            DEBUG_MESSAGE(reqQryDebugLog(), "standard exception raised");
            terminateStrategyComment(API2::CONSTANTS::
                                         RSP_StrategyComment_STRATEGY_ERROR_STATE);
            // return;
        }
    }

    /* ---------------------------------------------Workflow Functions --------------------------------------------------*/

    //Recieve Callbacks on Every Market Data Event
    //In this function,we can perform the calculations if any change in market corresponding to symbolId
    void Template::onMarketDataEvent(UNSIGNED_LONG symbolId)
    {
        // DEBUG_PRINT;
        // DEBUG_MESSAGE(reqQryDebugLog(), "In onMarketDataEvent");
        onBookSnapshot(symbolId);
    }

    //Recieve Callbacks from reqTimerEvent
    //In this function,Call back when an timer set by reqTimerEvent expires
    //Here,after 1000000 microsecond i.e. 1 second this function gets the callback
    void Template::onTimerEvent()
    {
        // DEBUG_PRINT;
        reqTimerEvent(wsc::appConfig::smConsumerInterval);
        onDefaultEvent();
    }

    //Strategy Driver event
    //onDefaultEvent Called as an event if Not configured to received marketData Event while Running strategy
    void Template::onDefaultEvent()
    {
        // DEBUG_PRINT;
        // logSnapshot();
    }

    //Method to do common work for all type of confirmations, confirmation status dependent work is done in specific methods
    bool Template::processConfirmation(API2::COMMON::OrderWrapper &orderWrapper, API2::OrderConfirmation &confirmation, const API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT;
        reqQryDebugLog()->saveConfirmation(confirmation);
        if (orderWrapper._orderId == orderId)
        {
            DEBUG_PRINT;
            DEBUG_PRINT << getOrderStr(orderWrapper);
            auto ret = orderWrapper.processConfirmation(confirmation);
            DEBUG_PRINT << getOrderStr(orderWrapper);

            if (!orderWrapper._isReset && orderWrapper.getLastQuantity() == 0)
            {
                DEBUG_PRINT << getOrderStr(orderWrapper);
                orderWrapper.reset();
                DEBUG_PRINT << getOrderStr(orderWrapper);
            }
            return ret;
        }
        return false;
    }

    //CallBack When a new Order gets confirmed by exchange
    //Here we typically update the corresponding order wrapper's state, update any strategy state variables
    void Template::onConfirmed(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT;
        orderResHandler(confirmation, orderId);
        // if (!processConfirmation(_orderWrapper, confirmation, orderId))
        // {
        //     DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
        // }
    }

    //CallBack When a new order gets rejected by the exchange
    void Template::onNewReject(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT;
        orderResHandler(confirmation, orderId);
        // if (!processConfirmation(_orderWrapper, confirmation, orderId))
        // {
        //     DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
        // }
    }

    //CallBack When an IOC order gets canceled by the exchange
    void Template::onIOCCanceled(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT;
        orderResHandler(confirmation, orderId);
        // if (!processConfirmation(_orderWrapper, confirmation, orderId))
        // {
        //     DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
        // }
    }

    //CallBack When an Order gets Filled at the exchange
    //Here we typically update the corresponding order wrapper's state, update any strategy member variables like self maintained custom positions etc
    void Template::onFilled(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT;
        orderResHandler(confirmation, orderId);

        // if (!processConfirmation(_orderWrapper, confirmation, orderId))
        // {
        //     DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
        // }
    }

    //CallBack When an Order gets Partially Filled at the exchange
    void Template::onPartialFill(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT;
        orderResHandler(confirmation, orderId);
        // if (!processConfirmation(_orderWrapper, confirmation, orderId))
        // {
        //     DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
        // }
    }

    //CallBack When a order is cancelled from exchange
    void Template::onCanceled(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT;
        orderResHandler(confirmation, orderId);
        // if (!processConfirmation(_orderWrapper, confirmation, orderId))
        // {
        //     DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
        // }
    }

    //CallBack When an Order gets Replaced successfully at the exchange
    void Template::onReplaced(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT;
        orderResHandler(confirmation, orderId);
        // if (!processConfirmation(_orderWrapper, confirmation, orderId))
        // {
        //     DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
        // }
    }

    //CallBack When an Order's Replace Request gets rejected by the exchange
    void Template::onReplaceRejected(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT;
        orderResHandler(confirmation, orderId);
        // if (!processConfirmation(_orderWrapper, confirmation, orderId))
        // {
        //     DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
        // }
    }

    //CallBack for When an Order's Cancel Request gets rejected by the exchange
    void Template::onCancelRejected(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT;
        orderResHandler(confirmation, orderId);
        // if (!processConfirmation(_orderWrapper, confirmation, orderId))
        // {
        //     DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
        // }
    }

    //Strategy receives Terminate command from Front End
    void Template::onCMDTerminateStartegy()
    {
        DEBUG_PRINT;
        DEBUG_MESSAGE(reqQryDebugLog(), "Strategy Ended From FrontEnd");
        terminateStrategyComment(API2::CONSTANTS::CMD_CommandCategory_TERMINATE_STRATEGY);
    }

    /* ---------------------------------------------Implementation Functions --------------------------------------------------*/

    //This is called from external interface, it registers the strategy and starts it
    //Here Strategy is Registered
    void Template::bidDriver(void *params)
    {
        DEBUG_PRINT;
        API2::StrategyParameters *sgParams = (API2::StrategyParameters *)params;
        boost::shared_ptr<API2::SGContext> obj(new Template(sgParams));
        /**
      * @brief reqStartAlgo function call to Start the Strategy
      * @param marketDataEventRequired Set True to register for call-backs on Market-Data Event on registered Instruments [Required]
      * @param tradeTicksEventRequired Set True to register for call-backs on Trade Tick Events on registered Instruments [Required]
      * @param preTradeEventRequired Set True to register for call-backs for pre trade confirmations [Default is false]
      * @param isConvertToManualOrder Set True if you want your open order's to persist after termination of algo. Order will start behaving like manual order. [Default is false]
      * @param childConfirmationEvent Set True to register for call-backs for confirmations coming from child strategy [Default is false]
      * * @return
      */

        obj->reqStartAlgo(true, false);
        API2::SGContext::registerStrategy(obj);
        obj->reqTimerEvent(10000);
        DEBUG_MESSAGE(obj->reqQryDebugLog(), "Strategy Registered!");
    }

    //Set our Parameter Structure
    bool Template::setInternalParameters(API2::UserParams *customParams, FrontEndParameters &userParams)
    {
        DEBUG_PRINT;
        FILL_PARAMS("StgSymbolId", userParams.stgSymbolId);
        userParams.strategyID = customParams->getStrategyId();
        userParams.clientId = customParams->getClientId();
        dump(userParams);
        DEBUG_PRINT << "FrontEnd Inputs are => strategyID: " << userParams.strategyID << ", stgSymbolId:  " << userParams.stgSymbolId << ", client: " << userParams.clientId;
        return true;
    }
    //Strategy modify command received from FrontEnd
    //Here usually new parameters are saved and used from next strategy iteration (eg. bid / hedge cycle)
    void Template::onCMDModifyStrategy(API2::AbstractUserParams *newParams)
    {
        DEBUG_PRINT;
        if (_terminateCheck)
            return;

        API2::UserParams *customParams = (API2::UserParams *)newParams;

        if (!setModifiedInternalParameters(customParams, _modUserParams))
        {
            DEBUG_MESSAGE(reqQryDebugLog(), "Parameters not set from front end");
            terminateStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
            return;
        }
        if (!mapModParameters())
        {
            dump(_userParams);
            terminateStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
            return;
        }
        reqSendStrategyResponse(
            API2::CONSTANTS::RSP_ResponseType_STRATEGY_RUNNING,
            API2::CONSTANTS::RSP_RiskStatus_SUCCESS,
            API2::CONSTANTS::RSP_StrategyComment_USER_INPUT);
    }
    //set modified parameters to current frontend parameters
    bool Template::mapModParameters()
    {
        DEBUG_PRINT;
        return true;
    }

    //Set modified Internal parameters
    bool Template::setModifiedInternalParameters(API2::UserParams *customParams, FrontEndParameters &userParams)
    {
        DEBUG_PRINT;
        return true;
    }

    //Terminate strategy and cancel strategy's open orders
    void Template::terminateStrategyComment(API2::DATA_TYPES::StrategyComment comment)
    {
        DEBUG_PRINT;
        if (_terminateCheck)
            return;

        _terminateCheck = true;

        reqAddStrategyComment(comment);
        reqTerminateStrategy();
    }
    //dump function to flush our FrontEnd Values
    void Template::dump(FrontEndParameters &params)
    {
        DEBUG_PRINT;
        DEBUG_VARSHOW(reqQryDebugLog(), "StgSymbolId", params.stgSymbolId);
        // DEBUG_VARSHOW(reqQryDebugLog(), "Order Mode ", (API2::DATA_TYPES::OrderMode)params.orderMode);
    }

    //========================================= 39K Implementation =========================================

    void Template::initSetUp()
    {
        DEBUG_PRINT;
        //Todo check userParams.strategyID already running or not
        wsc::common::appConfigFilePath = "/root/work/uTrade-dev/src/templateAlgo/appConfig.ini";
        setAppConfig();
        createOrders();

        DEBUG_PRINT << "#SymbolId: " << _contract->getSymbolId() << ", instrument:  " << _contract->getStaticData()->scripName << ", strategyID: " << _userParams.strategyID << ", stgSymbolId: " << _userParams.stgSymbolId << ", clientId: " << _userParams.clientId << ", account: " << _userParams.account.getString();
        DEBUG_PRINT << "STG_SNAPSHOT,Timestamp,NetPos,GrossPnL,NetPnL,MidPrice,TSTQ,TSTV,TBTQ,TBTV,Contract,B/S,TradeQty,TradePrice,ExchOrderId,ExchTradeId,SentMsgCount,StrategyInputs,NoOfOrdersInBook,ActiveOrderBook,InternalOrderBook,BookSnapshotBid,BookSnapshotAsk,TicksCount,MsgSentCount,TickDiscardCount,ThrottlerErrorCount";
        logSnapshot();
    }

    void Template::setAppConfig()
    {
        DEBUG_PRINT;
        mINI::INIStructure appConfigIni;
        if (!mINI::INIFile(wsc::common::appConfigFilePath).read(appConfigIni))
            throw std::string("Invalid appConfigIni");
        auto appConfig = appConfigIni["APP"];
        auto stgConfig = appConfigIni["STG_" + std::to_string(_userParams.stgSymbolId)];

        // ToDo  load appConfig only once
        wsc::appConfig::tickToOrderLatencyFlag = boost::lexical_cast<bool>(appConfig["TICK_TO_ORDER_LATENCY_FLAG"]);
        wsc::appConfig::smConsumerInterval = boost::lexical_cast<int>(appConfig["SM_CONSUMER_INTERVAL"]) * MICRO_SECONDS_IN_SEC;
        wsc::appConfig::minValidObLevel = boost::lexical_cast<int>(appConfig["MIN_VALID_OB_LEVEL"]);

        _stgSymbolConfig.source = boost::lexical_cast<std::string>(stgConfig["SOURCE"]);
        _stgSymbolConfig.exchange = boost::lexical_cast<std::string>(stgConfig["EXCHANGE"]);
        _stgSymbolConfig.symbol = boost::lexical_cast<std::string>(stgConfig["SYMBOL"]);
        _stgSymbolConfig.expiary = boost::lexical_cast<std::string>(stgConfig["EXPIARY"]);
        _stgSymbolConfig.strikePrice = boost::lexical_cast<std::string>(stgConfig["STRIKE_PRICE"]);
        _stgSymbolConfig.optType = boost::lexical_cast<std::string>(stgConfig["OPT_TYPE"]);

        _contract = createNewInstrument(getSymbolID(_stgSymbolConfig.source, _stgSymbolConfig.exchange, _stgSymbolConfig.symbol, _stgSymbolConfig.expiary, _stgSymbolConfig.strikePrice, _stgSymbolConfig.optType), true, true, false, false, BOOK_SNAPSHOT_PRICE_LEVELS);
        _mktData = reqQryUpdateMarketData(_contract->getSymbolId());

        _strategyInput.maxPos = boost::lexical_cast<int>(stgConfig["MAX_POS"]) * _contract->getStaticData()->marketLot;

        _userParams.account.setPrimaryClientCode("PRO");
        _userParams.account.setTraderId(654987);
        _userParams.account.setLocationId(111111111111000);
        _userParams.account.setAccountType(1);
    }

    void Template::updateNetPosition()
    {

        // DEBUG_PRINT;
        auto pos = _contract->getPosition();
        _netPosition.totalBuyTradedQty = pos->getTradedQty(API2::CONSTANTS::CMD_OrderMode_BUY);
        _netPosition.totalBuyTradedValue = pos->getAmount(API2::CONSTANTS::CMD_OrderMode_BUY);
        _netPosition.totalSellTradedQty = pos->getTradedQty(API2::CONSTANTS::CMD_OrderMode_SELL);
        _netPosition.totalSellTradedValue = pos->getAmount(API2::CONSTANTS::CMD_OrderMode_SELL);
        _netPosition.netPositionQty = _netPosition.totalBuyTradedQty - _netPosition.totalSellTradedQty;
        // DEBUG_PRINT;

        // API2::PositionStruct pos1;
        // API2::SymbolIdAndPositionStructHash pos2, pos3, pos4;
        // getApiPositionBySymbolId(pos1, _contract->getSymbolId());
        // boost::unordered_set<API2::DATA_TYPES::SYMBOL_ID> id;
        // id.insert(_contract->getSymbolId());
        // getApiPositionForClient(pos2, _userParams.account.getPrimaryClientCode(), id);
        // getApiPositionForDealer(pos3, _userParams.clientId);
        // getApiPositionForDealerClient(pos4, _userParams.clientId, _userParams.account.getPrimaryClientCode());

        // std::stringstream ss;
        // ss << " SymbolId: " << pos1.getPositionString();
        // ss << " || Client: ";
        // for (auto &&i : pos2)
        // {
        //     ss << "  " << i.first << " - " << i.second.getPositionString();
        // }

        // ss << " || Dealer: ";
        // for (auto &&i : pos3)
        // {
        //     ss << "  " << i.first << " - " << i.second.getPositionString();
        // }

        // ss << " || DealerClient: ";
        // for (auto &&i : pos4)
        // {
        //     ss << "  " << i.first << " - " << i.second.getPositionString();
        // }

        // DEBUG_PRINT << ss.str() << " || _contract: " << _netPosition.netPositionQty
        //             << ", " << _netPosition.totalBuyTradedQty << ", " << _netPosition.totalBuyTradedValue << ", "
        //             << _netPosition.totalSellTradedQty << ", " << _netPosition.totalSellTradedValue << ", ";
    }

    void Template::updateBookSnapshot()
    {
        //  DEBUG_PRINT;
        _bookSnapshot.contractId = _mktData->getSymbolId();
        _bookSnapshot.timestamp = _mktData->getTimeStamp() > 0 ? _mktData->getTimeStamp() : wsc::Time::getSystemTimestamp();
        for (int i = 0; i < BOOK_SNAPSHOT_PRICE_LEVELS; ++i)
        {
            _bookSnapshot.bidPriceLevels[i].price = _mktData->getBidPrice(i);
            _bookSnapshot.bidPriceLevels[i].quantity = _mktData->getBidQty(i);
            // _bookSnapshot.bidPriceLevels[i].orderCount = _mktData->getNoOfBids(i);
            _bookSnapshot.askPriceLevels[i].price = _mktData->getAskPrice(i);
            _bookSnapshot.askPriceLevels[i].quantity = _mktData->getAskQty(i);
            // _bookSnapshot.askPriceLevels[i].orderCount = _mktData->getNoOfAsks(i);
        }
        //  DEBUG_PRINT;
    }

    API2::DATA_TYPES::SYMBOL_ID Template::getSymbolID(const std::string &source, const std::string &exchange, const std::string &symbol, const std::string &expiary, const std::string &strikePrice, const std::string &optType)
    {
        // [SOURCE] [EXCHANGE] [SYMBOL] [Expiry1(YYYYMMDD)] [Expiry2(YYYYMMDD)] [StrikePrice] [C/P(For Call/Put)]
        // DEBUG_PRINT << " " << reqQrySymbolID("DEFAULT NSECM RELIANCE");
        // DEBUG_PRINT << " " << reqQrySymbolID("DEFAULT NSEFO BANKNIFTY 20210429");
        // DEBUG_PRINT << " " << reqQrySymbolID("DEFAULT NSEFO BANKNIFTY 20210429 2740000 P");
        // DEBUG_PRINT << " " << reqQrySymbolID("DEFAULT NSEFO BANKNIFTY 20210429 2740000 C");
        // DEBUG_PRINT << " " << reqQrySymbolID("DEFAULT NSEFO BANKNIFTY 20210429 2740000 C");
        // DEBUG_PRINT << "++ " << reqQrySymbolID("DEFAULT NSECM SBIN");
        // DEBUG_PRINT << " " << reqQrySymbolID("DEFAULT NSEFO SBIN 20210429");
        // DEBUG_PRINT << " " << reqQrySymbolID("DEFAULT NSEFO SBIN 20210429 17000 P");
        // DEBUG_PRINT << " " << reqQrySymbolID("DEFAULT NSEFO SBIN 20210429 17000 P");
        // DEBUG_PRINT << " " << reqQrySymbolID("DEFAULT ESMNSE SBIN 20210429 17000 P");
        // DEBUG_PRINT << " " << reqQrySymbolID("DEFAULT ESMNSE SBIN EQ");

        // DEFAULT ESMNSE SBIN

        // DEBUG_PRINT << source << "  " << exchange << "  " << symbol << "  " << expiary << "  " << strikePrice << "  " << optType;

        std::string symbolName = source + " " + exchange + " " + symbol;
        if (expiary.length() > 0)
            symbolName += " " + expiary;
        if (strikePrice.length() > 0)
            symbolName += " " + strikePrice;
        if (optType.length() > 0)
            symbolName += " " + optType;
        DEBUG_PRINT << reqQrySymbolID(symbolName) << "  " << symbolName;
        return reqQrySymbolID(symbolName);
    }

    void Template::onBookSnapshot(UNSIGNED_LONG symbolId)
    {
        DEBUG_PRINT;
        if (wsc::appConfig::tickToOrderLatencyFlag)
            _scopeLatency = wsc::Time::getSystemTimestamp();
        updateBookSnapshot();
        updateNetPosition();
        wsc::Time::setTimestampUnsafeForLive(_bookSnapshot.timestamp);

        if (!isValidBookSnapshot())
        {
            return;
        }

        for (size_t i = 0; i < _ordersPoolSize; i++)
        {
            _internalBuyOrderBook[i].reset();
            _internalSellOrderBook[i].reset();
        }
        // if (!_isRunning)
        // {
        //     // Square off  existing positions
        //     if (_netPosition.netPositionQty > 0)
        //     {
        //         _internalSellOrderBook[1].price = _bookSnapshot.bidPriceLevels[2].price;
        //         _internalSellOrderBook[1].qty = _netPosition.netPositionQty;
        //     }
        //     else if (_netPosition.netPositionQty < 0)
        //     {
        //         _internalBuyOrderBook[1].price = _bookSnapshot.askPriceLevels[2].price;
        //         _internalBuyOrderBook[1].qty = std::abs(_netPosition.netPositionQty);
        //     }
        // }
        // else
        {
            int _buyQty = std::max(std::min(_strategyInput.maxPos, _strategyInput.maxPos - _netPosition.netPositionQty), 0);
            int _sellQty = std::min(std::max(-_strategyInput.maxPos, -_strategyInput.maxPos - _netPosition.netPositionQty), 0);

            // Creating New position
            if (_buyQty > 0)
            {
                _internalBuyOrderBook[0].price = _bookSnapshot.bidPriceLevels[2].price;
                _internalBuyOrderBook[0].qty = _buyQty;
            }
            if (_sellQty < 0)
            {
                _internalSellOrderBook[0].price = _bookSnapshot.askPriceLevels[2].price;
                _internalSellOrderBook[0].qty = std::abs(_sellQty);
            }
            // Square off  existing positions
            if (_netPosition.netPositionQty > 0)
            {
                _internalSellOrderBook[1].price = _bookSnapshot.askPriceLevels[2].price;
                _internalSellOrderBook[1].qty = _netPosition.netPositionQty;
            }
            else if (_netPosition.netPositionQty < 0)
            {
                _internalBuyOrderBook[1].price = _bookSnapshot.bidPriceLevels[2].price;
                _internalBuyOrderBook[1].qty = std::abs(_netPosition.netPositionQty);
            }
        }

        orderManager();
        if (wsc::appConfig::tickToOrderLatencyFlag)
            std::cout << (wsc::Time::getSystemTimestamp() - _scopeLatency) << std::endl;
    }

    void Template::createOrders()
    {
        DEBUG_PRINT;
        _ordersPoolSize = 2;
        for (int i = 0; i < _ordersPoolSize; i++)
        {
            _buyOrderBook.push_back(
                API2::COMMON::OrderWrapper(
                    _contract,
                    API2::CONSTANTS::CMD_OrderMode_BUY,
                    this,
                    _userParams.account,
                    API2::CONSTANTS::CMD_OrderType_LIMIT));
            _sellOrderBook.push_back(
                API2::COMMON::OrderWrapper(
                    _contract,
                    API2::CONSTANTS::CMD_OrderMode_SELL,
                    this,
                    _userParams.account,
                    API2::CONSTANTS::CMD_OrderType_LIMIT));
            _internalBuyOrderBook.push_back(wsc::OrderDetails{API2::CONSTANTS::CMD_OrderMode_BUY, 0, 0});
            _internalSellOrderBook.push_back(wsc::OrderDetails{API2::CONSTANTS::CMD_OrderMode_SELL, 0, 0});
            _buyOrderBook[i].reset();
            _sellOrderBook[i].reset();
        }
    }

    bool Template::isValidBookSnapshot()
    {
        // DEBUG_PRINT;
        bool isValid = true;
        int maxLoop = std::min(wsc::appConfig::minValidObLevel, BOOK_SNAPSHOT_PRICE_LEVELS);
        int i = 0;
        for (i = 0; i < maxLoop; ++i)
        {
            if (_bookSnapshot.bidPriceLevels[i].price <= 0 || _bookSnapshot.askPriceLevels[i].price <= 0)
            {
                isValid = false;
                break;
            }
        }
        return isValid;
    }

    void Template ::orderManager()
    {
        // DEBUG_PRINT;
        _lastMsgSentCount = _msgSentCount;
        // BUY Side
        for (int i = 0; i < _ordersPoolSize; i++)
        {
            auto &_order = _buyOrderBook[i];
            auto _internalOrder = _internalBuyOrderBook[i];
            //If Order present in InternalBook
            if (_internalOrder.qty > 0)
            {
                //If Order Not present in Book then send new Order
                if (_order._isReset)
                {
                    if (_order.newOrder(_riskStatus, _internalOrder.price, _internalOrder.qty))
                    {
                        ++_msgSentCount;
                    }
                }
                //If Order  present in Book then check for Modify Order
                else if (_order.getLastQuantity())
                {
                    if ((_order._lastQuotedPrice != _internalOrder.price && abs(_order._lastQuotedPrice - _internalOrder.price) >= _minPriceDiff) || _order._lastQuantity != _internalOrder.qty)
                    {
                        if (_order.replaceOrder(_riskStatus, _internalOrder.price, _internalOrder.qty))
                        {
                            ++_msgSentCount;
                        }
                    }
                }
            }
            //If Order Not present in InternalBook
            else if (_order.getLastQuantity())
            {
                if (_order.cancelOrder(_riskStatus))
                {
                    ++_msgSentCount;
                }
            }
        }

        // SELL Side
        for (int i = 0; i < _ordersPoolSize; i++)
        {
            auto &_order = _sellOrderBook[i];
            auto _internalOrder = _internalSellOrderBook[i];
            //If Order present in InternalBook
            if (_internalOrder.qty > 0)
            {
                //If Order Not present in Book then send new Order
                if (_order._isReset)
                {
                    if (_order.newOrder(_riskStatus, _internalOrder.price, _internalOrder.qty))
                    {
                        ++_msgSentCount;
                    }
                }
                //If Order  present in Book then check for Modify Order
                else if (_order.getLastQuantity())
                {
                    if ((_order._lastQuotedPrice != _internalOrder.price && abs(_order._lastQuotedPrice - _internalOrder.price) >= _minPriceDiff) || _order._lastQuantity != _internalOrder.qty)
                    {
                        if (_order.replaceOrder(_riskStatus, _internalOrder.price, _internalOrder.qty))
                        {
                            ++_msgSentCount;
                        }
                    }
                }
            }
            //If Order Not present in InternalBook
            else if (_order.getLastQuantity())
            {
                if (_order.cancelOrder(_riskStatus))
                {
                    ++_msgSentCount;
                }
            }
        }

        // if (_lastMsgSentCount != _msgSentCount)
        //     logPosition(nullptr);
    }

    void Template::logSnapshot()
    {
        _grossPnL = (_netPosition.totalSellTradedValue - _netPosition.totalBuyTradedValue) + (_netPosition.netPositionQty * _midPrice);

        _netPnL = _grossPnL;

        std::stringstream ss;
        ss << "STG_SNAPSHOT,";
        wsc::Time::printTimestamp(ss, _bookSnapshot.timestamp);
        ss << "," << _netPosition.netPositionQty << "," << _grossPnL << "," << _netPnL << "," << _midPrice
           << "," << _netPosition.totalSellTradedQty << "," << _netPosition.totalSellTradedValue
           << "," << _netPosition.totalBuyTradedQty << "," << _netPosition.totalBuyTradedValue
           << "," << _contract->getStaticData()->scripName;
        ss << ",,,,,";
        ss << "," << _msgSentCount << "," << (_strategyInput.maxPos / _contract->getStaticData()->marketLot)
           << "," << _ordersPoolSize << ",\"[  ";

        for (size_t i = 0; i < _ordersPoolSize; i++)
        {
            ss
                << " { "
                << getOrderStr(_buyOrderBook[i])
                << " } , {"
                << getOrderStr(_sellOrderBook[i])
                << " } ,";
        }
        ss.seekp(-1, ss.cur);
        ss << " ]\",\"[  ";
        for (size_t i = 0; i < _ordersPoolSize; i++)
        {
            ss
                << " {"
                << " \"\"BuySellType\"\": \"\"" << wsc::BuySellTypeStr(_internalBuyOrderBook[i].buySell)
                << "\"\", \"\"Price\"\": " << _internalBuyOrderBook[i].price
                << ", \"\"Qty\"\": " << _internalBuyOrderBook[i].qty
                << "} ,  {"
                << " \"\"BuySellType\"\": \"\"" << wsc::BuySellTypeStr(_internalSellOrderBook[i].buySell)
                << "\"\", \"\"Price\"\": " << _internalSellOrderBook[i].price
                << ", \"\"Qty\"\": " << _internalSellOrderBook[i].qty
                << " } ,";
        }
        ss.seekp(-1, ss.cur);
        ss << " ]\",\"[  ";

        for (int i = BOOK_SNAPSHOT_PRICE_LEVELS - 1; i >= 0; i--)
            ss << " { "
               << "\"\"BP\"\": " << _bookSnapshot.bidPriceLevels[i].price
               << ", \"\"BQ\"\": " << _bookSnapshot.bidPriceLevels[i].quantity
               << "} ,";
        ss.seekp(-1, ss.cur);
        ss << " ]\",\"[  ";

        for (int i = 0; i < BOOK_SNAPSHOT_PRICE_LEVELS; i++)
            ss << " { "
               << "\"\"BP\"\": " << _bookSnapshot.askPriceLevels[i].price
               << ", \"\"BQ\"\": " << _bookSnapshot.askPriceLevels[i].quantity
               << "} ,";
        ss.seekp(-1, ss.cur);
        ss << " ]\"  ";
        DEBUG_PRINT << ss.str();
    }

    const std::string Template::getOrderStr(const API2::COMMON::OrderWrapper &order)
    {
        std::stringstream ss;
        ss << "\"\", \"\"BuySellType\"\": \"\"" << wsc::BuySellTypeStr(order._mode)
           << "\"\", \"\"ContractName\"\": \"\"" << order._instrument->getStaticData()->scripName
           << "\"\", \"\"OrderType\"\": \"\"" << order._orderType
           << "\"\", \"\"ExchOrderId\"\": " << order._exchangeOrderId
           << ", \"\"IsReset\"\": \"\"" << order._isReset
           << "\"\", \"\"Price\"\": " << order._price
           << "\"\", \"\"LastQuotedPrice\"\": " << order._lastQuotedPrice
           << ", \"\"Quantity\"\": " << order._lastQuantity
           << ", \"\"QuantityTraded\"\": " << order._lastFilledQuantity
           << "\"\"";
        return ss.str();
    }

    void Template::orderResHandler(API2::OrderConfirmation &confirmation, API2::COMMON::OrderId *orderId)
    {
        DEBUG_PRINT
            << " BuySellType: " << wsc::BuySellTypeStr(confirmation.getOrderMode())
            << ", ContractName: " << confirmation.getSymbolId()
            << ", OrderType: " << (int16_t)confirmation.getOrderType()
            << ", ExchOrderId: " << confirmation.getExchangeOrderId()
            << ", OrderStatus: " << confirmation.getOrderStatus()
            << ", OrigOrderPrice: " << confirmation.getOrigOrderPrice()
            << ", OrderPrice: " << confirmation.getOrderPrice()
            << ", Quantity: " << confirmation.getOrderQuantity()
            << ", OrigLastFillPrice: " << confirmation.getOrigLastFillPrice()
            << ", LastFillPrice: " << confirmation.getLastFillPrice()
            << ", LastFillQuantity: " << confirmation.getLastFillQuantity();
        for (int op = 0; op < _ordersPoolSize; ++op)
        {
            DEBUG_PRINT << " orderId: " << orderId << " - " << _buyOrderBook[op]._orderId << " - " << _sellOrderBook[op]._orderId;
            if (_buyOrderBook[op]._orderId == orderId)
            {
                DEBUG_PRINT;
                if (!processConfirmation(_buyOrderBook[op], confirmation, orderId))
                {
                    // DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
                    DEBUG_PRINT << "ProcessConfirmation Failed";
                }
                break;
            }
            else if (_sellOrderBook[op]._orderId == orderId)
            {
                DEBUG_PRINT;
                if (!processConfirmation(_sellOrderBook[op], confirmation, orderId))
                {
                    // DEBUG_MESSAGE(reqQryDebugLog(), "Process Confirmation Failed");
                    DEBUG_PRINT << "ProcessConfirmation Failed";
                }
                break;
            }
        }
        logSnapshot();
    }

}

#pragma once
#include <apiDataTypes.h>
#include <apiConstants.h>
#include <iostream>
#include <sstream>
#include <vector>
#include "sysZTime.h"

#define MICRO_SECONDS_IN_MINUTE 60000000L
#define MILI_SECONDS_IN_SEC 1000L
#define MICRO_SECONDS_IN_SEC 1000000L
#define NANO_SECONDS_IN_SEC 1000000000LL
#define NANO_SECONDS_IN_MILI_SEC 1000000LL
#define NANO_SECONDS_IN_MICRO_SEC 1000LL
#define NANO_SECONDS_IN_24_HOURS 86400000000000LL

struct X
{
    ~X() { std::cout << std::endl; }
};
#define AT std::string(__FILE__) + ":" + std::to_string(__LINE__) + ", " + std::string(__FUNCTION__) + " "
#define DEBUG_PRINT (X(), std::cout << AT << " | ")


namespace wsc
{

    struct BookPriceLevel
    {
        int price;
        int quantity;
        int orderCount;
    };

    struct BookSnapshot
    {

        int64_t timestamp;
        int64_t contractId;
#define BOOK_SNAPSHOT_PRICE_LEVELS 20
        BookPriceLevel bidPriceLevels[BOOK_SNAPSHOT_PRICE_LEVELS];
        BookPriceLevel askPriceLevels[BOOK_SNAPSHOT_PRICE_LEVELS];
    };

    struct NetPositionDetails
    {
        int64_t contractId = 0;
        int netPositionQty = 0;
        int64_t totalBuyTradedValue = 0;
        int64_t totalBuyTradedQty = 0;
        int64_t totalSellTradedValue = 0;
        int64_t totalSellTradedQty = 0;
    };

    struct StgSymbolConfig
    {
        std::string source = "";
        std::string exchange = "";
        std::string symbol = "";
        std::string expiary = "";
        std::string strikePrice = "";
        std::string optType = "";
    };

    inline std::vector<std::string> splitStr(const std::string &str, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;
        while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos)
        {
            token = str.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }
        res.push_back(str.substr(pos_start));
        return res;
    }

    template <typename T>
    bool tryAnyCast(const std::string &s, T &t)
    {
        std::stringstream ss(s);
        T result;
        if ((ss >> t).fail() || !(ss >> std::ws).eof())
            return false;
        return true;
    }

    static const std::string getTimeStampStr(int64_t ts)
    {
        std::stringstream ss;
        wsc::Time::printTimestamp(ss, ts);
        return ss.str();
    }

    struct OrderDetails
    {
        API2::DATA_TYPES::OrderMode buySell;
        int price = 0;
        int qty = 0;
        void reset()
        {
            price = 0;
            qty = 0;
        }
        OrderDetails(API2::DATA_TYPES::OrderMode _buySell, int _price, int _qty) : buySell(_buySell), price(_price), qty(_qty) {}
    };

    inline const char *BuySellTypeStr(API2::DATA_TYPES::OrderMode v)
    {
        switch (v)
        {
        case API2::CONSTANTS::CMD_OrderMode_BUY:
            return "BUY";
        case API2::CONSTANTS::CMD_OrderMode_SELL:
            return "SELL";
        default:
            return "[Unknown BuySellType]";
        }
    }

    //*************
    struct MyPriceLevel
    {
        int price = 0;
        // int quantity = 0;
        long long value = 0;
        int orderCount = 0;
        double spread = 0;
        double probability = 0;
        double spreadProbability = 0;
    };

    struct StgConfig
    {
        int maxPos = -1;
    };

    enum class ExchangeSegment
    {
        NSECM = 10, // Equity. Cash Market.
        NSEFO,      // Equity Derivative.
        NSECD,      // Currency Derivative.
        NSECOM,     // Commodity Derivative.
        BSECM = 20,
        BSEFO,
        BSECD,
        BSECOM,
    };

}
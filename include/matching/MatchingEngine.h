#pragma once

#include <vector>

#include "exchange/Order.h"
#include "exchange/Trade.h"
#include "orderbook/LimitOrderBook.h"

class MatchingEngine
{
public:
    void processOrder(Order order);

    const std::vector<Trade>& trades() const { return trades_; }
    const LimitOrderBook& orderBook() const { return book; }

private:
    LimitOrderBook book;
    std::vector<Trade> trades_;
};

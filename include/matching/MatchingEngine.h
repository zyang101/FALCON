#pragma once

#include "exchange/Order.h"
#include "orderbook/LimitOrderBook.h"

class MatchingEngine
{
public:
    void processOrder(const Order& order);

private:
    LimitOrderBook book;
};
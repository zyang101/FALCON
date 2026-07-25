#pragma once

#include <deque>

#include "exchange/Order.h"

#include <list>

struct PriceLevel
{
    Price price{};
    Quantity totalQuantity{0};

    std::list<Order> orders;
};
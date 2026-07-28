#pragma once

#include <list>

#include "common/Types.h"

struct Order
{
    OrderId id;

    Side side;
    OrderType type;

    Price price;

    Quantity originalQuantity;
    Quantity remainingQuantity;

    Timestamp timestamp;
};

struct OrderLocation
{
    Side side;
    Price price;
    std::list<Order>::iterator orderIterator;
};
#pragma once

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
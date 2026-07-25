#pragma once

#include "common/Types.h"

struct Trade
{
    OrderId buyOrderId;
    OrderId sellOrderId;

    Price price;
    Quantity quantity;

    Timestamp timestamp;
};
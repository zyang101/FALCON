#pragma once

#include <deque>

#include "exchange/Order.h"

struct PriceLevel
{
    Price price;

    std::deque<Order> orders;
};
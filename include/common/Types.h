#pragma once

#include <cstdint>

using OrderId = uint64_t;
using Price = int64_t;
using Quantity = uint32_t;
using Timestamp = uint64_t;

enum class Side
{
    Buy,
    Sell
};

enum class OrderType
{
    Limit,
    Market
};
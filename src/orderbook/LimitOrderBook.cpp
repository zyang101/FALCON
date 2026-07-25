#include "orderbook/LimitOrderBook.h"



void LimitOrderBook::addOrder(const Order& order)
{
    if (order.side == Side::Buy)
    {
        auto& level = buyLevels[order.price];
        level.price = order.price;
        level.orders.push_back(order);
        level.totalQuantity += order.remainingQuantity;
        orderLookup[order.id] = OrderLocation   {
            order.side,
            order.price,
            std::prev(level.orders.end())
        };
    }   
    else
    {
        auto& level = sellLevels[order.price];
        level.price = order.price;
        level.orders.push_back(order);
        level.totalQuantity += order.remainingQuantity;
            orderLookup[order.id] = OrderLocation   {
            order.side,
            order.price,
            std::prev(level.orders.end())
        };
    }
    
}

void LimitOrderBook::cancelOrder(OrderId id)
{
    // TODO: Implement after adding order lookup table.
    // auto it = orderLookup.find(id)
    // level.order.pop(location)
}

std::optional<Price> LimitOrderBook::bestBid() const
{
    if (buyLevels.empty())
    {
        return std::nullopt;
    }
    auto it = buyLevels.begin();
    return it->first;
}

std::optional<Price> LimitOrderBook::bestAsk() const
{
    if (sellLevels.empty())
    {
        return std::nullopt;
    }
    auto it = sellLevels.begin();
    return it->first;
}
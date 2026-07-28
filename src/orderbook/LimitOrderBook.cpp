#include "orderbook/LimitOrderBook.h"

#include <iterator>

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

/* The highest price someone is willing to buy at*/
std::optional<Price> LimitOrderBook::bestBid() const
{
    if (buyLevels.empty())
    {
        return std::nullopt;
    }
    auto it = buyLevels.begin();
    return it->first;
}

/* The lowest price someone is willing to sell at*/
std::optional<Price> LimitOrderBook::bestAsk() const
{
    if (sellLevels.empty())
    {
        return std::nullopt;
    }
    auto it = sellLevels.begin();
    return it->first;
}

const Order* LimitOrderBook::bestBidOrder() const
{
    if (buyLevels.empty())
    {
        return nullptr;
    }
    auto it = buyLevels.begin();
    if (it->second.orders.empty())
    {
        return nullptr;
    }
    return &(*it->second.orders.begin());
}

const Order* LimitOrderBook::bestAskOrder() const
{
    if (sellLevels.empty())
    {
        return nullptr;
    }
    auto it = sellLevels.begin();
    if (it->second.orders.empty())
    {
        return nullptr;
    }
    return &(*it->second.orders.begin());
}
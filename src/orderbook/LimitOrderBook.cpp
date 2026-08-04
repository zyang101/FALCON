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
        orderLookup[order.id] = OrderLocation{
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
        orderLookup[order.id] = OrderLocation{
            order.side,
            order.price,
            std::prev(level.orders.end())
        };
    }
}

void LimitOrderBook::cancelOrder(OrderId id)
{
    auto lookupIt = orderLookup.find(id);
    if (lookupIt == orderLookup.end())
    {
        return;
    }

    OrderLocation& location = lookupIt->second;
    Order& order = *location.orderIterator;
    Quantity quantity = order.remainingQuantity;
    
    if (location.side == Side::Buy)
    {
        auto levelIt = buyLevels.find(location.price);
        if (levelIt == buyLevels.end())
            return;
        PriceLevel& level = levelIt->second;
        level.totalQuantity -= quantity;
        level.orders.erase(location.orderIterator);
        orderLookup.erase(lookupIt);

        if (level.orders.empty())
            buyLevels.erase(levelIt);
    }
    else
    {
        auto levelIt = sellLevels.find(location.price);
        if (levelIt == sellLevels.end())
            return;
        PriceLevel& level = levelIt->second;
        level.totalQuantity -= quantity;
        level.orders.erase(location.orderIterator);
        orderLookup.erase(lookupIt);

        if (level.orders.empty())
            sellLevels.erase(levelIt);
    }
}

void LimitOrderBook::fillOrder(OrderId id, Quantity quantity)
{
    auto lookupIt = orderLookup.find(id);
    if (lookupIt == orderLookup.end())
    {
        return;
    }

    OrderLocation& location = lookupIt->second;
    Order& order = *location.orderIterator;

    if (quantity > order.remainingQuantity)
    {
        quantity = order.remainingQuantity;
    }

    order.remainingQuantity -= quantity;

    if (location.side == Side::Buy)
    {
        auto levelIt = buyLevels.find(location.price);
        if (levelIt == buyLevels.end())
        {
            return;
        }

        PriceLevel& level = levelIt->second;
        level.totalQuantity -= quantity;

        if (order.remainingQuantity == 0)
        {
            level.orders.erase(location.orderIterator);
            orderLookup.erase(lookupIt);

            if (level.orders.empty())
            {
                buyLevels.erase(levelIt);
            }
        }
    }
    else
    {
        auto levelIt = sellLevels.find(location.price);
        if (levelIt == sellLevels.end())
        {
            return;
        }

        PriceLevel& level = levelIt->second;
        level.totalQuantity -= quantity;

        if (order.remainingQuantity == 0)
        {
            level.orders.erase(location.orderIterator);
            orderLookup.erase(lookupIt);

            if (level.orders.empty())
            {
                sellLevels.erase(levelIt);
            }
        }
    }
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

Order* LimitOrderBook::bestBidOrder()
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

Order* LimitOrderBook::bestAskOrder()
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

const Order* LimitOrderBook::bestBidOrder() const
{
    return const_cast<LimitOrderBook*>(this)->bestBidOrder();
}

const Order* LimitOrderBook::bestAskOrder() const
{
    return const_cast<LimitOrderBook*>(this)->bestAskOrder();
}

/* From my research stock exchanges prefer to have the priority be implemented with a Tree, buy fifo queue for buy tree and sell fifo for sell tree. Then a map for order cancelations. (Near O(1))

Helpful Diagram:
            LIMIT ORDER BOOK

            +----------------+
            | LimitOrderBook |
            +----------------+
            /              \
           /                \
          /                  \
    Buy Side Tree            Sell Side Tree
(highest -> lowest)      (lowest -> highest)

    std::map                std::map
Price -> PriceLevel     Price -> PriceLevel
        |                       |
        |                       |
+-------------+         +-------------+
| PriceLevel  |         | PriceLevel  |
+-------------+         +-------------+
| Price       |         | Price       |
| FIFO Queue  |         | FIFO Queue  |
+-------------+         +-------------+
        |                       |
        v                       v

Order -> Order -> Order   Order -> Order


            Order Lookup Table

std::unordered_map<OrderId, Order*>

    Order ID  --------->  Order
*/


#pragma once

#include <map>
#include <optional>
#include <unordered_map>

#include "exchange/Order.h"

class LimitOrderBook
{
public:

    void addOrder(const Order& order);

    void cancelOrder(OrderId id);

    std::optional<Price> bestBid() const;

    std::optional<Price> bestAsk() const;

    const Order* bestBidOrder() const;
    
    const Order* bestAskOrder() const;

private:

    std::map<Price, PriceLevel, std::greater<Price>> buyLevels;

    std::map<Price, PriceLevel> sellLevels;

    std::unordered_map<OrderId, OrderLocation> orderLookup;
};
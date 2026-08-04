#include "matching/MatchingEngine.h"

#include <algorithm>

#include "common/Time.h"

/*
processOrder(order)

↓

While incoming quantity remains and the market is crossed:
    Trade against the best opposite order (resting price)
    Apply fill to resting order
    Reduce incoming remaining quantity

If leftover quantity and order is a Limit:
    Rest it on the book
*/
void MatchingEngine::processOrder(Order order)
{
    if (order.side == Side::Buy)
    {
        while (order.remainingQuantity > 0)
        {
            auto bestAsk = book.bestAsk();
            if (!bestAsk || order.price < *bestAsk)
            {
                break;
            }

            Order* restingOrder = book.bestAskOrder();
            if (!restingOrder)
            {
                break;
            }

            Quantity tradeQuantity = std::min(
                order.remainingQuantity,
                restingOrder->remainingQuantity);

            trades_.push_back(Trade{
                order.id,
                restingOrder->id,
                restingOrder->price,
                tradeQuantity,
                currentTimestamp()
            });

            book.fillOrder(restingOrder->id, tradeQuantity);
            order.remainingQuantity -= tradeQuantity;
        }
    }
    else
    {
        while (order.remainingQuantity > 0)
        {
            auto bestBid = book.bestBid();
            if (!bestBid || order.price > *bestBid)
            {
                break;
            }

            Order* restingOrder = book.bestBidOrder();
            if (!restingOrder)
            {
                break;
            }

            Quantity tradeQuantity = std::min(
                order.remainingQuantity,
                restingOrder->remainingQuantity);

            trades_.push_back(Trade{
                restingOrder->id,
                order.id,
                restingOrder->price,
                tradeQuantity,
                currentTimestamp()
            });

            book.fillOrder(restingOrder->id, tradeQuantity);
            order.remainingQuantity -= tradeQuantity;
        }
    }

    if (order.remainingQuantity > 0 && order.type == OrderType::Limit)
    {
        book.addOrder(order);
    }
}

/*
cancelOrder(id)

↓

Forward to LimitOrderBook
    lookup order by id
    remove full remaining quantity from its price level
    erase from FIFO + lookup
    drop empty price level if needed
*/
void MatchingEngine::cancelOrder(OrderId id)
{
    book.cancelOrder(id);
}
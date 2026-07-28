#include "matching/MatchingEngine.h"

#include <algorithm>

#include "common/Time.h"
#include "exchange/Trade.h"

/* processOrder(order)

↓

If order is BUY

    If there is no ask
        Add order to book

    Else if buy price < best ask
        Add order to book

    Else
        Trade with the best ask

----------------------------

If order is SELL

    If there is no bid
        Add order to book

    Else if sell price > best bid
        Add order to book

    Else
        Trade with the best bid
*/
void MatchingEngine::processOrder(const Order& order)
{
    if (order.side == Side::Buy)
    {
        auto bestAsk = book.bestAsk();

        if (!bestAsk)
        {
            book.addOrder(order);
            return;
        }

        if (order.price < *bestAsk)
        {
            book.addOrder(order);
            return;
        }
    
        const Order* restingOrder = book.bestAskOrder();
        if (!restingOrder)
        {
            return;
        }
        // Create Trade entry
        Quantity tradeQuantity = std::min(order.remainingQuantity, restingOrder->remainingQuantity);

        Trade trade
        {
            order.id,
            restingOrder->id,
            restingOrder->price,
            tradeQuantity,
            currentTimestamp()
        };
    }
    else
    {
        auto bestBid = book.bestBid();

        if (!bestBid)
        {
            book.addOrder(order);
            return;
        }

        if (order.price > *bestBid)
        {
            book.addOrder(order);
            return;
        }

        // Create Trade Entry
        const Order* restingOrder = book.bestBidOrder();
        if (!restingOrder)
        {
            return;
        }
        Quantity tradeQuantity = std::min(order.remainingQuantity, restingOrder->remainingQuantity);

        Trade trade{
            restingOrder->id,
            order.id,
            restingOrder->price,
            tradeQuantity,
            currentTimestamp()
        };

    }
}
#include "matching/MatchingEngine.h"

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

        if (order.price < bestAsk.value())
        {
            book.addOrder(order);
            return;
        }

        // TODO: Execute trade
    }
    else
    {
        auto bestBid = book.bestBid();

        if (!bestBid)
        {
            book.addOrder(order);
            return;
        }

        if (order.price > bestBid.value())
        {
            book.addOrder(order);
            return;
        }

        // TODO: Execute trade
    }
}
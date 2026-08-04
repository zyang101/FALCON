/*
================================================================================
 FALCON Matching System
================================================================================

                        +------------------+
   Incoming Order ----> | MatchingEngine   |
   Cancel (by id) ----> |  processOrder()  |
                        |  cancelOrder()   |
                        +--------+---------+
                                 |
                                 | owns
                 +---------------+----------------+
                 |                                |
                 v                                v
      +--------------------+           +--------------------+
      |  LimitOrderBook    |           |  vector<Trade>     |
      |  (resting orders)  |           |  (executions)      |
      +----------+---------+           +--------------------+
                 |
        +--------+--------+
        |                 |
        v                 v
   Buy Side Map      Sell Side Map
   (best bid first)  (best ask first)
        |                 |
        v                 v
   PriceLevel        PriceLevel
   FIFO list         FIFO list
        |
        +-- orderLookup: OrderId -> OrderLocation  (O(1) fill/cancel)


 processOrder(order) flow
 ------------------------

   Incoming Order (working copy)
            |
            v
   +--------------------+
   | remainingQty > 0   |
   | AND market crossed |---- no ----+
   +---------+----------+            |
             | yes                   |
             v                       |
   Get best opposite order           |
   (bestAsk / bestBid)               |
             |                       |
             v                       |
   qty = min(incoming, resting)      |
             |                       |
             v                       |
   Record Trade                      |
   (buyerId, sellerId,               |
    resting price, qty)              |
             |                       |
             v                       |
   book.fillOrder(resting, qty)      |
   incoming.remaining -= qty         |
             |                       |
             +------ loop -----------+
                                     |
                                     v
                      leftover AND Limit?
                           /    \
                         yes     no
                          |       |
                          v       v
                     addOrder   done
                     (rest it)


 Crossed market?
 ---------------
   BUY  crosses when: best ask exists AND buy.price  >= bestAsk
   SELL crosses when: best bid exists AND sell.price <= bestBid

 Trade rules
 -----------
   Price  = resting order's price  (time priority)
   Buyer  = buy-side order id      (incoming if buy, resting if sell)
   Seller = sell-side order id     (resting if buy, incoming if sell)

 fillOrder(id, qty)
 ------------------
   lookup id -> OrderLocation
        |
        v
   remainingQuantity -= qty
   level.totalQuantity -= qty
        |
        +-- if fully filled: erase from FIFO + lookup
        +-- if level empty:  erase price from map

 cancelOrder(id)
 ---------------
   Incoming cancel ----> MatchingEngine::cancelOrder(id)
                                |
                                v
                       book.cancelOrder(id)
                                |
                                v
   lookup id -> OrderLocation  (missing? done)
                                |
                                v
   Remove full remaining qty from level.totalQuantity
   Erase order from FIFO list
   Erase id from orderLookup
                                |
                                v
   If price level empty -> erase price from buy/sell map

   Notes:
   - Cancel only affects resting orders (already on the book)
   - Unknown / already-filled ids are a no-op
   - Does not create a Trade
================================================================================
*/

#pragma once

#include <vector>

#include "exchange/Order.h"
#include "exchange/Trade.h"
#include "orderbook/LimitOrderBook.h"

class MatchingEngine
{
public:
    void processOrder(Order order);
    void cancelOrder(OrderId id);

    const std::vector<Trade>& trades() const { return trades_; }
    const LimitOrderBook& orderBook() const { return book; }

private:
    LimitOrderBook book;
    std::vector<Trade> trades_;
};

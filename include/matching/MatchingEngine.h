/*
================================================================================
 FALCON Matching System
================================================================================

                        +------------------+
   Incoming Order ----> | MatchingEngine   |
                        |  processOrder()  |
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

    const std::vector<Trade>& trades() const { return trades_; }
    const LimitOrderBook& orderBook() const { return book; }

private:
    LimitOrderBook book;
    std::vector<Trade> trades_;
};

#include <gtest/gtest.h>

#include "matching/MatchingEngine.h"

// Write your matching-engine tests here.
// Suggested cases to cover:
//   - Rest buy/sell on an empty book
//   - No trade when prices do not cross
//   - Full fill
//   - Partial fill (incoming rests / resting remains)
//   - Multi-level sweep
//   - Sell aggressor: buyer/seller IDs and resting price
//   - FIFO within the same price level

namespace {

Order makeLimit(OrderId id, Side side, Price price, Quantity quantity)
{
    return Order{
        id,
        side,
        OrderType::Limit,
        price,
        quantity,
        quantity,
        0
    };
}

} // namespace

// Example skeleton (delete or replace):
//
// TEST(MatchingEngine, RestsBuyWhenBookEmpty)
// {
//     MatchingEngine engine;
//     engine.processOrder(makeLimit(1, Side::Buy, 100, 50));
//
//     EXPECT_TRUE(engine.trades().empty());
//     // ...
// }

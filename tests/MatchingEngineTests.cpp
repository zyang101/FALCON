#include <gtest/gtest.h>

#include "matching/MatchingEngine.h"

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

}

// If no trade is made, resting buy should remain in the book.
TEST(MatchingEngine, RestsBuyWhenBookEmpty)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Buy, 100, 50));

    EXPECT_TRUE(engine.trades().empty());
    ASSERT_TRUE(engine.orderBook().bestBid().has_value());
    EXPECT_EQ(*engine.orderBook().bestBid(), 100);
    EXPECT_FALSE(engine.orderBook().bestAsk().has_value());
}

// If no trade is made, resting sell should remain in the book.
TEST(MatchingEngine, RestsSellWhenBookEmpty)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Sell, 100, 50));

    EXPECT_TRUE(engine.trades().empty());
    ASSERT_TRUE(engine.orderBook().bestAsk().has_value());
    EXPECT_EQ(*engine.orderBook().bestAsk(), 100);
    EXPECT_FALSE(engine.orderBook().bestBid().has_value());
}

// If buy does not cross, no trade is made and resting sell should remain in the book.
TEST(MatchingEngine, RestslWhenBuyDoesNotCross)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Buy, 100, 50));
    engine.processOrder(makeLimit(2, Side::Sell, 101, 50));

    EXPECT_TRUE(engine.trades().empty());
    ASSERT_TRUE(engine.orderBook().bestAsk().has_value());
    EXPECT_EQ(*engine.orderBook().bestAsk(), 101);
    EXPECT_TRUE(engine.orderBook().bestBid().has_value());
    EXPECT_EQ(*engine.orderBook().bestBid(), 100);
}

// If sell aggressor crosses, a trade is made and resting buy is removed from the book.
TEST(MatchingEngine, FullFillSellAggressor)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Buy, 100, 50));
    engine.processOrder(makeLimit(2, Side::Sell, 100, 50));

    ASSERT_EQ(engine.trades().size(), 1u);
    EXPECT_EQ(engine.trades()[0].buyOrderId, 1u);
    EXPECT_EQ(engine.trades()[0].sellOrderId, 2u);
    EXPECT_EQ(engine.trades()[0].price, 100);
    EXPECT_EQ(engine.trades()[0].quantity, 50u);

    EXPECT_FALSE(engine.orderBook().bestBid().has_value());
    EXPECT_FALSE(engine.orderBook().bestAsk().has_value());
}

// If buy aggressor crosses, a trade is made and resting sell is removed from the book.
TEST(MatchingEngine, FullFillBuyAggressor)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Sell, 101, 100));
    engine.processOrder(makeLimit(2, Side::Buy, 105, 100));

    ASSERT_EQ(engine.trades().size(), 1u);
    EXPECT_EQ(engine.trades()[0].buyOrderId, 2u);
    EXPECT_EQ(engine.trades()[0].sellOrderId, 1u);
    EXPECT_EQ(engine.trades()[0].price, 101);
    EXPECT_EQ(engine.trades()[0].quantity, 100u);

    EXPECT_FALSE(engine.orderBook().bestBid().has_value());
    EXPECT_FALSE(engine.orderBook().bestAsk().has_value());
}

// If buy aggressor crosses but is not fully filled, the resting sell should remain in the book.
TEST(MatchingEngine, PartialFillIncomingRests)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Sell, 101, 100));
    engine.processOrder(makeLimit(2, Side::Buy, 105, 150));

    ASSERT_EQ(engine.trades().size(), 1u);
    EXPECT_EQ(engine.trades()[0].buyOrderId, 2u);
    EXPECT_EQ(engine.trades()[0].sellOrderId, 1u);
    EXPECT_EQ(engine.trades()[0].price, 101);
    EXPECT_EQ(engine.trades()[0].quantity, 100u);

    EXPECT_FALSE(engine.orderBook().bestAsk().has_value());
    ASSERT_TRUE(engine.orderBook().bestBid().has_value());
    EXPECT_EQ(*engine.orderBook().bestBid(), 105);

    const Order* restingBuy = engine.orderBook().bestBidOrder();
    ASSERT_NE(restingBuy, nullptr);
    EXPECT_EQ(restingBuy->id, 2u);
    EXPECT_EQ(restingBuy->remainingQuantity, 50u);
}

// If sell aggressor crosses but is not fully filled, the resting buy should remain in the book.
TEST(MatchingEngine, PartialFillRestingRemains)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Sell, 101, 100));
    engine.processOrder(makeLimit(2, Side::Buy, 105, 40));

    ASSERT_EQ(engine.trades().size(), 1u);
    EXPECT_EQ(engine.trades()[0].buyOrderId, 2u);
    EXPECT_EQ(engine.trades()[0].sellOrderId, 1u);
    EXPECT_EQ(engine.trades()[0].price, 101);
    EXPECT_EQ(engine.trades()[0].quantity, 40u);

    EXPECT_FALSE(engine.orderBook().bestBid().has_value());
    ASSERT_TRUE(engine.orderBook().bestAsk().has_value());
    EXPECT_EQ(*engine.orderBook().bestAsk(), 101);

    const Order* restingSell = engine.orderBook().bestAskOrder();
    ASSERT_NE(restingSell, nullptr);
    EXPECT_EQ(restingSell->id, 1u);
    EXPECT_EQ(restingSell->remainingQuantity, 60u);
}

// If multiple levels are involved, the trades should be made in the order of the book.
TEST(MatchingEngine, MultiLevelSweep)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Sell, 101, 100));
    engine.processOrder(makeLimit(2, Side::Sell, 102, 50));
    engine.processOrder(makeLimit(3, Side::Buy, 105, 150));

    ASSERT_EQ(engine.trades().size(), 2u);

    EXPECT_EQ(engine.trades()[0].buyOrderId, 3u);
    EXPECT_EQ(engine.trades()[0].sellOrderId, 1u);
    EXPECT_EQ(engine.trades()[0].price, 101);
    EXPECT_EQ(engine.trades()[0].quantity, 100u);

    EXPECT_EQ(engine.trades()[1].buyOrderId, 3u);
    EXPECT_EQ(engine.trades()[1].sellOrderId, 2u);
    EXPECT_EQ(engine.trades()[1].price, 102);
    EXPECT_EQ(engine.trades()[1].quantity, 50u);

    EXPECT_FALSE(engine.orderBook().bestBid().has_value());
    EXPECT_FALSE(engine.orderBook().bestAsk().has_value());
}

// If sell aggressor crosses, the resting buy should use the resting price.
TEST(MatchingEngine, SellAggressorUsesRestingPrice)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Buy, 100, 100));
    engine.processOrder(makeLimit(2, Side::Sell, 99, 50));

    ASSERT_EQ(engine.trades().size(), 1u);
    EXPECT_EQ(engine.trades()[0].buyOrderId, 1u);
    EXPECT_EQ(engine.trades()[0].sellOrderId, 2u);
    EXPECT_EQ(engine.trades()[0].price, 100);
    EXPECT_EQ(engine.trades()[0].quantity, 50u);

    ASSERT_TRUE(engine.orderBook().bestBid().has_value());
    EXPECT_EQ(*engine.orderBook().bestBid(), 100);
    EXPECT_EQ(engine.orderBook().bestBidOrder()->remainingQuantity, 50u);
    EXPECT_FALSE(engine.orderBook().bestAsk().has_value());
}

// If multiple orders at the same price level, the trades should be made in the order of the book (FIFO).
TEST(MatchingEngine, FifoWithinSamePriceLevel)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Sell, 101, 30));
    engine.processOrder(makeLimit(2, Side::Sell, 101, 70));
    engine.processOrder(makeLimit(3, Side::Buy, 101, 50));

    ASSERT_EQ(engine.trades().size(), 2u);

    EXPECT_EQ(engine.trades()[0].sellOrderId, 1u);
    EXPECT_EQ(engine.trades()[0].quantity, 30u);

    EXPECT_EQ(engine.trades()[1].sellOrderId, 2u);
    EXPECT_EQ(engine.trades()[1].quantity, 20u);

    ASSERT_TRUE(engine.orderBook().bestAsk().has_value());
    EXPECT_EQ(*engine.orderBook().bestAsk(), 101);
    EXPECT_EQ(engine.orderBook().bestAskOrder()->id, 2u);
    EXPECT_EQ(engine.orderBook().bestAskOrder()->remainingQuantity, 50u);
}

// If an order is cancelled, it should be removed from the book.
TEST(MatchingEngine, CancelSellOrder)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Sell, 101, 30));
    engine.cancelOrder(1);

    EXPECT_TRUE(engine.trades().empty());
    EXPECT_FALSE(engine.orderBook().bestAsk().has_value());
    EXPECT_FALSE(engine.orderBook().bestBid().has_value());
    EXPECT_EQ(engine.orderBook().bestAskOrder(), nullptr);
}

// If an order is cancelled, it should be removed from the book.
TEST(MatchingEngine, CancelBuyOrder)
{
    MatchingEngine engine;
    engine.processOrder(makeLimit(1, Side::Buy, 101, 30));
    engine.cancelOrder(1);

    EXPECT_TRUE(engine.trades().empty());
    EXPECT_FALSE(engine.orderBook().bestBid().has_value());
    EXPECT_FALSE(engine.orderBook().bestAsk().has_value());
    EXPECT_EQ(engine.orderBook().bestBidOrder(), nullptr);
}
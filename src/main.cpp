#include <iostream>

#include "matching/MatchingEngine.h"

int main()
{
    MatchingEngine engine;

    // Resting sell: 100 @ 101
    engine.processOrder(Order{
        1,
        Side::Sell,
        OrderType::Limit,
        101,
        100,
        100,
        0
    });

    // Incoming buy: 150 @ 105 — should trade 100 @ 101, rest 50 @ 105
    engine.processOrder(Order{
        2,
        Side::Buy,
        OrderType::Limit,
        105,
        150,
        150,
        0
    });

    std::cout << "Falcon Trading Platform\n";
    std::cout << "Trades: " << engine.trades().size() << "\n";

    for (const Trade& trade : engine.trades())
    {
        std::cout << "  buy=" << trade.buyOrderId
                  << " sell=" << trade.sellOrderId
                  << " price=" << trade.price
                  << " qty=" << trade.quantity << "\n";
    }

    auto bestBid = engine.orderBook().bestBid();
    auto bestAsk = engine.orderBook().bestAsk();

    std::cout << "Best bid: " << (bestBid ? *bestBid : -1) << "\n";
    std::cout << "Best ask: " << (bestAsk ? *bestAsk : -1) << "\n";

    return 0;
}

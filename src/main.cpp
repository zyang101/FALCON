#include <iostream>
#include "exchange/Order.h"

int main()  {

    std::cout << "Falcon Trading Platform"  << std::endl;
    Order order{
        1,
        Side::Buy,
        OrderType::Limit,
        100,
        10,
        123456789
    };
    std::cout << "Order ID: " << order.id << std::endl;
    std::cout << "Price: " << order.price << std::endl;
    std::cout << "Quantity: " << order.quantity << std::endl;

    return 0;
}
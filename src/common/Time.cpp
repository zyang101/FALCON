#include "common/Time.h"

#include <chrono>

Timestamp currentTimestamp()
{
    using namespace std::chrono;

    return duration_cast<nanoseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}
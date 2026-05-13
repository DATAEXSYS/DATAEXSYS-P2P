#ifndef MINI_POW_TRACKER_H

#include "PKCertChain/MiniPowTracker.h"

#define MINI_POW_TRACKER_H

#include "pkcertchain_config.h"
#include <stdint.h>
#include <string.h>
#include <time.h>

#ifndef MINI_POW_TRACKER_INLINE
#define MINI_POW_TRACKER_INLINE static inline __attribute__((always_inline))
#endif

/*
 * MiniPowTracker
 * Tracks start time, receive time, and cumulative duration for challenges
 * using Linux monotonic time (microseconds).
 */


MINI_POW_TRACKER_INLINE void mini_pow_tracker_init(MiniPowTracker *tracker)
{
    if (!tracker) return;
    tracker->challenge_id = 0;
    tracker->session_id = 0;
    tracker->recent_start_time = 0;
    tracker->recent_receive_time = 0;
    tracker->cumulative_duration = 0;
}

MINI_POW_TRACKER_INLINE uint64_t mini_pow_tracker_get_current_us(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)(ts.tv_nsec / 1000ULL);
    }
    return 0;
}

MINI_POW_TRACKER_INLINE void mini_pow_tracker_update_start(MiniPowTracker *tracker)
{
    if (!tracker) return;
    tracker->recent_start_time = mini_pow_tracker_get_current_us();
}

MINI_POW_TRACKER_INLINE void mini_pow_tracker_update_receive(MiniPowTracker *tracker)
{
    if (!tracker) return;
    tracker->recent_receive_time = mini_pow_tracker_get_current_us();
    if (tracker->recent_receive_time > tracker->recent_start_time) {
        tracker->cumulative_duration += (tracker->recent_receive_time - tracker->recent_start_time);
    }
}

#endif // MINI_POW_TRACKER_H

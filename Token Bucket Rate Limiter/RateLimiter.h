#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

/**
 * Abstract base class for rate limiters.
 */
class RateLimiter {
public:
    virtual ~RateLimiter() = default;

    /**
     * Attempts to acquire a token from the rate limiter.
     *
     * @return true if a token was available and acquired, false otherwise
     */
    virtual bool tryAcquire() = 0;
};

#endif // RATE_LIMITER_H

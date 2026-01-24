#ifndef TOKEN_BUCKET_RATE_LIMITER_H
#define TOKEN_BUCKET_RATE_LIMITER_H

#include "RateLimiter.h"
#include <mutex>
#include <chrono>
#include <algorithm>

/**
 * Token Bucket Rate Limiter implementation.
 *
 * This class implements the token bucket algorithm for rate limiting.
 * Tokens are replenished at a fixed rate, and requests must acquire
 * a token before proceeding.
 */
class TokenBucketRateLimiter : public RateLimiter {
private:
    long capacity_;              // Max tokens
    long refill_rate_;            // Tokens per second
    long tokens_;
    std::chrono::nanoseconds last_refill_time_;
    mutable std::mutex lock_;

    /**
     * Refills the bucket based on elapsed time since last refill.
     */
    void refill() {
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
        auto elapsed_nanos = now - last_refill_time_;

        long tokens_to_add = (elapsed_nanos.count() * refill_rate_) / 1'000'000'000L;

        if (tokens_to_add > 0) {
            tokens_ = std::min(capacity_, tokens_ + tokens_to_add);
            last_refill_time_ = now;
        }
    }

public:
    /**
     * Constructs a TokenBucketRateLimiter.
     *
     * @param capacity The maximum number of tokens the bucket can hold
     * @param refill_rate The number of tokens to add per second
     */
    TokenBucketRateLimiter(long capacity, long refill_rate)
        : capacity_(capacity),
          refill_rate_(refill_rate),
          tokens_(capacity),
          last_refill_time_(std::chrono::high_resolution_clock::now().time_since_epoch()) {}

    /**
     * Attempts to acquire a token from the rate limiter.
     *
     * @return true if a token was available and acquired, false otherwise
     */
    bool tryAcquire() override {
        std::lock_guard<std::mutex> guard(lock_);
        refill();

        if (tokens_ > 0) {
            tokens_--;
            return true;
        }
        return false;
    }

    /**
     * Returns the current number of tokens available in the bucket.
     *
     * @return The current token count
     */
    long getAvailableTokens() const {
        std::lock_guard<std::mutex> guard(lock_);
        return tokens_;
    }

    /**
     * Returns the capacity of the bucket.
     *
     * @return The maximum number of tokens
     */
    long getCapacity() const {
        return capacity_;
    }

    /**
     * Returns the refill rate.
     *
     * @return The number of tokens added per second
     */
    long getRefillRate() const {
        return refill_rate_;
    }

    virtual ~TokenBucketRateLimiter() = default;
};

#endif // TOKEN_BUCKET_RATE_LIMITER_H

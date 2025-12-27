import java.util.concurrent.locks.ReentrantLock;

public class TokenBucketRateLimiter implements RateLimiter {

    private final long capacity;          // Max tokens
    private final long refillRate;         // Tokens per second

    private long tokens;
    private long lastRefillTimestamp;

    private final ReentrantLock lock = new ReentrantLock();

    public TokenBucketRateLimiter(long capacity, long refillRate) {
        this.capacity = capacity;
        this.refillRate = refillRate;
        this.tokens = capacity;
        this.lastRefillTimestamp = System.nanoTime();
    }

    @Override
    public boolean tryAcquire() {
        lock.lock();
        try {
            refill();

            if (tokens > 0) {
                tokens--;
                return true;
            }
            return false;
        } finally {
            lock.unlock();
        }
    }

    private void refill() {
        long now = System.nanoTime();
        long elapsedNanos = now - lastRefillTimestamp;

        long tokensToAdd =
                (elapsedNanos * refillRate) / 1_000_000_000L;

        if (tokensToAdd > 0) {
            tokens = Math.min(capacity, tokens + tokensToAdd);
            lastRefillTimestamp = now;
        }
    }
}

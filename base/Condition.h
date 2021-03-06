#ifndef __CONDITION__
#define __CONDITION__

#include "Mutex.h"
#include <pthread.h>
#include <errno.h>
#include <stdint.h>

class Condition : noncopyable {
public:
    //显示构造
    explicit Condition(MutexLock &mutex) : mutex_(mutex) {
        pthread_cond_init(&pcond_, NULL); //初始化条件变量
    }

    ~Condition() {
        pthread_cond_destroy(&pcond_); //释放条件变量
    }

    void wait() {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()); //阻塞在条件变量上
    }

    void notify() {
        pthread_cond_signal(&pcond_);  //解除在条件变量上的阻塞
    }

    void notifyAll() {
        pthread_cond_broadcast(&pcond_);
    }
    //超时等待
    bool WaitForSeconds(double seconds) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);

        const int64_t kNanoSecondsPerSecond = 1000000000;
        int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

        abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
        abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);

        return ETIMEDOUT == pthread_cond_timedwait(&pcond_, mutex_.getPthreadMutex(), &abstime);  //阻塞直到指定时间
    }
private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;
};

#endif
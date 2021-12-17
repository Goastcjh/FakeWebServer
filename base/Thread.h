#ifndef __THREAD__
#define __THREAD__

#include "noncopyable.h"
#include "MemoryPool.h"
#include <pthread.h>
#include <functional>
#include <memory>
#include <string>
#include <assert.h>

struct ThreadData {
    typedef std::function<void()> ThreadFunc;
    ThreadFunc func;
    std::string name;

    ThreadData(const ThreadFunc& Func, const std::string& Name) : func(Func), name(Name) {}
    void runInThread() { func(); }
};

class Thread : noncopyable {
public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc&&, const std::string& name = std::string());
    ~Thread();
    void start();
    int join();
    bool started() const;
    const std::string& name() const;
private:
    bool started_; //是否已启动
    bool joined_; //是否已调用join()函数
    pthread_t pthreadId; //线程ID
    ThreadFunc func;  //运行函数
    std::string name_; //名字

    void setDefaultName();

};

typedef std::shared_ptr<Thread> SP_Thread;
typedef std::unique_ptr<Thread, decltype(deleteElement<Thread>)*> UP_Thread;

#endif
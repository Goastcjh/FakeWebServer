#include "Thread.h"

inline Thread::Thread(const ThreadFunc&& Func, const std::string& n) : started_(false), joined_(false), pthreadId(0), func(Func), name_(n) {
    setDefaultName();
}

inline Thread::~Thread() {
    if (started_ && !joined_) {
        pthread_detach(pthreadId);
    }
}

inline void Thread::setDefaultName() {
    if (name_.empty()) {
        name_.append("Thread");
    }
}

void *startThread(void *obj) {
    ThreadData *data = static_cast<ThreadData*>(obj);
    data->runInThread();
    deleteElement<ThreadData>(data);
    return NULL;    
}

inline void Thread::start() {
    assert(!started_);
    started_ = true;
    ThreadData *data = newElement<ThreadData>(func, name_);
    if (pthread_create(&pthreadId, NULL, &startThread, data)) {
        started_ = false;
        deleteElement<ThreadData>(data);
    }
}

inline int Thread::join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId, NULL);
}

inline bool Thread::started() const {
    return started_;
}

inline const std::string& Thread::name() const {
    return name_;
}

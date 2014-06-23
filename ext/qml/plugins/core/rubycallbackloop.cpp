#include "rubycallbackloop.h"
#include <QDebug>

namespace RubyQml {

RubyCallbackLoop::RubyCallbackLoop(QObject *parent) :
    QObject(parent)
{
}

void RubyCallbackLoop::runTask(const Task &task)
{
    std::unique_lock<std::mutex> lock(mMutex);

    std::promise<void> promise;
    auto future = promise.get_future();

    mTasks.push(std::make_tuple(task, std::move(promise)));

    lock.unlock();
    mCondition.notify_all();

    future.wait();
}

void RubyCallbackLoop::exec()
{
    forever {
        std::unique_lock<std::mutex> lock(mMutex);

        mCondition.wait(lock, [&] { return !mTasks.empty() || mStop; });
        if (mStop) {
            return;
        }

        Task task;
        std::promise<void> promise;
        std::tie(task, promise) = std::move(mTasks.front());

        task();
        promise.set_value();

        mTasks.pop();
    }
}

void RubyCallbackLoop::stop()
{
    std::unique_lock<std::mutex> lock(mMutex);
    mStop = true;
}

} // namespace RubyQml

#pragma once
#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <queue>
#include <mutex>
#include <future>

namespace RubyQml {

class RubyCallbackLoop : public QObject
{
    Q_OBJECT
public:
    using Task = std::function<void ()>;

    explicit RubyCallbackLoop(QObject *parent = 0);
    void runTask(const Task &task);

public slots:
    void exec();
    void stop();

private:
    std::queue<std::tuple<Task, std::promise<void>>> mTasks;
    std::mutex mMutex;
    std::condition_variable mCondition;
    bool mStop = false;
};

} // namespace RubyQml

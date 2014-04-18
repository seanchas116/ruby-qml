#ifndef RUBYQML_MAINUTIL_H
#define RUBYQML_MAINUTIL_H

#include <QtCore/QObject>

namespace RubyQml {

class BackendMain : public QObject
{
    Q_OBJECT
public:
    explicit BackendMain(QObject *parent = 0);

    static BackendMain *instance() { return sInstance; }

public slots:

private:
    static BackendMain *sInstance;
};

} // namespace RubyQml

#endif // RUBYQML_MAINUTIL_H

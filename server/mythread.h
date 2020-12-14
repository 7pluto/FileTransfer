#include <QThread>
#include "server.h"
#include "debugononroff.h"
class MyThread : public QThread
{
     Q_OBJECT
public:
     void run();
};

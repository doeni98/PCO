#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QString>>




void doHackingTR(QString charset,
                 QString salt,
                 QString hash,
                 unsigned int nbChars,
                 unsigned int nbThreads,
                 long long unsigned int nbToCompute,
                 QString *resultPassword,
                 unsigned int idThread);

#endif // MYTHREAD_H

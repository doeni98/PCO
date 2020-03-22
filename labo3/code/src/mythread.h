/*
 * PCO - Laboratoire 3
 *
 * Modified by Bourqui Denis & Müller Nicolas on 22.03
 *
 */

#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "threadmanager.h"
#include <QString>

void doHackingTR(QString charset,
                 QString salt,
                 QString hash,
                 unsigned int nbChars,
                 unsigned int nbThreads,
                 long long unsigned int nbToCompute,
                 QString *resultPassword,
                 unsigned int idThread,
                 ThreadManager *manager);

#endif // MYTHREAD_H

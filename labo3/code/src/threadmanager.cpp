/*
 * PCO - Laboratoire 3
 *
 * Modified by Bourqui Denis & Müller Nicolas on 22.03
 *
 */

#include <QCryptographicHash>
#include <QVector>
#include <QDebug>
#include "threadmanager.h"
#include "mythread.h"
#include <pcosynchro/pcothread.h>

/*
 * std::pow pour les long long unsigned int
 */
long long unsigned int intPow (
        long long unsigned int number,
        long long unsigned int index)
{
    long long unsigned int i;

    if (index == 0)
        return 1;

    long long unsigned int num = number;

    for (i = 1; i < index; i++)
        number *= num;

    return number;
}

ThreadManager::ThreadManager(QObject *parent) :
    QObject(parent)
{}


void ThreadManager::incrementPercentComputed(double percentComputed)
{

    emit sig_incrementPercentComputed(percentComputed);
}

/*
 * Les paramètres sont les suivants:
 *
 * - charset:   QString contenant tous les caractères possibles du mot de passe
 * - salt:      le sel à concaténer au début du mot de passe avant de le hasher
 * - hash:      le hash dont on doit retrouver la préimage
 * - nbChars:   le nombre de caractères du mot de passe à bruteforcer
 * - nbThreads: le nombre de threads à lancer
 *
 * Cette fonction doit retourner le mot de passe correspondant au hash, ou une
 * chaine vide si non trouvé.
 */
QString ThreadManager::startHacking(
        QString charset,
        QString salt,
        QString hash,
        unsigned int nbChars,
        unsigned int nbThreads)
{
    long long unsigned int nbToCompute;

    QString resultPassword = "";

    /*
     * Set les variables du .h utilisé par tous les threads.
     *
     * et Calcul du nombre de hash à générer
     */
    nbToCompute        = intPow(charset.length(),nbChars);
    std::vector<std::unique_ptr<PcoThread>> threadList;

    /* demarrage des threads */
    for (unsigned int i=0; i<nbThreads; i++) {

        PcoThread *currentThread = new PcoThread(doHackingTR, charset,
                                                 salt,
                                                 hash,
                                                 nbChars,
                                                 nbThreads,
                                                 nbToCompute,
                                                 &resultPassword,
                                                 i,
                                                 this);

        threadList.push_back(std::unique_ptr<PcoThread>(currentThread));
    }

    /* Jointure des threads */
    for (size_t i = 0; i < threadList.size(); i++) {
        threadList.at(i)->join();
    }

    return resultPassword;
}


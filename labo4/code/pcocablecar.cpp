//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Denis Bourqui, Nicolas Müller

#include "pcocablecar.h"
#include <pcosynchro/pcothread.h>

#include <QDebug>
#include <QRandomGenerator>
constexpr unsigned int MIN_SECONDS_DELAY = 1;
constexpr unsigned int MAX_SECONDS_DELAY = 5;
constexpr unsigned int SECOND_IN_MICROSECONDS = 1000000;


PcoCableCar::PcoCableCar(const unsigned int capacity) : capacity(capacity), mutex(1), waitBeforeCC(0), waitInCC(0), cabinCanMove(0)
{

}

PcoCableCar::~PcoCableCar()
{

}

void PcoCableCar::waitForCableCar(int id)
{
    mutex.acquire();

    qDebug() << "Skieur " << id << " attends le télécabine";
    nbSkiersWaiting++;

    mutex.release();

    // Attends de pouvoir entrer dans la cabine
    waitBeforeCC.acquire();

}

void PcoCableCar::waitInsideCableCar(int id)
{
    // Attends que la cabine arrive en haut de la montagne
    qDebug() << "Skieur " << id << " attends dans le télécabine";
    waitInCC.acquire();

}

void PcoCableCar::goIn(int id)
{
    mutex.acquire();

    qDebug() << "Skieur " << id << " entre dans le télécabine";
    // Tous les skieurs sont entrés dans la cabine, elle peut monter
    if (++nbSkiersInside == nbSkiersToLoad) {
        cabinCanMove.release();
    }

    mutex.release();


}

void PcoCableCar::goOut(int id)
{
    mutex.acquire();

    qDebug() << "Skieur " << id << " sort du télécabine";
    // Tous les skieurs sont sortis, la cabine peut redescendre
    if (--nbSkiersInside == 0) {
        cabinCanMove.release();
    }

    mutex.release();

}

bool PcoCableCar::isInService()
{
    return inService;
}

void PcoCableCar::endService()
{
    inService = false;

    // Relache les skieurs qui attendent devant la cabine pour
    // que les threads se stoppent correctement et qu'ils ne
    // restent pas bloqués dans de l'attente infinie

    mutex.acquire();
    unsigned nbToFree = nbSkiersWaiting;
    mutex.release();


    for (unsigned i = 0; i < nbToFree; i++) {
        waitBeforeCC.release();
    }

}

void PcoCableCar::goUp()
{
    // Attends que tous les skieurs soient entrés pour monter
    cabinCanMove.acquire();

    qDebug() << "Le télécabine monte";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
}

void PcoCableCar::goDown()
{
    // Attends que tous les skieurs soient sortis pour redescendre
    cabinCanMove.acquire();

    qDebug() << "Le télécabine descend";
    PcoThread::usleep((MIN_SECONDS_DELAY + QRandomGenerator::system()->bounded(MAX_SECONDS_DELAY + 1)) * SECOND_IN_MICROSECONDS);
}

void PcoCableCar::loadSkiers()
{

    mutex.acquire();

    unsigned max = nbSkiersWaiting < capacity ? nbSkiersWaiting : capacity;
    nbSkiersWaiting -= max;

    // On définit dans cette fonction combien de skieurs vont entrer
    // pendant cette période de chargement. Un skieur qui arrive entre le load
    // et le moment où la cabine part ne pourra pas entrer car nous avons
    // fait le nombre de release ici basé sur le nombre de skieurs actuel
    // qui attendent
    nbSkiersToLoad = max;

    mutex.release();

    // Se fait 0 fois si y a pas de skieur, remonte juste après
    for (unsigned i = 0; i < max; i++) {
        waitBeforeCC.release();
    }

}

void PcoCableCar::unloadSkiers()
{
    mutex.acquire();

    unsigned nbToRelease = nbSkiersInside;

    mutex.release();

    // Relache tous les skieurs
    for (unsigned i = 0; i < nbToRelease; i++) {
        waitInCC.release();
    }

}

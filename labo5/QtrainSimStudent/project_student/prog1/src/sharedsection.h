//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Bourqui Denis, Müller Nicolas
//
#ifndef SHAREDSECTION_H
#define SHAREDSECTION_H

#include <QDebug>

#include <pcosynchro/pcosemaphore.h>

#include "locomotive.h"
#include "ctrain_handler.h"
#include "sharedsectioninterface.h"

/**
 * @brief La classe SharedSection implémente l'interface SharedSectionInterface qui
 * propose les méthodes liées à la section partagée.
 */
class SharedSection final : public SharedSectionInterface
{
public:

    /**
     * @brief SharedSection Constructeur de la classe qui représente la section partagée.
     * Initialisez vos éventuels attributs ici, sémaphores etc.
     */
    SharedSection() : isOccupied(false), isWaiting(false), mutex(1), waitingQueue(0) {
    }

    /**
     * @brief request Méthode a appeler pour indiquer que la locomotive désire accéder à la
     * section partagée (deux contacts avant la section partagée).
     * @param loco La locomotive qui désire accéder
     * @param priority La priorité de la locomotive qui fait l'appel
     */
    void request(Locomotive& loco, Priority priority) override {

        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 requested the shared section.").arg(loco.numero())));
    }

    /**
     * @brief getAccess Méthode à appeler pour accéder à la section partagée, doit arrêter la
     * locomotive et mettre son thread en attente si la section est occupée ou va être occupée
     * par une locomotive de plus haute priorité. Si la locomotive et son thread ont été mis en
     * attente, le thread doit être reveillé lorsque la section partagée est à nouveau libre et
     * la locomotive redémarée. (méthode à appeler un contact avant la section partagée).
     * @param loco La locomotive qui essaie accéder à la section partagée
     * @param priority La priorité de la locomotive qui fait l'appel
     */
    void getAccess(Locomotive &loco, Priority priority) override {

        mutex.acquire();

        if (isOccupied) {

            loco.arreter();
            isWaiting = true;

            mutex.release();

            waitingQueue.acquire();

            mutex.acquire();

            isWaiting = false;
            loco.demarrer();

        }

        adaptRailwayForLoco(loco);
        isOccupied = true;
        mutex.release();

        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 accesses the shared section.").arg(loco.numero())));
    }

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section
     * partagée. (reveille les threads des locomotives potentiellement en attente).
     * @param loco La locomotive qui quitte la section partagée
     */
    void leave(Locomotive& loco) override {

        mutex.acquire();

        isOccupied = false;

        if (isWaiting) {
            waitingQueue.release();
        }

        mutex.release();

        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 leaves the shared section.").arg(loco.numero())));
    }

private:

    void adaptRailwayForLoco(Locomotive &loco) {

        switch (loco.numero()) {

        case 7:
            diriger_aiguillage(2, DEVIE, 0);
            diriger_aiguillage(9, DEVIE, 0);
            break;
        case 42:
            diriger_aiguillage(2, TOUT_DROIT, 0);
            diriger_aiguillage(9, TOUT_DROIT, 0);
            break;
        default:
            afficher_message("Unknown loco, unable to predict route");
        }
    }

    bool isOccupied, isWaiting;

    PcoSemaphore mutex, waitingQueue;
};


#endif // SHAREDSECTION_H

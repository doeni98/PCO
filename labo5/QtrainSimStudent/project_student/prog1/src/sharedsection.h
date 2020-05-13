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

        // Teste si le chemin est libre
        if (isOccupied) {

            // Arrête la loco si c'est occupé
            loco.arreter();
            isWaiting = true;

            // Bloque dans la file d'attente si besoin
            while (isOccupied) {

                mutex.release();
                waitingQueue.acquire();
                mutex.acquire();

            }

            // Évite de redémarrer la locomotive si la
            // simulation a été coupée
            staticMutex.acquire();
            if (!isRunning) {

                staticMutex.release();
                mutex.release();
                return;
            }
            staticMutex.release();

            // Redémarre la loco si la simulation tourne
            // toujours et informe que plus personne n'attends
            loco.demarrer();
            isWaiting = false;
        }

        // Informe que le chemin est occupé
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

        // Annonce que la section est libre
        isOccupied = false;

        // Libère un train s'il y en a un qui attends
        if (isWaiting) {
            waitingQueue.release();
        }

        mutex.release();

        // Exemple de message dans la console globale
        afficher_message(qPrintable(QString("The engine no. %1 leaves the shared section.").arg(loco.numero())));
    }

    static void stopSimulation() {

        staticMutex.acquire();

        // Arrêt définitif
        isRunning = false;

        // On le libère pas les trains en attente dans la file d'attente
        // car dans le cadre de ce laboratoire, il a été décidé de ne pas joindre
        // les threads à la fin de l'execution. Cette méhtode statique évite simplement
        // qu'une locomotive puisse redémarrer après un arrêt d'urgence

        staticMutex.release();
    }

private:

    /**
     * @brief isRunning     Variable partagée qui définit si les sections partagées
     *                      sont encore actives
     */
    static bool isRunning;

    /**
     * @brief staticMutex   Mutex pour proteger isRunning
     */
    static PcoSemaphore staticMutex;

    /**
     * @brief isOccupied    true si la section est occupée, false sinon
     * @brief isWaiting     true si un train est dans la file d'attente, false sinon
     */
    bool isOccupied, isWaiting;

    /**
     * @brief mutex         Protection des variables
     * @brief waitingQueue  Sychronisation de la file d'attente
     */
    PcoSemaphore mutex, waitingQueue;
};

// Init des vars statiques
bool SharedSection::isRunning = true;
PcoSemaphore SharedSection::staticMutex(1);

#endif // SHAREDSECTION_H

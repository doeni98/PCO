//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Bourqui Denis, Müller Nicolas
//
#include "locomotivebehavior.h"
#include "ctrain_handler.h"

void LocomotiveBehavior::run()
{
    // Sert de compteur pour savoir quand on inverse le sens de rotation
    int loopCounter = 0;
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    // Protège les accès à la variable isRunning qui est partagée
    mutex.acquire();

    // On se permet de tester uniquement là haut, si la simulation fonctionne.
    // La terminaison ne sera pas propre dans tous les cas, si les trains s'arrêtent
    // alors qu'on attends un contact, on ne fera plus ce test dans tous les cas.
    // Ici, nous avons fait le choix que, si la simulation est arrêtée, on sera sûrment
    // dans une attente infinie, et si ce n'est pas le cas, les instructions qui vont encore
    // s'effectuer ne sont pas génantes par rapport à l'arrêt du système.
    while(isRunning) {

        mutex.release();

        // Attends le contact après lequel il faudra annoncer vouloir accéder à la section partagée
        attendre_contact(getRequestContact());
        // Demande
        sharedSection->request(loco, priority);
        // Attends le contact d'accès à la section critique
        attendre_contact(getAccessContact());
        // Demande d'accès à la section critique, bloque jusqu'à ce que
        // la locomotive a accès
        sharedSection->getAccess(loco, priority);
        // Maintenant que la loco a accès, modifie le tracé pour lui permettre de faire
        // son circuit
        adaptRailwayForLoco();
        // Attends d'arriver sur le contact qui permet de leave
        attendre_contact(getLeaveContact());
        // Sors de la section critique
        sharedSection->leave(loco);
        // Attends d'être de retour au point de départ
        attendre_contact(getStartingPointContact());

        // Change de sens si la loco a fait 2 tours une fois qu'elle a atteint le point
        // de départ.
        if(++loopCounter == 2) {
            // Reset le compteur
            loopCounter = 0;

            // Inverse le sens de rotation de la loco
            invertTurnDirection();
        }

        mutex.acquire();
    }

    mutex.release();
}

// Init des vars static
PcoSemaphore LocomotiveBehavior::mutex(1);
bool LocomotiveBehavior::isRunning = true;

// Méthodes
void LocomotiveBehavior::printStartMessage()
{
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage()
{
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}

int LocomotiveBehavior::getAccessContact() {

    return contacts[turningClockwise?1:4];
}

int LocomotiveBehavior::getRequestContact() {

    return contacts[turningClockwise?0:5];
}

int LocomotiveBehavior::getLeaveContact() {

    return contacts[turningClockwise?3:2];
}
int LocomotiveBehavior::getStartingPointContact() {

    return contacts[6];
}

void LocomotiveBehavior::invertTurnDirection() {

    // Inverse le sens de la locomotive
    inverser_sens_loco(loco.numero());

    // Modifie le comportement de la loco
    // dans le code aussi
    turningClockwise = !turningClockwise;
}

void LocomotiveBehavior::adaptRailwayForLoco() {

    switch (type) {

    case LocomotiveBehavior::LocomotiveType::LocomotiveA:
        diriger_aiguillage(2, DEVIE, 0);
        diriger_aiguillage(9, DEVIE, 0);
        break;
    case LocomotiveBehavior::LocomotiveType::LocomotiveB:
        diriger_aiguillage(2, TOUT_DROIT, 0);
        diriger_aiguillage(9, TOUT_DROIT, 0);
        break;
    default:
        afficher_message("Unknown loco, unable to predict route");
    }
}

void LocomotiveBehavior::stopAllBehaviors() {

    mutex.acquire();

    isRunning = false;

    mutex.release();
}

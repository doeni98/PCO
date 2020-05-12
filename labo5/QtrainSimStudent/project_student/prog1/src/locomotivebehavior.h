//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Bourqui Denis, Müller Nicolas
//
#ifndef LOCOMOTIVEBEHAVIOR_H
#define LOCOMOTIVEBEHAVIOR_H

#include "locomotive.h"
#include "launchable.h"
#include "sharedsectioninterface.h"

/**
 * @brief La classe LocomotiveBehavior représente le comportement d'une locomotive
 */
class LocomotiveBehavior : public Launchable
{
public:
    /*!
     * \brief locomotiveBehavior Constructeur de la classe
     * \param loco la locomotive dont on représente le comportement
     */
    LocomotiveBehavior(Locomotive& loco, std::shared_ptr<SharedSectionInterface> sharedSection /*, autres paramètres éventuels */) : loco(loco), sharedSection(sharedSection) {

        switch (loco.numero()) {

        case 7:
            accessContact = 23;
            leaveContact = 5;
            break;
        case 42:
            accessContact = 19;
            leaveContact = 1;
            break;
        default:
            // TODO
            return;
        }
    }

    int getAccesContact() {

        return accessContact;
    }

    int getLeaveContact() {

        return leaveContact;
    }

protected:
    /*!
     * \brief run Fonction lancée par le thread, représente le comportement de la locomotive
     */
    void run() override;

    /*!
     * \brief printStartMessage Message affiché lors du démarrage du thread
     */
    void printStartMessage() override;

    /*!
     * \brief printCompletionMessage Message affiché lorsque le thread a terminé
     */
    void printCompletionMessage() override;

    /**
     * @brief loco La locomotive dont on représente le comportement
     */
    Locomotive& loco;

    /**
     * @brief sharedSection Pointeur sur la section partagée
     */
    std::shared_ptr<SharedSectionInterface> sharedSection;

    /*
     * Vous êtes libres d'ajouter des méthodes ou attributs
     *
     * Par exemple la priorité ou le parcours
     */

    int accessContact;
    int leaveContact;
    int startContact;

    bool turningClockwise;

};

#endif // LOCOMOTIVEBEHAVIOR_H

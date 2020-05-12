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
    LocomotiveBehavior(Locomotive& loco, std::shared_ptr<SharedSectionInterface> sharedSection /*, autres paramètres éventuels */)
        : loco(loco), sharedSection(sharedSection), turningClockwise(true) {

        switch (loco.numero()) {

        case 7:
            contacts = {24, 23, 16, 5, 34, 33, 25};
            break;
        case 42:
            contacts = {20, 19, 13, 1, 31, 30, 22};
            break;
        default:
            // TODO
            return;
        }
    }

    int getAccessContact(){
        return contacts[turningClockwise?1:4];
    }

    int getRequestContact(){
        return contacts[turningClockwise?0:5];
    }

    int getLeaveContact(){
        return contacts[turningClockwise?3:2];
    }
    int getStartingPointContact(){
        return contacts[6];
    }

    void invertTurnDirection(){
        turningClockwise = !turningClockwise;
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

    /* Clockwise
     * request[0], access[1], enter[2], leave[3]
     * Anticlockwise
     * request[5], access[4], enter[3], leave[2]
     * starting point always [6]
     * */
    QVector<int> contacts;

};

#endif // LOCOMOTIVEBEHAVIOR_H

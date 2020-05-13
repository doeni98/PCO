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
#include "pcosynchro/pcosemaphore.h"

/**
 * @brief La classe LocomotiveBehavior représente le comportement d'une locomotive
 */
class LocomotiveBehavior : public Launchable
{
public:

    enum class LocomotiveType {
        LocomotiveA,
        LocomotiveB
    };
    /*!
     * \brief locomotiveBehavior Constructeur de la classe
     * \param loco la locomotive dont on représente le comportement
     */
    LocomotiveBehavior(Locomotive& loco, std::shared_ptr<SharedSectionInterface> sharedSection , LocomotiveType type)
        : loco(loco), sharedSection(sharedSection), turningClockwise(true), type(type) {

        // Définit les contacts importants en fonction du type de la locomotive
        switch (type) {

        case LocomotiveType::LocomotiveA:
            contacts = {24, 23, 16, 5, 34, 33, 25};
            break;
        case LocomotiveType::LocomotiveB:
            contacts = {20, 19, 13, 1, 31, 30, 22};
            break;
        default:
            // Ne fait rien
            return;
        }
    }

    /**
     * @brief getAccessContact  Récupère le contact qui doit déclancher l'accès
     *                          à la section critique
     */
    int getAccessContact();

    /**
     * @brief getRequestContact  Récupère le contact qui doit déclancher la demande d'accès
     *                           à la section critique
     */
    int getRequestContact();

    /**
     * @brief getLeaveContact   Récupère le contact qui doit déclancher la sortie
     *                          de la section critique
     */
    int getLeaveContact();

    /**
     * @brief getStartingPointContact  Récupère le contact qui représente le point de départ
     */
    int getStartingPointContact();

    /**
     * @brief invertTurnDirection   Inverse le sens de rotation des tours de circuit
     */
    void invertTurnDirection();

    /**
     * @brief adaptRailwayForLoco   Adapte le tracé pour correspondre à la bonne locomotive
     */
    void adaptRailwayForLoco();

    /**
     * @brief stopAllBehaviors  Arrête la simulation côté comportement des locomotives
     */
    static void stopAllBehaviors();

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

    /**
     * @brief turningClockwise Contient le sens de rotation
     */
    bool turningClockwise;

    /**
     * @brief type Contient quel est le type de locomotive
     */
    LocomotiveType type;

    /**
     * @brief isRunning Variable partagée qui définit si la simulation
     *                  est toujours active
     * @brief mutex     Protège cette variable
     */
    static bool isRunning;
    static PcoSemaphore mutex;

    /**
     * @brief Contient les points de contacts importants
     *        pour une locomotive. Voici la liste des
     *        points en fonction du sens de rotation
     *
     * Clockwise
     * request[0], access[1], enter[2], leave[3]
     * Anticlockwise
     * request[5], access[4], enter[3], leave[2]
     * starting point always [6]
     * */
    QVector<int> contacts;

};

#endif // LOCOMOTIVEBEHAVIOR_H

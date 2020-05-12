//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Denis Bourqui, Nicolas Müller

#include "cablecarbehavior.h"

void CableCarBehavior::run()
{

    // Un test de service ici suffit, car il est demandé que
    // la télécabine finisse sa routine une fois qu'elle a commencé
    while (cableCar->isInService()) {

        cableCar->loadSkiers();
        cableCar->goUp();

        cableCar->unloadSkiers();
        cableCar->goDown();

    }
}

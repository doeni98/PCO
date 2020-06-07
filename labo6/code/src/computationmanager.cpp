//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Denis Bourqui, Nicolas Müller


// A vous de remplir les méthodes, vous pouvez ajouter des attributs ou fonctions pour vous aider
// déclarez les dans ComputationManager.h et définissez les méthodes ici.
// Certaines fonctions ci-dessous ont déjà un peu de code, il est à remplacer, il est là temporairement
// afin de faire attendre les threads appelants et aussi afin que le code compile.

#include "computationmanager.h"


ComputationManager::ComputationManager(int maxQueueSize): MAX_TOLERATED_QUEUE_SIZE(maxQueueSize)
{
    // TODO
}

int ComputationManager::requestComputation(Computation c) {

    monitorIn();

    /* get the ordinal of computationType. A : 0, B : 1, C : 2 */
    int ctype = static_cast<std::underlying_type<ComputationType>::type>(c.computationType);

    /* wait until there's space in request queue of this computationType*/
    if(requestQueue[ctype].size() == (int) MAX_TOLERATED_QUEUE_SIZE)
        wait(queueNotFull[ctype]);

    /* Add computation to Requests queue of corresponding computationType */
    int id = nextId++;
    requestQueue[ctype].push_back(Request(c, id));

    /* Send signal that a new request of type 'computationType' arrived. This awakes threads that are waiting in getWork*/
    signal(newRequest[ctype]);

    monitorOut();

    /* Return the calculated id of this request */
    return id;
}

void ComputationManager::abortComputation(int id) {
    monitorIn();

    /* check if the id is in one of the three Requests buffer. If so, remove it */

    /* for each Request buffer */
    for (int i = 0; i < DIFFERENT_COMPUTATION ; i++) {

        /* For each request in buffer */
        int requestCount = requestQueue[i].size();
        for(int j = 0; j < requestCount; ++j){

            /* if the id's are matching */
            if(requestQueue[i].at(j).getId() == id){

                /* Remove the id */
                requestQueue[i].removeAt(j);
                /* Signal that there is space in the request buffer */
                signal(queueNotFull[i]);

                monitorOut();

                /* Stop here */
                return;
            }
        }
    }

    /* If the id wasn't in the Request buffer then it's already running in a worker.
     * We have add the id to the stoppedId- and the ignoredResultsId list*/

    /* Add it to the ignoredResultsId list */
    ignoredResultsId.push_back(id);
    /* if this is the id, that should be the next result, then awake the getNextResult function for skipping this one */
    if(id == nextResultId)
        signal(newResult);

    /* add this id to the stoppedId list. list used by continueWork() function*/
    stoppedId.push_back(id);

    monitorOut();
}

Result ComputationManager::getNextResult() {
    /* this function is supposed to be called only by one thread at the time. */

    monitorIn();

    /* Repeat on every new result that arrives (and ones at the calling of this function)*/
    do{

        /* if the nextResultId is in the ignored list, then skip this id and remove it from the ignored list.*/
        while(ignoredResultsId.contains(nextResultId)){
           /* remove id from ignored list */
           ignoredResultsId.erase(std::find(ignoredResultsId.begin(), ignoredResultsId.end(), nextResultId));
           // skip this  nextResultId
           nextResultId++;
        }

        /* foreach result in resultList*/
        for(int i = 0; i < results.size(); ++i){

            Result result = results.at(i);
            /* if this is the next result to send then return this result */
            if(result.getId() == nextResultId){

                /* Increment the next result id */
                nextResultId++;

                /* Remove this result from list */
                results.removeAt(i);

                monitorOut();

                /* return this result */
                return result;
            }
        }

        /* Wait until a new result arrives, signal is sent by provideResult */
        wait(newResult);

    }while (1);
}

Request ComputationManager::getWork(ComputationType computationType) {

    monitorIn();

    /* get the ordinal of computationType. A : 0, B : 1, C : 2 */
    int ctype = static_cast<std::underlying_type<ComputationType>::type>(computationType);

    /* if there is no request of my type -> wait */
    if(requestQueue[ctype].size() == 0)
        wait(newRequest[ctype]);

    /* Remove request from RequestQueue */
    Request r = requestQueue[ctype].front();
    requestQueue[ctype].pop_front();

    /* signal there is space for a new queue.
     * this awakes threads that are sleeping in requestComputation due to a full buffer */
    signal(queueNotFull[ctype]);

    monitorOut();

    /* Return the request containing the computation for the worker */
    return r;
}

bool ComputationManager::continueWork(int id) {

    monitorIn();

    /* if this id is in the aborted list*/
    if(stoppedId.contains(id)){

        /* remove the id from the aborted list */
        ignoredResultsId.erase(std::find(ignoredResultsId.begin(), ignoredResultsId.end(), id));

        monitorOut();

        /* return false: that stops the worker*/
        return false;
    }

    monitorOut();

    /* if the id isn't in the aborted list, then continue work */
    return true;
}

void ComputationManager::provideResult(Result result) {

    monitorIn();

    /* Add result to results list */
    results.push_back(result);

    /* Signal that there is a new result */
    signal(newResult);

    monitorOut();
}

void ComputationManager::stop() {
    // TODO
}

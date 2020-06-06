//    ___  _________    ___  ___  ___  ___  //
//   / _ \/ ___/ __ \  |_  |/ _ \|_  |/ _ \ //
//  / ___/ /__/ /_/ / / __// // / __// // / //
// /_/   \___/\____/ /____/\___/____/\___/  //
//                                          //
// Auteurs : Denis Bourqui, Nicolas Müller


// A vous de remplir les méthodes, vous pouvez ajouter des attributs ou méthodes pour vous aider
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
    int ctype = static_cast<std::underlying_type<ComputationType>::type>(c.computationType);
    if(requestQueue[ctype].size() == (int) MAX_TOLERATED_QUEUE_SIZE)
        wait(queueNotFull[ctype]);

    int id = nextId++;
    requestQueue[ctype].push_back(Request(c, id));
    signal(newRequest[ctype]);
    monitorOut();
    return id;
}

void ComputationManager::abortComputation(int id) {
    monitorIn();
    for (int i = 0; i < DIFFERENT_COMPUTATION ; i++) {
        int requestCount = requestQueue[i].size();
        for(int j = 0; j < requestCount; ++j){
            if(requestQueue[i].at(j).getId() == id){
                requestQueue[i].removeAt(j);
                signal(queueNotFull[i]);
                monitorOut();
                return;
            }
        }
    }

    ignoredResultsId.push_back(id);
    if(id == nextResultId)
        signal(newResult);
    stoppedId.push_back(id);
    monitorOut();
}

Result ComputationManager::getNextResult() {

    monitorIn();
    do{ // on every new Result (and ones at the calling of this function)

        //remove next results that are ignored
        while(ignoredResultsId.contains(nextResultId)){
           // remove id from ignored list
           ignoredResultsId.erase(std::find(ignoredResultsId.begin(), ignoredResultsId.end(), nextResultId));
           // increment next result id
           nextResultId++;
        }

        for(int i = 0; i < results.size(); ++i){ // foreach result in buffers result list

            Result result = results.at(i);
            if(result.getId() == nextResultId){ // if this is the next result to send then return this result
                nextResultId++;
                results.removeAt(i);
                monitorOut();
                return result;
            }
        }

        wait(newResult);
    }while (1);
}

Request ComputationManager::getWork(ComputationType computationType) {

    monitorIn();
    int ctype = static_cast<std::underlying_type<ComputationType>::type>(computationType);
    if(requestQueue[ctype].size() == 0) // if there is no request of my type -> wait
        wait(newRequest[ctype]);

    Request r = requestQueue[ctype].front();
    requestQueue[ctype].pop_front(); // remove request from queue
    signal(queueNotFull[ctype]); // signal there is space for a new queue
    monitorOut();
    return r;
}

bool ComputationManager::continueWork(int id) {
    monitorIn();
    if(stoppedId.contains(id)){
        ignoredResultsId.erase(std::find(ignoredResultsId.begin(), ignoredResultsId.end(), id));
        monitorOut();
        return false;
    }
    monitorOut();
    return true;
}

void ComputationManager::provideResult(Result result) {
    monitorIn();
    results.push_back(result);
    signal(newResult);
    monitorOut();
}

void ComputationManager::stop() {
    // TODO
}

# PCO - Labo06 - Rapport

#### 14.05.2020

## Denis Bourqui - Nicolas Müller

# Partie 1

### Shared Section

Dans cette partie le but était de gérer les accès à la section partagée du circuit et éviter que deux trains y soient en même temps.

Le code est commenté est facile à comprendre. L'idée générale est de bloquer une locomotive si elle veut accéder à la section alors qu'elle est occupée. On gère ceci avec un bool `isOccupied` qui est à `true` si la section est déjà utilisée.

Donc la locomotive peut y accéder, on la laisse passer. Et sinon, on l'arrête jusqu'à ce que `isOccupied` repasse à `false` et on la redémarre. Cela permet de laisser passer sans arrêter les locomotives qui ont le droit de passer et d'arrêter celles qui doivent.

Il y a une difficulté pour arrêter le circuit, si une locomotive sort de la section critique avec l’inertie. Elle va donc relâcher le mutex qui bloquait l'autre et celle-ci va redémarrer.

```c++
staticMutex.acquire();
if (!isRunning) {

    staticMutex.release();
    mutex.release();
    return;
}
staticMutex.release();
```

Pour cela, un `bool` statique définit si la simulation est toujours en fonctionnement. Ce dernier est protégé par un `mutex` statique et permet de quitter la fonction si la simulation est terminée au lieu de redémarrer la locomotive.

Pour sortir de la zone critique, on libère le train qui était en attente, s'il y en avait un et on modifie `isOccupied` pour informer qu'il a quitter la zone.

### Locomotive Behavior

Pour gérer les différents comportements, on a décidé de définir deux types de locomotives différents avec: 

```c++
enum class LocomotiveType {
        LocomotiveA,
        LocomotiveB
    };
```

Le circuit doit fonctionner pour deux locomotives. Ici on définit une Locomotive `A` et `B`. Ce qui veut dire qu'en fonction du type, on va définir les points de contacts importants ainsi que les réglages d'aiguillage qu'il va falloir appliquer en fonction de la locomotive qui veut entrer dans la section critique. 

###### Contacts

Nous avons un vecteur qui contient tous les contacts importants. 

```c++
case LocomotiveType::LocomotiveA:
contacts = {24, 23, 16, 5, 34, 33, 25};
break;
```

```
     * Clockwise
     * request[0], access[1], enter[2], leave[3]
     * Anticlockwise
     * request[5], access[4], enter[3], leave[2]
     * starting point always [6]
```

Nous avons donc des getters et des setters pour récupérer les bons contacts avec des noms parlants pour éviter d'utiliser les indices du vecteur. Avec par exemple `getLeaveContact`. Dans la fonction on retourne le bon contact du vecteur en fonction du sens de rotation de la locomotive.

###### Tracé

`adaptRailwayForLoco()` s'occupe d'adapter le tracé en fonction du type de la locomotive.

###### Arrêt

Même manière de faire que dans `SharedSection`. Nous avons une variable statique et un mutex pour la protéger.

###### Run

Le principe est simple, la loco part depuis son point de départ, attends le contact qui déclenche `getAccess`, demande l'accès. À ce moment le circuit est mis à jour pour permettre à la loco de faire son trajet. Une fois à l'intérieur de la section critique, elle attends le contact qui définira que la locomotive est sortie. À ce moment, on attends le point de départ et on repart pour un tour. Change de sens à ce moment si la locomotive en a déjà fait deux.

### Tests

![](/home/nico/heig_vd/ba_4/pco/labos/labo06/mod_circuit.png)

Pour ces tests, les positions de départs sont celles par défaut et la vitesse est la même pour les deux locomotives. Ici `14` mais si les deux ont la même vitesse, le résultat sera identique. La vitesse maximale de test est de `14`, augmenter au delà devient dangereux car même si on ordonne à la locomotive de s'arrêter, l'inertie fera qu'elle arrivera tout de même dans la section critique.

En rouge, c'est la section partagée, en bleu le point de `getAccess` et en vert, `leave`.

 Le premier conflit devrait se produire tout de suite. 

On constate donc qu'elles ne se rentrent pas dedans quand on lance le programme. Après deux tours, elles tournent et attendent les bons contacts.

Le dernier test effectué concerne l'arrêt d'urgence. Il ne faut pas que la locomotive bleue redémarre si la rouge, avec l'inertie, sort de la zone critique. Ce test réussi avec le programme fourni.

# Partie 2

Dans cette partie la priorité à été ajoutée. Cela signifie que les locomotives vont maintenant demander si elles peuvent effectivement accéder à la ressources. Si une locomotive de priorité plus élevée demande après la première, l'accès sera donné à celle avec la priorité haute.

On considère dans le laboratoire que les deux locomotives ont des priorités distinctes.

### Locomotive Behavior

Ici, un champ priorité a été ajouté pour définir la priorité à la création du comportement de la locomotive. Cette dernière utilise:

```c++
enum class Priority {
    LowPriority,
    HighPriority
};
```

De la classe `SharedSectionInterface`.

On passe donc la priorité d'une locomotive en paramètre à sa création. 

###### Run

Le déroulement n'a pas beaucoup changé: On a ajouté l'attente du contact qui lancera la requête d'accès une fois qu'il sera atteint.

### Shared Section

Ici aussi, les changements ne sont pas énormes. La gestion des priorité se gère de la manière suivante:

- Un boolean `highPriorityRequest` initialisé à `false`, définit si une requête importante a été demandée. 
- Lorsque la locomotive de priorité basse appelle `request`, cela ne modifie pas la variable.
- Lorsqu'une locomotive de priorité haute appelle `request`, il passe à `true`.

Cela permet de savoir si une locomotive prioritaire souhaite accéder au tronçon critique. Dans `getAccess`, une condition en plus pour bloquer les locomotives apparaît. En effet, il faut que si `highPriorityRequest` est à `true`, si la locomotive est celle qui est prioritaire, elle puisse passer. Si c'est l'autre locomotive qui arrive en premier, elle doit se bloquer et rester bloquée tant que la locomotive prioritaire n'est pas entrée et ressortie du tronçon critique.

Dans le code:

```c++
if (isOccupied || (highPriorityRequest && priority != Priority::HighPriority)) {
```

Permet de faire un premier tri et bloquer une locomotive non prioritaire si une requête prioritaire existe. Cette condition va toujours laisser passer n'importe quelle locomotive si le tronçon est vide.

La condition de la boucle d'attente aussi a changé pour correspondre.

Tout à la fin de `getAccess`:

```c++
if (highPriorityRequest) {
    highPriorityRequest = false;
}
// Libère les ressources
mutex.release();
```

Si la locomotive qui voulait accéder à la ressource y a accéder via une requête prioritaire, la remet à `false`. Cela ne suffira pas pour libérer la locomotive non prioritaire. En effet, maintenant que la prioritaire est passée, `isOccupied` vaut `true` jusqu'à ce qu'elle sorte. C'est à ce moment que la locomotive prioritaire va relâcher `waitingQueue`.

On aura donc toujours qu'une seule locomotive dans le tronçon critique.

### Tests

![](/home/nico/heig_vd/ba_4/pco/labos/labo06/p2_mod_circuit.png)

Le circuit est le même que pour la partie 1, on ajoute en bleu clair, le contact qui va déclencher la demande d'accès.Pour provoquer un conflit de priorité la locomotive `A` (rouge) sera à la vitesse 10 et la `B` (bleue) à 14. La rouge arrivera avant la bleue au `request` mais est moins prioritaire. La bleue fera ensuite la `request`. La rouge arrive au point `getAccess()` mais l'accès lui est refusé parce que la bleue doit arriver. Celle-ci passe et la rouge démarre une fois que la bleue a quitté la section critique.

L'arrêt d'urgence fonctionne toujours.

# Conclusion

Ce laboratoire est très intéressant. Il aurait été plaisant de pouvoir tester notre code sur la maquette réelle pour voir une application à ce que nous avons fait Avec la situation sanitaire actuelle, cela n'est pas possible mais la simulation ajoute un grand point positif par rapport au terminal. 

Nous pensons avoir globalement atteint les objectifs de ce laboratoire.
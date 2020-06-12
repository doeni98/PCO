# PCO Labo 06

**Denis Bourqui, Müller Nicolas**

## Introduction

Comme nous avons interprété la donnée il y a 3 queue dans le buffer. Une pour chaque type de computation. Une fois qu’une requête est en traitement elle n'est plus dans la queue. 

## Classe `ComputationManager` (Buffer)

Voici une représentation graphique de notre `ComputationManager`. Les différents éléments serons expliqué dans les différents étapes.

![image-20200610125925085](/mnt/reds/home/reds/pco20_student/labo6/img/dia.png)

## Etape 1

Dans cette étape nous allons faire l'ajout de requêtes par le client dans le buffer ainsi que la sortie de la queue par les worker. (implémentation standard d'un prod./cons. à buffer multiple)

### Fonction ` int requestComputation(Computation c)`

Cette fonction fait l'ajout d'une computation dans la queue associé. Elle est potentiellement bloquante si la queue est pleine.  Si la queue est pleine on se met en attente sur la variable de condition `queueNotFull` du numéro de queue associé. 

Si il y a de la place dans la queue on donne une id a cette requête et on l'ajoute à la queue. 

Pour finir on signal qu'une nouvelle requête d'un type de computation (0 à 2) est arrivé. Cela permet aux worker de ce type attendent une nouvelle requête, de sortir cela de la queue et de commencer leur travaille.

### Fonction `Request getWork(ComputationType computationType)`

Cette fonction est appelé par les worker quand ils attendent du travaille. 

Si la queue des requêtes du types donné en paramètre `computationtype` est vide, ils se mettent en attende sur la condition `newRequest`. 

Si/quand la queue n'est pas vide ils sortent la première requêtes de la queue.

Finalement ils signalent `queueNotFull`du type concernée pour laisser arriver de nouvelle requêtes dans la queue.  

### Tests

| Test                                                         | Resultat attendu                         | Resultat |
| ------------------------------------------------------------ | ---------------------------------------- | -------- |
| computation de différent types pris par le bon worker        | chaque worker reçoit son type de calcule | ok       |
| 12 calcules d'un type donnée, avec 1 seul worker             | le 12 sera pas mise en queue             | ok       |
| Un calcule en attende d’entrer dans la queue sera mise dans queue dès qu'il y a de la place | Calcule en attende est ajouté à la queue | ok       |



## Etape 2

Cette étape gère les résultats.

### Fonction `Result getNextResult());` 

On sait que cette fonction est appelé que par un thread, car il y a un seul client. On aura donc jamais plusieurs thread qui vont ce battre pour recevoir le mutex sur le quelle ils on fait le wait. 

Cette fonction à deux tâches principales. 

* Elle trouve et retourne le prochain résultat attendu du client.
* Elle sort tout les résultat qui proviennent d'un calcul qui à été stoppé, de la liste des résultat.

Elle fait ces deux tâches une fois qu'elle à été appelé. Si le résultat de calcule attendu n'est pas encore disponible elle c'est met en attende sur la condition `newResult`. Au prochain signale, elle refait ses 2 tâches.  

### Fonction ` void provideResult(Result result);`

Cette fonction ajoute juste le résultat dans la liste des résultat. Même si cette tâche est sensé été stopper au prélavant. Ça sera la fonction getNextResult qui va la liquider de la liste des résultat.

### Tests

| Test                                                         | Resultat attendu           | Resultat |
| ------------------------------------------------------------ | -------------------------- | -------- |
| Un résultat attend d’être retourner si c'est pas lui le prochain ID. | Résultat arrivent en ordre | ok       |



## Etape 3

Cette étape gère les arrêts des calcules.

### Fonction `void abortComputation(int id); `

Dans cette fonction on doit savoir si le calcule et encore dans la queue ou si elle est déjà en cours de calcule. 

Nous avons pas de possibilité de savoir le type de computation depuis un certain id. Nous devons donc parcourir les 3 queues pour rechercher l'id. Si on la trouve, on la sort de la queue et on signale `queueNotFull`.

Si elle n'est pas dans les queue mais déjà en cours de calcule, on ajoute son id dans la liste des `stoppedId` ainsi dans la liste des `ignoredResults`. 

Si il s'agit de l'`id` qui est attendu comme prochain résultat, on signal `newResult`. Car il est possible que le résultat d'après est déjà présent.  

### Fonction `bool continueWork(int id);`

Cette fonction regarde si son `id` est dans la liste des `stoppedId`.  Si oui, il la supprime de la liste et retourne `false`, ce qui fait stopper le calcule. Sinon elle retourne `true` pour continuer le calcule. 

### Tests

| Test                                     | Resultat attendu                              | Resultat |
| ---------------------------------------- | --------------------------------------------- | -------- |
| un worker s’arrête si on le stoppe       | worker s’arrête, résultat n'est pas récupérer | ok       |
| Arrêter un calcule qui est dans la queue | le calcule sort de la queue                   | ok       |



## Etape 4

Cette étape géré l’arrêt de tout les threads.

### Fonction ` void stop()`

Cette fonction met le flag `stop` de la classe a true. Avec ca plus aucun nouvau wait sera fait dans toutes les fonctions.

Pour relâcher les threads qui sont actuellement sur un wait, il signale sur toutes les variables de conditions. 

### Modification des fonctions

Toutes les fonctions contiennent maintenant un check si le flag `stop` est actif. Si cela est activ ils sortent du moniteur et ils lèvent une exception. Ce check est fait avant et après tous les wait.

| Test                                                         | Resultat attendu | Resultat |
| ------------------------------------------------------------ | ---------------- | -------- |
| plus aucun nouveau "segment" se dessine sur le gui après un arrêt | ok               | ok       |
| plus aucun calcule dans la queue est pris                    | ok               | ok       |
| Plus aucun résultat s'affiche                                | ok               | ok       |
| Si un worker est en cours de son dernier segment, le résultat ne s'affiche non plus. | ok               | ok       |
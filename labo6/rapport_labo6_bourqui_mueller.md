# PCO Labo 06
**Denis Bourqui, Müller Nicolas**
## Introduction
Comme nous avons interprété la donnée, il y a 3 queues dans le buffer. Une pour chaque type de computation. Une fois qu’une requête est en traitement, elle n'est plus dans la queue. 
## Classe `ComputationManager` (Buffer)
Voici une représentation graphique de notre `ComputationManager`. Les différents éléments seront expliqués dans les différentes étapes.
![image-20200610125925085](/mnt/reds/home/reds/pco20_student/labo6/img/dia.png)
## Étape 1
Dans cette étape nous allons faire l'ajout de requêtes par le client dans le buffer ainsi que la sortie de la queue par les worker. (implémentation standard d'un prod./cons. à buffer multiple)
### Fonction ` int requestComputation(Computation c)`
Cette fonction fait l'ajout d'une computation dans la queue associée. Elle est potentiellement bloquante si la queue est pleine.  Si la queue est pleine, on se met en attente sur la variable de condition `queueNotFull` du numéro de queue associé. 
S’il y a de la place dans la queue, on donne une id à cette requête et on l'ajoute à la queue. 
Pour finir, on signal qu'une nouvelle requête d'un type de computation (0 à 2) est arrivée. Cela permet aux worker de ce type attendent une nouvelle requête, de sortir cela de la queue et de commencer leur travaille.
### Fonction `Request getWork(ComputationType computationType)`
Cette fonction est appelée par les worker quand ils attendent du travail. 
Si la queue des requêtes du type donné en paramètre `computationtype` est vide, ils se mettent en attende sur la condition `newRequest`. 
Si/quand la queue n'est pas vide ils sortent la première requête de la queue.
Finalement ils signalent `queueNotFull`du type concernée pour laisser arriver de nouvelle requêtes dans la queue.  
### Tests
| Test                                                         | Résultat attendu                         | Résultat |
| ------------------------------------------------------------ | ---------------------------------------- | -------- |
| computation de différents types pris par le bon worker        | chaque worker reçoit son type de calcule | ok       |
| 12 calcule d'un type donné, avec 1 seul worker             | le 12 ne sera pas mis en queue             | ok       |
| Un calcule en attende d’entrer dans la queue sera mise dans queue dès qu'il y a de la place | Calcule en attende est ajouté à la queue | ok       |

## Étape 2
Cette étape gère les résultats.
### Fonction `Result getNextResult());` 
On sait que cette fonction est appelée que par un thread, car il y a un seul client. On aura donc jamais plusieurs thread qui vont ce battre pour recevoir le mutex sur le quelle ils ont fait le wait. 
Cette fonction à deux tâches principales. 
* Elle trouve et retourne le prochain résultat attendu du client.
* Elle sort tous les résultats qui proviennent d'un calcul qui a été stoppé, de la liste des résultats.
Elle fait ces deux tâches une fois qu'elle a été appelée. Si le résultat de calcule attendu n'est pas encore disponible elle c'est met en attende sur la condition `newResult`. Au prochain signal, elle refait ses 2 tâches.  
### Fonction ` void provideResult(Result result);`
Cette fonction ajoute juste le résultat dans la liste des résultats. Même si cette tâche est sensée été stopper au prélevant. Ça sera la fonction getNextResult qui va la liquider de la liste des résultats.
### Tests
| Test                                                         | Résultat attendu           | Résultat |
| ------------------------------------------------------------ | -------------------------- | -------- |
| Un résultat attend d’être retourné si ce n’est pas lui le prochain ID. | Résultat arrivent en ordre | ok       |

## Étape 3
Cette étape gère les arrêts des calcules.
### Fonction `void abortComputation(int id); `
Dans cette fonction on doit savoir si le calcule et encore dans la queue ou si elle est déjà en cours de calcule. 
Nous n’avons pas de possibilité de savoir le type de computation depuis un certain id. Nous devons donc parcourir les 3 queues pour rechercher l'id. Si on la trouve, on la sort de la queue et on signale `queueNotFull`.
Si elle n'est pas dans les queues, mais déjà en cours de calcule, on ajoute son id dans la liste des `stoppedId` ainsi dans la liste des `ignoredResults`. 
S’il s'agit de l'`id` qui est attendu comme prochain résultat, on signal `newResult`. Car il est possible que le résultat d'après soit déjà présent.  
### Fonction `bool continueWork(int id);`
Cette fonction regarde si son `id` est dans la liste des `stoppedId`.  Si oui, il la supprime de la liste et retourne `false`, ce qui fait stopper le calcule. Sinon elle retourne `true` pour continuer le calcule. 
### Tests
| Test                                     | Résultat attendu                              | Résultat |
| ---------------------------------------- | --------------------------------------------- | -------- |
| un worker s’arrête si on le stoppe       | worker s’arrête, résultat n'est pas récupéré | ok       |
| Arrêter un calcule qui est dans la queue | le calcule sort de la queue                   | ok       |

## Étape 4
Cette étape gérer l’arrêt de tous les threads.
### Fonction ` void stop()`
Cette fonction met le flag `stop` de la classe a true. Avec ça plus aucun nouvau wait sera fait dans toutes les fonctions.
Pour relâcher les threads qui sont actuellement sur un wait, il signale sur toutes les variables de conditions. 
### Modification des fonctions
Toutes les fonctions contiennent maintenant un check si le flag `stop` est actif. Si cela est actives ils sortent du moniteur et ils lèvent une exception. Ce check est fait avant et après tous les wait.
| Test                                                         | Résultat attendu | Resultat |
| ------------------------------------------------------------ | ---------------- | -------- |
| plus aucun nouveau "segment" ne se dessine sur le gui après un arrêt | ok               | ok       |
| plus aucun ne calcule dans la queue est pris                    | ok               | ok       |
| Plus aucun résultat ne s'affiche                                | ok               | ok       |
| Si un worker est en cours de son dernier segment, le résultat ne s'affiche non plus. | ok               | ok       |
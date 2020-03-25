# PCO- Labo03 - Bourqui Denis & Müller Nicolas

## Le 22.03.2020

### Introduction

Le but de ce laboratoire est de craquer un hash MD5 en multi-thread. Le code pour craquer le hash nous est donné pour un seul thread. Le laboratoire demande une solution pour partager le travail entre les différents thread et accélérer la recherche du mot de passe.

### Partage du travail entre les threads 

Nous avons choisi de garder l'algorithme pour calculer les mots de passe qui était fourni et de partager correctement le travail entre plusieurs threads pour l'utiliser correctement. Nous avons donc choisi de séparer le travail en fixant pour chaque thread un mot de passe par lequel il doit commencer et un mot de passe par lequel il fini. Avec le charset actuel de taille 66, cela signife que pour par exemple deux threads, l'un va commencer à `aaa` et le deuxième commencera au charactère `66 / 2` qui est ici `H`. 

L'algorithme donné fait varier d'abord les caractères en début de mot puis ceux en fin de mot. Dans l'exemple ci-dessus, si le mot de passe recherché est composé de 4 caractères, le premier thread vérifira les mots de passe de `aaaa` à `***G` et le deuxième ceux de `aaaH` à `****`. Les boucles s'arrêtent une fois qu'un mot de passe est trouvé.

Quelques différences en terme de charge de travail entre le dernier thread et les autres peuvent survenir à cause de la division entière. En effet, le dernier mot de passe de tous les autres threads est calculé de la même manière tandis que le dernier thread s'arrête au mot de passe uniquement composé de `*` (pour notre charset). Cela veut dire qu'il compensera les imperfections de la division entière vu qu'il fera le reste des mots de passes. Cette différence est petite par rapport au nombre d'itérations du code.  

### Barre de progression

Pour la barre de progression nous avons pu réutiliser le même code que celui pour un seul thread. Si 1000 opérations représentent 1% du travail, dès qu'un thread a effectué 1000 opérations, il incrémentera la barre de progression. Cette approche sera un peu moins précise que d'incrémenter un compteur commun entre tous les threads. Nous avons décidé d'opter pour la première solution pour éviter de bloquer des ressources à chaque test de mot de passe. Nous pensons que la petite perte de précision n'est pas significative par rapport au nombre d'opérations effectuées dans ce code.

### Tests et résultats

###### Test de repartition entre les threads
Charset:
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!$~*

avec la longeur du charset de = 66
on divise la taille du charset par le nombre de thread:
66 / 3 = 22

le prémier thread fera donc le mot de passe de '..a' à '..v'
charset[0] = a

le deuxième thread fera donc le mot de passe de '..w' à ..R'
charset[22] = w

le troisiemme thread fera donc le mot de passe de '..S' à ..\*'
charset[44] = S

Repartition avec 3 threads (produit à l'éxecution)
```
Thread  2  first password  "aaS"  last password  "\*\*\*"
Thread  1  first password  "aaw"  last password  "\*\*R"
Thread  0  first password  "aaa"  last password  "\*\*v"
```
On voit que les mot de passes à tester sont bien repartie entre les 3 threads.

###### Test de performances

| longeur mdp | nombre threads | temps |
| ----------- | -------------- | ----- |
| 1           | 1              | 4ms |
| 1           | 30             | 13ms |
| 1           | 66             | 45ms |
| 2           | 1              | 19ms      |
| 2           | 2              | 18ms      |
| 2           | 5              | 16ms      |
| 2           | 10             | 16ms      |
| 3           | 1              | 1071ms|
| 3           | 2              | 611ms |
| 3           | 5              | 538ms |
| 3           | 10             | 567ms |
| 4           | 1              | 27.3s |
| 4           | 2              | 16.2s |
| 4           | 5              | 16.2s |
| 4           | 10             | 17.5s |

Il n'est pas facile de faire des tel tests performances. Les résultats dépendes de la charge du cpu actuel, de la configuration de la machine virtuelle et de la manière de la préamption fonctionne. 
Malgré cela on peux observer une tendence d'une augmentation de la vitesse si on utilise plusieurs threads. Cela n'est pas valable pour un mot de passe de la longeur 1, car le temps pour créer les threads sont plus important par rapport à au temps de crackage.
Le temps augmente a nouveau si il y a trop de thread. Le programme peux même crasher si il y en à trop. 


Ces tests on été effectué avec un sel.

### Limites du code

Cette manière de partager le travail entre les threads limite de nombre de threads possibles. Il est impossible de créer plus de thread que de caractères dans le charset. En effet, chaque thread teste les mots de passe possibles à partir d'un caractère calculé à partir du nombre de threads. Avec le charset actuel, il est donc impossible de partager le travail correctement entre plus de `66` threads.

### Conclusion

Ce laboratoire nous a permis de nous rendre compte que répartir le travail pour une tâche donnée n'était pas facile. Nous avons pensé à plusieurs solutions mais les implémenter était difficle. Globalement nous pensons avoir atteint les objectifs de ce laboratoire.


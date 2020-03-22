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

TODO

### Limites du code

Cette manière de partager le travail entre les threads limite de nombre de threads possibles. Il est impossible de créer plus de thread que de caractères dans le charset. En effet, chaque thread teste les mots de passe possibles à partir d'un caractère calculé à partir du nombre de threads. Avec le charset actuel, il est donc impossible de partager le travail correctement entre plus de `66` threads.

### Conclusion

Ce laboratoire nous a permis de nous rendre compte que répartir le travail pour une tâche donnée n'était pas facile. Nous avons pensé à plusieurs solutions mais les implémenter était difficle. Globalement nous pensons avoir atteint les objectifs de ce laboratoire.


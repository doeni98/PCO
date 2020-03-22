/*
 * PCO - Laboratoire 3
 *
 * Modified by Bourqui Denis & Müller Nicolas on 22.03
 *
 */

#include "mythread.h"
#include "threadmanager.h"

#include <QCryptographicHash>
#include <QVector>
#include <QDebug>

/*
 * Fonction qui teste si le hash du mot de passe passé en paramètre est le même que le hash cible
 *
 */

bool testPassword (
                    const QString& salt,
                    const QString& hash,
                    const QString& currentPasswordString,
                    QCryptographicHash& md5);

void doHackingTR(QString charset,
                 QString salt,
                 QString hash,
                 unsigned int nbChars,
                 unsigned int nbThreads,
                 long long unsigned int nbToCompute,
                 QString *resultPassword,
                 unsigned int idThread,
                 ThreadManager *manager) {

    unsigned int i;

    /*
     * Correspond à l'index dans le charset. Défini la plage des caractères que
     * le thread doit traiter.
     */
    unsigned int indexFirstCharToTest;
    unsigned int indexFirstCharOfNextThread;

    /*
     * Nombre de mot de passe testé par le thread
     */
    long long unsigned int nbComputed;

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    unsigned int nbValidChars;

    /*
     * Mot de passe courant à tester et le dernier mot de passe qui doit être traité
     * par le thread
     */
    QString currentPasswordString;
    QString lastPasswordToTest;

    /*
     * Tableau contenant les index dans la chaine charset des caractères de
     * currentPasswordString
     */
    QVector<unsigned int> currentPasswordArray;

    /*
     * Object QCryptographicHash servant à générer des md5
     */
    QCryptographicHash md5(QCryptographicHash::Md5);


    nbComputed         = 0;

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    nbValidChars       = charset.length();

    /*
     * Les mots de passe sont traité en modifiant les premiers caractères puis les derniers. Cette
     * méthodologie a été reprise du code fourni qui fonctionnait mais sur un thread uniquement.
     *
     * Ici, on génère le premier mot de passe que le thread va tester et le dernier.
     * Le premier sera de la forme <aaaaaX> et le dernier de la forme <*****1> pour que le
     * thread d'après commence à tester de <aaaaa2> à <******?> par exemple.
     */
    indexFirstCharToTest = nbValidChars / (int) nbThreads * idThread;
    indexFirstCharOfNextThread = nbValidChars / (int) nbThreads * (idThread + 1);

    /*
     * Remplis le mot de passe par le premier char du charset
     */
    currentPasswordString.fill(charset.at(0),nbChars);
    currentPasswordArray.fill(0,nbChars);

    /*
     * Dernier char mis aux possibilités qu'on aimerait tester avec le thread courant
     */
    currentPasswordString.replace(nbChars - 1, 1, charset.at(indexFirstCharToTest));
    currentPasswordArray.replace(nbChars - 1, indexFirstCharToTest);

    /*
     * Le dernier mot de passe à tester pour le thread sera toujours composé que du dernier
     * caractère du charset sauf la dernière lettre. Ici tout le mot de passe est set à '*'
     * avec le charset actuel
     */
    lastPasswordToTest.fill(charset.at(nbValidChars - 1), nbChars);

    /*
     * Si le thread n'est pas le dernier, il faut modifier le dernier caractère pour qu'il
     * corresponde au caractère avant celui du thread suivant
     *
     */
    if (idThread + 1 != nbThreads) {

        lastPasswordToTest.replace(nbChars - 1, 1, charset.at(indexFirstCharOfNextThread - 1));
    }

    /*
     * Tant qu'on a pas tout essayé et qu'aucun mot de passe à été trouvé...
     */

    while (currentPasswordString != lastPasswordToTest && *resultPassword != "") {

        /*
         * Teste le mot de passe courant
         */
        if (testPassword(salt, hash, currentPasswordString, md5)) {
            *resultPassword = currentPasswordString;
            break;
        }

        /*
         * Tous les 1000 hash calculés, on notifie qui veut bien entendre
         * de l'état de notre avancement (pour la barre de progression)
         */
        if (nbComputed != 0 && (nbComputed % 1000) == 0) {

            manager->incrementPercentComputed((double)1000/nbToCompute);
        }

        /*
         * On récupère le mot de pass à tester suivant.
         *
         * L'opération se résume à incrémenter currentPasswordArray comme si
         * chaque élément de ce vecteur représentait un digit d'un nombre en
         * base nbValidChars.
         *
         * Le digit de poids faible étant en position 0
         */
        i = 0;

        while (i < (unsigned int)currentPasswordArray.size()) {
            currentPasswordArray[i]++;

            if (currentPasswordArray[i] >= nbValidChars) {
                currentPasswordArray[i] = 0;
                i++;
            } else
                break;
        }

        /*
         * On traduit les index présents dans currentPasswordArray en
         * caractères
         */
        for (i=0;i<nbChars;i++)
            currentPasswordString[i]  = charset.at(currentPasswordArray.at(i));

        nbComputed++;
    }

    /* Test du dernier mot de passe que le thread doit tester */

    if (*resultPassword == "" && testPassword(salt, hash, currentPasswordString, md5)) {
        *resultPassword = currentPasswordString;
    }
    nbComputed++;
}

bool testPassword (
                    const QString& salt,
                    const QString& hash,
                    const QString& currentPasswordString,
                    QCryptographicHash& md5) {

    /*
     * Hash du mot de passe à tester
     */
    QString currentHash;

    /* On vide les données déjà ajoutées au générateur */
    md5.reset();
    /* On préfixe le mot de passe avec le sel */
    md5.addData(salt.toLatin1());
    md5.addData(currentPasswordString.toLatin1());
    /* On calcul le hash */
    currentHash = md5.result().toHex();

    return currentHash == hash;
}

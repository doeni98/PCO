#include "mythread.h"
#include "threadmanager.h"

#include <QCryptographicHash>
#include <QVector>
#include <QDebug>

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
    unsigned int indexFirstCharToTest;
    unsigned int indexFirstCharOfNextThread;


    long long unsigned int nbComputed;

    /*
     * Nombre de caractères différents pouvant composer le mot de passe
     */
    unsigned int nbValidChars;

    /*
     * Mot de passe à tester courant
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
     * On initialise le premier mot de passe à tester courant en le remplissant
     * de nbChars fois du premier caractère de charset
     */
    indexFirstCharToTest = nbValidChars / (int) nbThreads * idThread;
    indexFirstCharOfNextThread = nbValidChars / (int) nbThreads * (idThread + 1);

    currentPasswordString.fill(charset.at(0),nbChars);
    currentPasswordArray.fill(0,nbChars);

    currentPasswordString.replace(nbChars - 1, 1, charset.at(indexFirstCharToTest));
    currentPasswordArray.replace(nbChars - 1, indexFirstCharToTest);

    lastPasswordToTest.fill(charset.at(nbValidChars - 1), nbChars);

    if (idThread + 1 != nbThreads) {

        lastPasswordToTest.replace(nbChars - 1, 1, charset.at(indexFirstCharOfNextThread - 1));
    }

    /*
     * Tant qu'on a pas tout essayé...
     */

    while (currentPasswordString != lastPasswordToTest) {

        if (*resultPassword != "") {
            qDebug() << "A thread found password";
            break;
        }

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

    // Test last password

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
     * Hash du mot de passe à tester courant
     */
    QString currentHash;

    /* On vide les données déjà ajoutées au générateur */
    md5.reset();
    /* On préfixe le mot de passe avec le sel */
    md5.addData(salt.toLatin1());
    md5.addData(currentPasswordString.toLatin1());
    /* On calcul le hash */
    currentHash = md5.result().toHex();

    /*
     * Si on a trouvé, on retourne le mot de passe courant (sans le sel)
     */
    return currentHash == hash;
}

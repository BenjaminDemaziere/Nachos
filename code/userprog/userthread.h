

/*
(étape 3)
Création interne d'un thread avec la fonction f, et l'argument arg

Retourne -1 si la création du thread a échoué
*/
extern int do_UserThreadCreate(int f, int arg, int adrExit);



/*
    Détruit le thread qui appelle cette fonction
*/
extern void do_UserThreadExit();


/* Le thread appelant attend que le thread idT termine
*/
void do_UserThreadJoin(int idT);
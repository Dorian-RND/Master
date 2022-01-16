#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <ncurses.h>

#include <commun.h>
#include <paquet.h>
#include <ecran.h>

#include <pthread.h>

/**
 * Prog Conc - TP Kems (version 3) 
 * Etudiante : Mathilde MOTTAY 
 */

typedef struct {
  int numJoueur; 
  int numCoequipier; 
  int numEquipe; 
  int signalLance; 
} joueur_t; 

/***********************
	VARIABLES GLOBALES 
 **********************/ 

// Paquet de cartes 
paquet_t * paquet = PAQUET_NULL; 

// Tapis central 
tapis_t * tapis_central = TAPIS_NULL; 

// Tableau des tapis des joueurs 
tapis_t ** tapis = NULL; /* tableau des tapis */

// Nombre de joueurs 
int nbJoueurs = 0; 

// Compte-rendu 
err_t cr = OK; 

// Indice carte 
carte_id_t c = -1; 

// Compteur de tapis
tapis_id_t t = 0; 

// Booléen pour fin de la partie 
booleen_t fini = FAUX; 

// Déclarations des mutex (sémaphores d'exclusion mutuelle)
pthread_mutex_t mutexFini = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexTapisCentral = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t mutexCarte1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCarte2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCarte3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCarte4 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexEcran = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexKems = PTHREAD_MUTEX_INITIALIZER; 

// Ecran 
ecran_t * ecran = NULL ; 
  
// Message 
char mess[256] ; 

// Tableau pour savoir par équipe qui Kems
int Kems[3]= {0, 0, 0}; 

joueur_t initJoueur (int numJoueur, int numCoequipier, int numEquipe){
  joueur_t j; 
  j.numJoueur = numJoueur; 
  j.numCoequipier = numCoequipier; 
  j.numEquipe = numEquipe; 
  j.signalLance = -1; 
  return j; 
}

void arret(int sig){
  /* printf( "Arrêt utilisateur\n");*/
} 

/*************************
	PROCESSUS JOUEUR   
	Un joueur = un thread 
 *************************/ 
void Joueur(void * arg){
  joueur_t j = *((joueur_t*)arg);
  carte_id_t ind_carte = -1; // Indice de la carte du joueur qui sera échangée 
  carte_id_t ind_carte_central = -1; // Indice de carte du tapis central qui sera échangée 
	booleen_t echange = FAUX; // Booléen pour l'échange des cartes 
	fini = FAUX; 

	// Verrouillage mutexFini
  pthread_mutex_lock(&mutexFini); 
  while(!fini){
    // Déverouillage mutexFini
    pthread_mutex_unlock(&mutexFini); 

    // Vérifie si son coéquipier lui a fait signe pour dire Kems 
    if((Kems[j.numEquipe] == 1) && (j.signalLance != 1)){
      // Verrouillage mutexFini
      pthread_mutex_lock(&mutexFini); 
      fini = VRAI;
      // Déverrouillage mutexFini
      pthread_mutex_unlock(&mutexFini); 

      sprintf(mess, "Le joueur %2d crie Kems ! Les joueurs %2d et %d ont gagné", j.numJoueur, j.numJoueur, j.numCoequipier); 
      // Verrouillage mutexEcran 
      pthread_mutex_lock(&mutexEcran); 
      ecran_message_pause_afficher(ecran, mess); 
      // Déverrouillage mutexEcran
      pthread_mutex_unlock(&mutexEcran); 

      break; 
    }

    /* Test arrêt */
    if(tapis_carre(tapis[j.numJoueur])){   
      pthread_mutex_lock(&mutexKems); 
      Kems[j.numEquipe] = 1; 
      j.signalLance = 1; 
      pthread_mutex_unlock(&mutexKems); 
    }

    pthread_mutex_lock(&mutexTapisCentral);
  
  	// On choisit les 2 cartes à échanger entre le tapis du joueur et le tapis central.
    if((cr = tapis_cartes_choisir(&echange, tapis[j.numJoueur], &ind_carte, tapis_central, &ind_carte_central))){
      sprintf(mess, "Pb dans choix des cartes, code retour = %d\n", cr);
      pthread_mutex_lock(&mutexEcran); 
      ecran_message_pause_afficher(ecran, mess); 
      pthread_mutex_unlock(&mutexEcran); 
      erreur_afficher(cr); 
      break; 
    }

		// Si l'échange de cartes est possible 
		if(echange){

			// Verrouillage du mutex de la carte correspondant à celle échangée sur le tapis central 
    	switch(ind_carte_central){
				case 0: pthread_mutex_lock(&mutexCarte1); break; 
				case 1: pthread_mutex_lock(&mutexCarte2); break; 
				case 2: pthread_mutex_lock(&mutexCarte3); break; 
				case 3: pthread_mutex_lock(&mutexCarte4); break; 
			}

      if((cr = tapis_cartes_echanger(tapis[j.numJoueur], ind_carte, tapis_central, ind_carte_central))){
        sprintf(mess, "Pb d'échange de cartes entre la carte %ld du tapis du joueur %d et la carte %ld du tapis central", ind_carte, j.numJoueur, ind_carte_central); 
        // Verrouillage mutexEcran 
        pthread_mutex_lock(&mutexEcran); 
        ecran_message_pause_afficher(ecran, mess); 
        erreur_afficher(cr); 
        // Déverrouillage mutexEcran 
        pthread_mutex_unlock(&mutexEcran); 
        break; 
      }	     

      sprintf(mess, "Joueur %i: Echange carte %ld avec carte %ld du tapis central ", j.numJoueur, ind_carte, ind_carte_central);
      // Verrouillage mutexEcran 
      pthread_mutex_lock(&mutexEcran); 
      ecran_message_pause_afficher(ecran, mess); 
      ecran_cartes_echanger(ecran, f_tapis_f_carte_lire(ecran_tapis_central_lire(ecran), ind_carte_central), f_tapis_f_carte_lire(ecran_tapis_joueur_lire(ecran, j.numJoueur), ind_carte));
      ecran_afficher(ecran, tapis_central, tapis); 
      ecran_message_effacer(ecran);
      // Déverrouillage mutexEcran 
      pthread_mutex_unlock(&mutexEcran); 

			// Déverrouillage du mutex de la carte correspondant à celle échangée sur le tapis central 
			switch(ind_carte_central){
				case 0: pthread_mutex_unlock(&mutexCarte1); break; 
				case 1: pthread_mutex_unlock(&mutexCarte2); break; 
				case 2: pthread_mutex_unlock(&mutexCarte3); break; 
				case 3: pthread_mutex_unlock(&mutexCarte4); break; 
			}   	 
	  
    }

    pthread_mutex_unlock(&mutexTapisCentral);
	}
}

/**
 * Processus TAPIS
 * Tapis = Un thread 
 */
void Tapis(){
  // Verrouillage du mutexFini
	pthread_mutex_lock(&mutexFini); 
  while(!fini){
    // Déverrouillage du mutexFini
    pthread_mutex_unlock(&mutexFini); 

    sleep(3);

    // Verrouillage mutexTapisCentral
		pthread_mutex_lock(&mutexTapisCentral);
    // Verrouillage mutexEcran 
 		pthread_mutex_lock(&mutexEcran);
 
    /* 
      * Pas un seul échange des joueurs
      * --> redistribution du tapis central 
      */
    ecran_message_pause_afficher(ecran, "Pas d'échange --> Redistribution tapis central"); 
    for(c = 0; c < TAPIS_NB_CARTES; c++){
      if((cr = tapis_carte_retirer(tapis_central, c, paquet))){
        ecran_message_pause_afficher(ecran, "Pb dans retrait d'une carte du tapis central"); 
        erreur_afficher(cr); 
        break; 
      }
      
       if((cr = tapis_carte_distribuer(tapis_central, c, paquet))){
        ecran_message_pause_afficher(ecran, "Pb dans distribution d'une carte pour le tapis central"); 
        erreur_afficher(cr); 
        break; 
      }
    }

    ecran_afficher(ecran, tapis_central, tapis);
    ecran_message_effacer(ecran); 

    // Déverrouillage mutexTapisCentral
    pthread_mutex_unlock(&mutexTapisCentral);
     // Déverrouillage mutexEcran
 		pthread_mutex_unlock(&mutexEcran);
  }
}


int main (int argc, char * argv[]){
  int i; 
  signal(SIGINT, arret); 

  printf("\n\n\n\t===========Début %s==========\n\n", argv[0]);

  if(argc != 2){
    printf(" Programme de test sur l'affichage de <nb joueurs> tapis avec ncurses\n");
    printf(" usage : %s <nb joueurs>\n", argv[0]);
    exit(0); 
  }

  /*
	 * Vérifications si le nombre de joueurs saisi par l'utilisateur est correct
	 */
	int NbJoueurs = atoi(argv[1]);
	if((NbJoueurs != 4) && (NbJoueurs != 6)){
		printf("Le nombre de joueurs doit être égal à 4 ou 6 !\n");
		exit(0);
	}

  srandom(getpid());

  /* Création du paquet */
  printf("Création du paquet..." ); fflush(stdout); 
  paquet = paquet_creer(); 
  printf("OK\n"); 

  /* Création tapis central */
  printf("Création du tapis central..."); 
  fflush(stdout); 
  if((tapis_central = tapis_creer() ) == TAPIS_NULL){
      printf("Erreur sur création du tapis central\n");
      exit(-1);
  }

  for(c = 0; c < TAPIS_NB_CARTES; c++){
    if((cr = tapis_carte_distribuer(tapis_central, c, paquet))){
	    erreur_afficher(cr); 
	    exit(-1); 
	  }
  }
  printf("OK\n"); 

  /* Création des tapis des joueurs */
  printf("Créations des %d tapis...", NbJoueurs);
  fflush(stdout); 

  if((tapis = malloc(sizeof(tapis_t *) * NbJoueurs)) == NULL){
    printf(" Erreur allocation mémoire tableau des tapis (%lu octets demandés)\n", (long unsigned int)(sizeof(tapis_t *) * NbJoueurs));
    exit(-1); 
  }

  for(t = 0; t < NbJoueurs; t++){
    if((tapis[t] = tapis_creer()) == TAPIS_NULL){
      printf("Erreur sur création du tapis %ld\n", t);
      exit(-1);
	  }

    for(c = 0; c < TAPIS_NB_CARTES; c++){
	    if((cr = tapis_carte_distribuer(tapis[t], c, paquet))){
	      if(cr == ERR_PAQUET_VIDE){
          printf("Pas assez de cartes pour tous les joueurs\n"); 
        }
	      erreur_afficher(cr) ; 
	      exit(-1) ; 
	    }
	  }
  }
  printf("OK\n"); 

  /*
   * Création et affichage de l'écran  
   */

  if((ecran = ecran_creer(tapis_central, tapis, NbJoueurs)) == NULL){
    printf("Erreur sur la création de l'écran\n"); 
    exit(-1);
  }

  ecran_message_afficher(ecran, "Début de partie: ^C pour commencer");

  joueur_t j0 = initJoueur(0,1,0); 
  joueur_t j1 = initJoueur(1,0,0); 
  joueur_t j2 = initJoueur(2,3,1); 
  joueur_t j3 = initJoueur(3,2,1); 
  joueur_t j4 = initJoueur(4,5,2); 
  joueur_t j5 = initJoueur(5,4,2); 

  /**
	 * Tableau pour contenir les threads  
	 */
	int nbThreads = NbJoueurs + 1;
	pthread_t tousLesThreads[nbThreads]; 

	/**
	 * Création des threads pour les joueurs 
	 */
	pthread_create(&tousLesThreads[0], NULL, (void *)Joueur, (void *)&j0); 
	pthread_create(&tousLesThreads[1], NULL, (void *)Joueur, (void *)&j1); 
	pthread_create(&tousLesThreads[2], NULL, (void *)Joueur, (void *)&j2); 
	pthread_create(&tousLesThreads[3], NULL, (void *)Joueur, (void *)&j3); 
  if(nbJoueurs == 6){
    pthread_create(&tousLesThreads[4], NULL, (void *)Joueur, (void *)&j4); 
    pthread_create(&tousLesThreads[5], NULL, (void *)Joueur, (void *)&j5); 
  }

	/*
	 * Création du thread pour le tapis 
	 */
	pthread_create(&tousLesThreads[nbThreads-1], NULL, (void *)Tapis, (void *)NULL); 
	
	for(i = 0; i < nbThreads; i++){
		pthread_join(tousLesThreads[i], NULL); 
	}

  /* Destruction de l'ecran */
  if((cr = ecran_detruire(&ecran))){
    fprintf(stderr, "Erreur lors de la destruction de l'ecran, cr = %d\n", cr);
    exit(-1); 
  }
    
  /* Destructions du tapis central */
  printf("Destructions du tapis central...");
  fflush(stdout); 
  if((cr = tapis_detruire(&tapis_central))){
    fprintf(stderr, " Erreur sur destruction du tapis central\n");
    erreur_afficher(cr); 
    exit(-1); 
  }
  printf("OK\n"); 
  
  /* Destructions des tapis des joueurs */
  printf("Destructions des tapis des joueurs..."); 
  fflush(stdout); 
  for(t = 0; t < NbJoueurs; t++){
    if((cr = tapis_detruire(&tapis[t]))){
      fprintf(stderr, " Erreur sur destruction du tapis %ld\n", t);
      erreur_afficher(cr); 
      exit(-1); 
    }
  }
  free(tapis);
  printf("OK\n"); 

  /*  Destruction du paquet */
  printf("\nDestruction du paquet..." );
  fflush(stdout); 
  if((cr = paquet_detruire(&paquet))){
	  fprintf(stderr, " Erreur sur destruction du paquet\n");
	  erreur_afficher(cr); 
	  exit(-1); 
  }
  printf("OK\n"); 

	/**
	 * Destruction des mutex 
	 */
	printf("\nDestruction des mutex...");
	fflush(stdout); 
	pthread_mutex_destroy(&mutexFini); 
	pthread_mutex_destroy(&mutexTapisCentral); 
	pthread_mutex_destroy(&mutexCarte1); 
	pthread_mutex_destroy(&mutexCarte2); 
	pthread_mutex_destroy(&mutexCarte3); 
	pthread_mutex_destroy(&mutexCarte4); 
 	pthread_mutex_destroy(&mutexEcran); 
 	pthread_mutex_destroy(&mutexKems); 
	printf("OK\n"); 

	printf("\n\n\t===========Fin %s==========\n\n", argv[0]);
  return(0);
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/ipc.h>

#include <ncurses.h>

#include <commun.h>
#include <paquet.h>
#include <tapis.h>
#include <ecran.h>

#include <pthread.h>

/*
	Etudiant : Dorian RENARD
*/

// Variables Globales
  paquet_t * paquet = PAQUET_NULL ;
  tapis_t * tapis_central = TAPIS_NULL ;
  tapis_t ** tapis = NULL ; /* tableau des tapis */
  tapis_id_t t = 0 ; /* Compteur de tapis */
  err_t cr = OK ;
  carte_id_t c = -1 ;
  int nbJoueur = 0;
  booleen_t fini = FAUX;
  char mess[256] ;
  ecran_t * ecran = NULL ;
  int signe[3]={0,0,0};



	// Mutex
	pthread_mutex_t mTapis = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mCarte1 = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mCarte2 = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mCarte3 = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mCarte4 = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mFini = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mEcran = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mKems = PTHREAD_MUTEX_INITIALIZER;

void arret( int sig ){
  /* printf( "Arret utilisateur\n");*/
}

//Processus Joueur
//Chaque joueur créé sera un thread par la suite
void Joueur(int i){
  carte_id_t ind_carte = -1;  
  carte_id_t ind_carte_central = -1;  
	booleen_t echange = FAUX; 
	fini = FAUX; 

  pthread_mutex_lock(&mFini); 
  while(!fini){
    pthread_mutex_unlock(&mFini); 
    
    //on test si le joueur a un carre(=4 cartes identiques)
    if(tapis_carre(tapis[i])){
      //on lock le mutex de fin de partie pour pouvoir le modifier   
      pthread_mutex_lock(&mFini); 
      //on met le boolean de fin de partie à VRAI
      fini = VRAI;
	    //on unlock le mutex car on a finit la modification
      pthread_mutex_unlock(&mFini); 
      //on lock le mutex de l'écran pour qu'il y est que le message qui s'affiche
      pthread_mutex_lock(&mEcran); 
      sprintf(mess, "Le joueur %2d crit KEMS! Les joueurs %2d et %d ont gagnés", i, i, ); 
      ecran_message_pause_afficher(ecran, mess); 
      pthread_mutex_unlock(&mEcran); 

        exit(0); 
    }
		//on lock le tapis pour pouvoir procéder aux échanges de cartes
    pthread_mutex_lock(&mTapis);

		//on recupère les indices des 2 cartes qui vont être échangées
    if((cr = tapis_cartes_choisir(&echange, tapis[i], &ind_carte, tapis_central, &ind_carte_central))){
      sprintf(mess, "Pb dans choix des cartes, code retour = %d\n", cr);
      //on lock le mutex de l'écran pour qu'il y est que le message qui s'affiche
      pthread_mutex_lock(&mEcran); 
      ecran_message_pause_afficher(ecran, mess); 
      //on unlock le mutex de l'écran car on a finit d'afficher
      pthread_mutex_unlock(&mEcran); 
      erreur_afficher(cr); 
      break; 
    }

		//si il y a un echange
		if(echange){

			//on lock la carte qui correspond à celle que le joueur veut prendre sur le tapis
    	switch(ind_carte_central){
				case 0: pthread_mutex_lock(&mCarte1); break; 
				case 1: pthread_mutex_lock(&mCarte2); break; 
				case 2: pthread_mutex_lock(&mCarte3); break; 
				case 3: pthread_mutex_lock(&mCarte4); break; 
			}

			// on procède à l'échange entre une carte d'un joueur avec une carte du tapis
      if((cr = tapis_cartes_echanger(tapis[i], ind_carte, tapis_central, ind_carte_central))){
        sprintf(mess, "Pb d'échange de cartes entre la carte %ld du tapis du joueur %d et la carte %ld du tapis central", ind_carte, i, ind_carte_central); 
      //on lock le mutex de l'écran pour qu'il y est que le message qui s'affiche
        pthread_mutex_lock(&mEcran); 
        ecran_message_pause_afficher(ecran, mess); 
      //on unlock le mutex de l'écran car on a finit d'afficher
        pthread_mutex_unlock(&mEcran); 
        erreur_afficher(cr); 
        break; 
      }	     

      sprintf(mess, "Joueur %i: Echange carte %ld avec carte %ld du tapis central ", i, ind_carte, ind_carte_central);
      //on lock le mutex de l'écran pour qu'il y est que le message qui s'affiche
      pthread_mutex_lock(&mEcran); 
      ecran_message_pause_afficher(ecran, mess); 
      ecran_cartes_echanger(ecran, f_tapis_f_carte_lire(ecran_tapis_central_lire(ecran), ind_carte_central), f_tapis_f_carte_lire(ecran_tapis_joueur_lire(ecran, i), ind_carte));
      ecran_afficher(ecran, tapis_central, tapis); 
      ecran_message_effacer(ecran);
      //on unlock le mutex de l'écran car on a finit d'afficher
      pthread_mutex_unlock(&mEcran); 

			//on unlock toutes les cartes du tapis car l'échange est finit
			switch(ind_carte_central){
				case 0: pthread_mutex_unlock(&mCarte1); break; 
				case 1: pthread_mutex_unlock(&mCarte2); break; 
				case 2: pthread_mutex_unlock(&mCarte3); break; 
				case 3: pthread_mutex_unlock(&mCarte4); break; 
			}   	 
	  
    }

		//on unlock le tapis car on a finit d'utiliser le tapis
    pthread_mutex_unlock(&mTapis);
	}
}

//Processus Tapis
// Il sert a afficher le tapis et redistribuer les cartes quand il n'y a plus d'échange
void Tapis(){
  // Verrouillage du mutexFini
	pthread_mutex_lock(&mFini); 
  while(!fini){
    // Déverrouillage du mutexFini
    pthread_mutex_unlock(&mFini); 

    sleep(3);

    // Verrouillage mutexTapisCentral
		pthread_mutex_lock(&mTapis);
    // Verrouillage mutexEcran 
 		pthread_mutex_lock(&mEcran);
 
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
    pthread_mutex_unlock(&mTapis);
     // Déverrouillage mutexEcran
 		pthread_mutex_unlock(&mEcran);
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
	if(NbJoueurs <= 0){
		printf("Le nombre de joueurs est insuffisant !\n");
		exit(0);
	}
	else if (NbJoueurs > 7){
		printf("Dans cette version, il ne peut y avoir que jusqu'à 7 joueurs !\n");
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
   * Creation et affichage de l'ecran  
   */

  if( ( ecran = ecran_creer( tapis_central , 
			     tapis , 
			     NbJoueurs ) ) == NULL ) 
    {
      printf("Erreur sur la creation de l'ecran\n"); 
      exit(-1) ;
    }

  ecran_message_afficher( ecran , "Debut de partie: ^C pour commencer");
  pause() ; 


  /**
	 * Tableau pour contenir les threads  
	 */
	int nbThreads = NbJoueurs + 1;
	pthread_t tousLesThreads[nbThreads]; 

	/**
	 * Création des threads pour les joueurs 
	 */
	for(i = 0; i < NbJoueurs; i++){
		pthread_create(&tousLesThreads[i], NULL, (void *)Joueur, i); 
	}

	/*
	 * Création du thread pour le tapis 
	 */
	pthread_create(&tousLesThreads[nbThreads-1], NULL, (void *)Tapis, (void *)NULL); 
	
	for(i = 0; i < nbThreads; i++){
		pthread_join(tousLesThreads[i], NULL); 
	}

  /* Destruction de l'ecran */
  if( ( cr = ecran_detruire( &ecran ) ) )
    {
      fprintf( stderr , "Erreur lors de la destruction de l'ecran, cr = %d\n" , cr ) ;
      exit(-1) ; 
    }
    

  /* Destructions du tapis central */
  printf( "Destructions du tapis central...") ; fflush(stdout) ; 
  if( ( cr = tapis_detruire( &tapis_central ) ) )
    {
      fprintf( stderr , " Erreur sur destruction du tapis central\n") ;
      erreur_afficher(cr) ; 
      exit(-1) ; 
    }
  printf("OK\n"); 
  
  /* Destructions des tapis des joueurs */
  printf( "Destructions des tapis des joueurs...") ; fflush(stdout) ; 
  for( t=0 ; t<NbJoueurs ; t++ )  
    {

      if( ( cr = tapis_detruire( &tapis[t] ) ) )
       {
	 fprintf( stderr , " Erreur sur destruction du tapis %ld\n"  , t ) ;
	 erreur_afficher(cr) ; 
	 exit(-1) ; 
       }

    }
  free( tapis ) ;
  printf("OK\n"); 

  /*  Destruction du paquet */
  printf("\nDestruction du paquet..." ) ; fflush(stdout) ; 
  if( ( cr = paquet_detruire( &paquet ) ) )
    {
	 fprintf( stderr , " Erreur sur destruction du paquet\n" ) ;
	 erreur_afficher(cr) ; 
	 exit(-1) ; 
    }
  printf("OK\n") ; 

  /**
	 * Destruction des mutex 
	 */
	printf("\nDestruction des mutex...");
	fflush(stdout); 
	pthread_mutex_destroy(&mFini); 
	pthread_mutex_destroy(&mTapis); 
	pthread_mutex_destroy(&mCarte1); 
	pthread_mutex_destroy(&mCarte2); 
	pthread_mutex_destroy(&mCarte3); 
	pthread_mutex_destroy(&mCarte4); 
 	pthread_mutex_destroy(&mEcran); 
	printf("OK\n"); 
 
  printf("\n\n\t===========Fin %s==========\n\n", argv[0]);
  return(0);
}


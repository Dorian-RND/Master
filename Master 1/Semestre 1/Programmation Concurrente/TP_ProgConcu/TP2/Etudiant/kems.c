#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>

#include <paquet.h>
#include <tapis.h>

#include <pthread.h>
/*
	Etudiant : Dorian RENARD
	Version 1
*/

// Variables Globales
    paquet_t * paquet = PAQUET_NULL ;
    tapis_t * tapis_central = TAPIS_NULL ;
    tapis_t ** tapis = NULL ; /* tableau des tapis */
    err_t cr = OK ;
    carte_id_t c = -1 ;
    int nbJoueur = 0;
    booleen_t fini = FAUX;
	int peutPasJouer = 0;
	int NbJoueurs = 0;

// Mutex
    pthread_mutex_t mTapis = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mCarte1 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mCarte2 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mCarte3 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mCarte4 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mFini = PTHREAD_MUTEX_INITIALIZER;

// Processus Joueur
//Chaque Joueur créé sera un Thread
void Joueur(int i){
    carte_id_t ind_carte = -1 ;
    carte_id_t ind_carte_central = -1 ;
    booleen_t echange = FAUX; 
    fini = FAUX;
	pthread_mutex_lock(&mFini);

	while(!fini){
	    pthread_mutex_unlock(&mFini);

        //Affichage du joueur
	    printf( "Tapis joueur %d\n" , i ) ;
	    tapis_stdout_afficher( tapis[i] ) ;
	    printf( "\n" );

	    // On test si le joueur i a un carre(=4 cartes identiques)
	    if(tapis_carre(tapis[i])){
			//on lock le mutex de fin de partie pour pouvoir le modifier
	        pthread_mutex_lock(&mFini);
			//on met le boolean de fin de partie à VRAI
	        fini = VRAI;
			//on unlock le mutex car on a finit la modification
	        pthread_mutex_unlock(&mFini);
            printf( "*----------------------*\n") ;
            printf( "* Le joueur%2d a gagné *\n" , i ) ;
            printf( "*----------------------*\n") ;
            break ;  /* Sort de la boucle des joueurs */
	    }

		//on lock le tapis pour pouvoir procéder aux échanges de cartes
		pthread_mutex_lock(&mTapis);

		//on recupère les indices des 2 cartes qui vont être échangées
        if( ( cr = tapis_cartes_choisir( &echange , tapis[i] , &ind_carte , tapis_central , &ind_carte_central) ) ){
            printf( "Pb dans choix des cartes, code retour = %d\n", cr ) ;
            erreur_afficher(cr) ;
            exit(-1) ;
        }

		//si il y a un echange
		if( echange ){
			peutPasJouer = 0;
			//on lock la carte qui correspond à celle que le joueur veut prendre sur le tapis
        	switch (ind_carte_central){
        		case 0: pthread_mutex_lock(&mCarte1);
        			break;
        		case 1: pthread_mutex_lock(&mCarte2);
        			break;
        		case 2: pthread_mutex_lock(&mCarte3);
        			break;
        		case 3: pthread_mutex_lock(&mCarte4);
        			break;
        	}

			// on procède à l'échange entre une carte d'un joueur avec une carte du tapis
            if( ( cr = tapis_cartes_echanger( tapis[i] , ind_carte , tapis_central , ind_carte_central ) ) ){
                printf( "Pb d'echange de cartes entre la carte %ld du tapis du joueur %d\n" , ind_carte , i );
                carte_stdout_afficher( tapis_carte_lire( tapis[i] , ind_carte ) ) ;
                printf( "\n et la carte %ld du tapis central\n" , ind_carte_central );
                carte_stdout_afficher( tapis_carte_lire( tapis_central , ind_carte_central ) ) ;
                erreur_afficher(cr) ;
                exit(-1) ;
            }

			//on unlock toutes les cartes du tapis car l'échange est finit
        	switch (ind_carte_central){
        		case 0: pthread_mutex_unlock(&mCarte1);
        			break;
        		case 1: pthread_mutex_unlock(&mCarte2);
        			break;
        		case 2: pthread_mutex_unlock(&mCarte3);
        			break;
        		case 3: pthread_mutex_unlock(&mCarte4);
        			break;
        	}
        }
		else{
			peutPasJouer ++;
		}

		//on unlock le tapis car on a finit d'utiliser le tapis
    	pthread_mutex_unlock(&mTapis);
	}
}

// Processus Tapis
// Il sert a afficher le tapis et redistribuer les cartes quand il n'y a plus d'échange
void Tapis(){
	pthread_mutex_lock(&mFini);
    while(!fini){
		pthread_mutex_unlock(&mFini);
		if(peutPasJouer > NbJoueurs * 2){
			/* Affichage Central */
			printf( "Tapis central \n" ) ;
			tapis_stdout_afficher( tapis_central ) ;
			printf( "\n" );

			pthread_mutex_lock(&mTapis);
			/*
			* Pas un seul echange des joueur
			* --> redistribution du tapis central
			*/
			printf( "Redistribution tapis central\n") ;
			for( c=0 ; c<TAPIS_NB_CARTES ; c++ ){
				if( ( cr = tapis_carte_retirer( tapis_central , c , paquet ) ) ){
					printf( "Pb dans retrait d'une carte du tapis central\n" );
					erreur_afficher(cr) ;
					exit(-1) ;
				}

				if( ( cr = tapis_carte_distribuer( tapis_central , c , paquet ) ) ){
					printf( "Pb dans distribution d'une carte pour le tapis central\n" );
					erreur_afficher(cr) ;
					exit(-1) ;
				}
			}
			//on unlock le tapis car on a finit d'utiliser le tapis
			pthread_mutex_unlock(&mTapis);
		}
		
	}
}

int main( int argc , char * argv[] ){
    int i;

    printf("\n\n\n\t===========Debut %s==========\n\n" , argv[0] );

    if( argc != 2 ){
    	printf( " Programme de test des joueurs de Kems\n" );
    	printf( " usage : %s <Nb joueurs>\n" , argv[0] );
    	exit(0);
    }

    if(atoi( argv[1] ) <= 0){
    	printf( " Nombre de joueurs insuffisant ! \n" );
    	exit(0);
    }

    NbJoueurs  = atoi( argv[1] ) ;

    srandom(getpid());

    printf("Creation du paquet de cartes\n") ;
    if( ( paquet = paquet_creer() ) == PAQUET_NULL ){
    	printf("Erreur sur creation du paquet\n" ) ;
    	exit(-1) ;
    }

    printf("Creation du tapis central\n")  ;
    if( ( tapis_central = tapis_creer() ) == TAPIS_NULL ){
    	printf("Erreur sur creation du tapis central\n" ) ;
    	exit(-1) ;
    }

    for( c=0 ; c<TAPIS_NB_CARTES ; c++ ){
    	if( ( cr = tapis_carte_distribuer( tapis_central  , c , paquet ) ) ){
    		erreur_afficher(cr) ;
    		exit(-1) ;
    	}
    }

    printf( "Tapis Central\n" ) ;
    tapis_stdout_afficher( tapis_central ) ;
    printf("\n");

    printf("Creation des %d tapis des joueurs\n" , NbJoueurs ) ;
    if( ( tapis = malloc( sizeof(tapis_t *) * NbJoueurs ) ) == NULL ){
    	printf(" Erreur allocation memoire tableau des tapis (%lu octets demandes)\n" ,(long unsigned int)(sizeof(tapis_t *) * NbJoueurs) ) ;
    	exit(-1) ;
    }

    for( i=0 ; i<NbJoueurs ; i++ ){
    	if( ( tapis[i] = tapis_creer() ) == TAPIS_NULL ){
    		printf("Erreur sur creation du tapis %d\n" , i ) ;
    		exit(-1) ;
    	}

    	for( c=0 ; c<TAPIS_NB_CARTES ; c++ ){
    		if( ( cr = tapis_carte_distribuer( tapis[i]  , c , paquet ) ) ){
    			if( cr == ERR_PAQUET_VIDE ) printf("Pas assez de cartes pour tous les joueurs\n");
    			erreur_afficher(cr) ;
    			exit(-1) ;
    		}
    	}

    	printf( "Tapis joueur %d\n" , i ) ;
    	tapis_stdout_afficher( tapis[i] ) ;
    	printf("\n");
    }

    int nThread = NbJoueurs + 1;

    pthread_t tThread[nThread];

    for(i = 0; i<NbJoueurs; i++){
    	pthread_create(&tThread[i], NULL, (void *)Joueur, i);
    }

    pthread_create(&tThread[nThread - 1], NULL, (void *)Tapis, (void *)NULL);

    for(i = 0; i<nThread; i++){
    	pthread_join(tThread[i], NULL);
    }

    printf("\nDestruction des tapis..." ) ; fflush(stdout) ;
     for (i=0 ; i<NbJoueurs ; i++ ){
    	if( ( cr = tapis_detruire( &tapis[i] ) ) ){
    	    printf(" Erreur sur destruction du tapis du joueur %d\n"  , i ) ;
    	    erreur_afficher(cr) ;
    	    exit(-1) ;
        }
     }
     printf("OK\n") ;


     printf("\nDestruction du paquet..." ) ; fflush(stdout) ;
     paquet_detruire( &paquet ) ;
     printf("OK\n") ;

     printf("\n\n\t===========Fin %s==========\n\n" , argv[0] );

    pthread_mutex_destroy(&mFini);
    pthread_mutex_destroy(&mTapis);
    pthread_mutex_destroy(&mCarte1);
    pthread_mutex_destroy(&mCarte2);
    pthread_mutex_destroy(&mCarte3);
    pthread_mutex_destroy(&mCarte4);

    return 0;
}
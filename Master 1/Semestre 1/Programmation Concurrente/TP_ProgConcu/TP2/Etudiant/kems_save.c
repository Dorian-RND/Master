#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <paquet.h>
#include <tapis.h>

int
main( int argc , char * argv[] ) 
{

  paquet_t * paquet = PAQUET_NULL ;

  tapis_t * tapis_central = TAPIS_NULL ;
  tapis_t ** tapis = NULL ; /* tableau des tapis */
  int i = 0 ;
  err_t cr = OK ;

  carte_id_t c = -1 ;
  carte_id_t ind_carte = -1 ;
  carte_id_t ind_carte_central = -1 ; 

  booleen_t fini = FAUX ;
  booleen_t echange = FAUX ;

  printf("\n\n\n\t===========Debut %s==========\n\n" , argv[0] );

 if( argc != 2 ) 
   {
     printf( " Programme de test des joueurs de Kems\n" );
     printf( " usage : %s <Nb joueurs>\n" , argv[0] );
     exit(0); 
   }

  int NbJoueurs  = atoi( argv[1] ) ;

  srandom(getpid());

  printf("Creation du paquet de cartes\n") ;
  if( ( paquet = paquet_creer() ) == PAQUET_NULL )
    {
      printf("Erreur sur creation du paquet\n" ) ;
      exit(-1) ; 
    }

  printf("Creation du tapis central\n")  ;
  if( ( tapis_central = tapis_creer() ) == TAPIS_NULL )
    {
      printf("Erreur sur creation du tapis central\n" ) ;
      exit(-1) ;
    }

  for( c=0 ; c<TAPIS_NB_CARTES ; c++ )
    {
      if( ( cr = tapis_carte_distribuer( tapis_central  , c , paquet ) ) )
	{
	  erreur_afficher(cr) ; 
	  exit(-1) ; 
	}
    }

  printf( "Tapis Central\n" ) ;
  tapis_stdout_afficher( tapis_central ) ;
  printf("\n");

  printf("Creation des %d tapis des joueurs\n" , NbJoueurs ) ;
  if( ( tapis = malloc( sizeof(tapis_t *) * NbJoueurs ) ) == NULL )
    {
      printf(" Erreur allocation memoire tableau des tapis (%lu octets demandes)\n" , 
	     (long unsigned int)(sizeof(tapis_t *) * NbJoueurs) ) ;
      exit(-1) ; 
    }
	
  for( i=0 ; i<NbJoueurs ; i++ ) 
    {
      if( ( tapis[i] = tapis_creer() ) == TAPIS_NULL )
	{
	  printf("Erreur sur creation du tapis %d\n" , i ) ;
	  exit(-1) ;
	}

      for( c=0 ; c<TAPIS_NB_CARTES ; c++ )
	{
	  if( ( cr = tapis_carte_distribuer( tapis[i]  , c , paquet ) ) )
	    {
	      if( cr == ERR_PAQUET_VIDE ) printf("Pas assez de cartes pour tous les joueurs\n"); 
	      erreur_afficher(cr) ; 
	      exit(-1) ; 
	    }
	}

      printf( "Tapis joueur %d\n" , i ) ;
      tapis_stdout_afficher( tapis[i] ) ;
      printf("\n");
    }

  /* Phase de jeu */
  fini = FAUX ; 
  while( ! fini ) 
    {
      /* Affichage Central */
      printf( "Tapis central \n" ) ;
      tapis_stdout_afficher( tapis_central ) ;
      printf( "\n" ); 
      
      echange=FAUX ; 

      for( i=0 ; i<NbJoueurs ; i++ ) /* boucle des joueurs */
	{
	  /* Affichage Joueur */
	  printf( "Tapis joueur %d\n" , i ) ;
	  tapis_stdout_afficher( tapis[i] ) ;
	  printf( "\n" ); 

	  /* Test arret */
	  if( tapis_carre( tapis[i] ) )
	    {
	      fini = VRAI ;
	      printf( "*----------------------*\n") ; 
	      printf( "* Le joueur %2d a gagne *\n" , i ) ;
	      printf( "*----------------------*\n") ; 
	      break ;  /* Sort de la boucle des joueurs */
	    }

	  if( ( cr = tapis_cartes_choisir( &echange , tapis[i] , &ind_carte , tapis_central , &ind_carte_central) ) )
	    {
	      printf( "Pb dans choix des cartes, code retour = %d\n", cr ) ;
	      erreur_afficher(cr) ; 
	      exit(-1) ; 
	    }

	  if( echange ) 
	    {
	      if( ( cr = tapis_cartes_echanger( tapis[i] , ind_carte , tapis_central , ind_carte_central ) ) )
		{
		  printf( "Pb d'echange de cartes entre la carte %ld du tapis du joueur %d\n" , ind_carte , i ); 
		  carte_stdout_afficher( tapis_carte_lire( tapis[i] , ind_carte ) ) ; 
		  printf( "\n et la carte %ld du tapis central\n" , ind_carte_central ); 
		  carte_stdout_afficher( tapis_carte_lire( tapis_central , ind_carte_central ) ) ; 
		  erreur_afficher(cr) ; 
		  exit(-1) ; 
		}	     	 
	    }
	}

      if( ! echange ) 
	{
	  /* 
	   * Pas un seul echange des joueur 
	   * --> il y a une redistribution du tapis central 
	   */
	  printf( "Redistribution tapis central\n") ; 
	  for( c=0 ; c<TAPIS_NB_CARTES ; c++ )
	    {
	      if( ( cr = tapis_carte_retirer( tapis_central , c , paquet ) ) )
		{
		  printf( "Pb dans retrait d'une carte du tapis central\n" ); 
		  erreur_afficher(cr) ; 
		  exit(-1) ; 
		}
	  
	      if( ( cr = tapis_carte_distribuer( tapis_central , c , paquet ) ) )
		{
		  printf( "Pb dans distribution d'une carte pour le tapis central\n" ); 
		  erreur_afficher(cr) ; 
		  exit(-1) ; 
		}
	    }
	}

    }

 printf("\nDestruction des tapis..." ) ; fflush(stdout) ; 
 for (i=0 ; i<NbJoueurs ; i++ ) 
   {
     if( ( cr = tapis_detruire( &tapis[i] ) ) )
       {
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
 
 return(0) ;
}


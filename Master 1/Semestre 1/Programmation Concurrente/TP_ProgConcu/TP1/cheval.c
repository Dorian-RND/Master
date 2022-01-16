#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <commun.h>
#include <liste.h>
#include <piste.h>

/* machine : ic2s130-19.univ-lemans.fr
  clés shm + sem piste : 11
  clés shm + sem liste : 12

  ssh nom_machine
*/

struct sembuf op_p = {0, -1, 0};
struct sembuf op_v = {0, 1, 0};

int 
main( int nb_arg , char * tab_arg[] )
{     

  int cle_piste ;
  piste_t * piste = NULL ;

  int cle_liste ;
   liste_t * liste = NULL ;

  char marque ;

  booleen_t fini = FAUX ;
  piste_id_t deplacement = 0 ;
  piste_id_t depart = 0 ;
  piste_id_t arrivee = 0 ;

  
  cell_t cell_cheval ;


  elem_t elem_cheval ;



  /*-----*/

  if( nb_arg != 4 )
    {
      fprintf( stderr, "usage : %s <cle de piste> <cle de liste> <marque>\n" , tab_arg[0] );
      exit(-1);
    }
  
  if( sscanf( tab_arg[1] , "%d" , &cle_piste) != 1 )
    {
      fprintf( stderr, "%s : erreur , mauvaise cle de piste (%s)\n" , 
	       tab_arg[0]  , tab_arg[1] );
      exit(-2);
    }


  if( sscanf( tab_arg[2] , "%d" , &cle_liste) != 1 )
    {
      fprintf( stderr, "%s : erreur , mauvaise cle de liste (%s)\n" , 
	       tab_arg[0]  , tab_arg[2] );
      exit(-2);
    }

  if( sscanf( tab_arg[3] , "%c" , &marque) != 1 )
    {
      fprintf( stderr, "%s : erreur , mauvaise marque de cheval (%s)\n" , 
	       tab_arg[0]  , tab_arg[3] );
      exit(-2);
    }
    
/* Récupération des id de mémoires partagées de la liste et de la piste*/
int shmid_liste;
if((shmid_liste = shmget(cle_liste, sizeof(liste_t), 0666)) == -1){
  perror ("Erreur lors du shmget pour la liste");
  exit(-1);
}

int shmid_piste;
if((shmid_piste = shmget(cle_piste, sizeof(piste_t), 0666)) == -1){
  perror ("Erreur lors du shmget pour la liste");
  exit(-1);
}

liste = shmat(shmid_liste, 0, 0);
piste = shmat(shmid_piste, 0, 0);

/* Récupération des sémaphores de la liste et de la piste*/
int Sliste;
if((Sliste = semget(cle_liste, 1, 0666)) == -1){
  perror ("Erreur lors du semget pour la liste");
  exit(-1);
}

int Spiste;
if((Spiste = semget(cle_piste, 1, 0666)) == -1){
  perror ("Erreur lors du semget pour la liste");
  exit(-1);
}




  /* Init de l'attente */
  commun_initialiser_attentes() ;


  /* Init de la cellule du cheval pour faire la course */
  cell_pid_affecter( &cell_cheval  , getpid());
  cell_marque_affecter( &cell_cheval , marque );

  /* Init de l'element du cheval pour l'enregistrement */
  elem_cell_affecter(&elem_cheval , cell_cheval ) ;
  elem_etat_affecter(&elem_cheval , EN_COURSE ) ;

  /* 
   * Enregistrement du cheval dans la liste
   */

  elem_sem_creer(&elem_cheval);

  /* Avant d'ajouter le cheval dans la liste, on fait un P sur le sémaphore de la liste*/
  semop(Sliste, &op_p, 1);
  liste_elem_ajouter(liste, elem_cheval);
  semop(Sliste, &op_v, 1);

/*Il faut savoir où est placé le cheval dans la liste, on récupère donc sa position avec i*/
  int i;
  liste_elem_rechercher(&i, liste, elem_cheval);

  while( ! fini )
    {
      /* Attente entre 2 coup de de */
      commun_attendre_tour() ;

      /* 
       * Verif si pas decanille 
       */
      semop(Sliste, &op_p, 1);
      if(elem_decanille(elem_cheval)){
        fini = VRAI;

        /*Pour modifier la piste on doit être le seul à le faire donc on fait un P sur la piste pour s'assurer d'être le seul*/
        semop(Spiste, &op_p, 1);
        piste_cell_effacer(piste, i);
        semop(Spiste, &op_v, 1);

        liste_elem_rechercher(&i, liste, elem_cheval);
        liste_elem_supprimer(liste, i);

        /*On a finit de modifier la liste on fait donc un V dessus pour la libérer*/
        semop(Sliste, &op_v, 1);

        elem_sem_deverrouiller(&elem_cheval);
        elem_sem_detruire(&elem_cheval);
      }
      else{
        semop(Sliste, &op_v, 1);
      }

      /*
       * Avancee sur la piste
       */

      /* Coup de de */
      deplacement = commun_coup_de_de() ;
#ifdef _DEBUG_
      printf(" Lancement du De --> %d\n", deplacement );
#endif

      arrivee = depart+deplacement ;

      if( arrivee > PISTE_LONGUEUR-1 )
	{
	  arrivee = PISTE_LONGUEUR-1 ;
	  fini = VRAI ;
	}

      if( depart != arrivee )
	{

	  /* 
	   * Si case d'arrivee occupee alors on decanille le cheval existant 
     * Il faut supprimer le cheval de la liste et de la piste
	   */
	   elem_sem_verrouiller(&elem_cheval);

    if(piste_cell_occupee(piste, arrivee)){
        cell_t cellChevalADecaniller;
        elem_t chevalADecaniller;
        int posCheval;

        if(piste_cell_lire(piste, arrivee, &cellChevalADecaniller) != 1){
            perror("Problème lors de la lecture de la piste");
            exit(-2);
        }

        /*On décanille le cheval d'avant*/
        elem_cell_affecter(&chevalADecaniller, cellChevalADecaniller);

        /*On veut pas que la liste soit changée quand on fait des recherches*/
        semop(Sliste, &op_p, 1);
        liste_elem_rechercher(&posCheval, liste, chevalADecaniller);

        elem_sem_verrouiller(&chevalADecaniller);
        liste_elem_decaniller(liste, posCheval);
        elem_sem_deverrouiller(&chevalADecaniller);
        semop(Sliste, &op_v, 1);
    }
	       
	  /* 
	   * Deplacement: effacement case de depart, affectation case d'arrivee 
	   */

    /*On efface notre cheval de la cellule de départ*/
    piste_cell_effacer(piste, depart);
    /*On attend d'avoir sauter pour devérouiller notre cheval*/
    commun_attendre_fin_saut();
    elem_sem_deverrouiller(&elem_cheval);

    /*On déplace notre cheval sur la case d'arrivée*/
    piste_cell_affecter(piste, arrivee, cell_cheval);

#ifdef _DEBUG_
	  printf("Deplacement du cheval \"%c\" de %d a %d\n",
		 marque, depart, arrivee );
#endif

	  
	} 
      /* Affichage de la piste  */
      piste_afficher_lig( piste );
     
      depart = arrivee ;
    }

  printf( "Le cheval \"%c\" A FRANCHIT LA LIGNE D ARRIVEE\n" , marque );

 
     
  /* 
   * Suppression du cheval de la liste 
   */
   semop(Spiste, &op_p, 1);
   piste_cell_effacer(piste, depart);
   semop(Spiste, &op_v, 1);

   semop(Sliste, &op_p, 1);
   liste_elem_supprimer(liste, i);
   semop(Sliste, &op_v, 1);
  
  exit(0);
}

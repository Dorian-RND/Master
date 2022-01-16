#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include <sens.h>
#include <train.h>
#include <moniteur_voie_unique.h>

/**
 * Etudiant : Dorian RENARD
 */

/*---------- MONITEUR ----------*/

extern moniteur_voie_unique_t * moniteur_voie_unique_creer(const train_id_t nb){
  moniteur_voie_unique_t * moniteur = NULL; 

  /* Création structure moniteur */
  if((moniteur = malloc(sizeof(moniteur_voie_unique_t))) == NULL){
    fprintf(stderr, "moniteur_voie_unique_creer: débordement memoire (%lu octets demandés)\n", sizeof(moniteur_voie_unique_t));
    return (NULL); 
  }

  /* Création de la voie */
  if((moniteur->voie_unique = voie_unique_creer()) == NULL) 
    return (NULL); 
  
  /* Initialisation des variables du moniteur */
  pthread_mutex_init(&moniteur->mutexVoie, NULL); 
  moniteur->nbMaxTrainsSurLaVoie = 0; 
  moniteur->nbTrainOuestEst = 0; 
  moniteur->nbTrainEstOuest = 0;
  moniteur->nbTrainOuestEstAttente = 0; 
  moniteur->nbTrainEstOuestAttente = 0; 

  return (moniteur); 
}

extern int moniteur_voie_unique_detruire (moniteur_voie_unique_t ** moniteur){
  int noerr; 

  /* Destructions des attributs du moniteur */
  pthread_mutex_destroy(&(*moniteur)-> mutexVoie);
  pthread_cond_destroy(&(*moniteur) -> condAccesOuest);
  pthread_cond_destroy(&(*moniteur) -> condAccesEst);

  /* Destruction de la voie */
  if((noerr = voie_unique_detruire(&((*moniteur)->voie_unique))))
    return (noerr);

  /* Destruction de la structure du moniteur */
  free((*moniteur));

  (*moniteur) = NULL; 

  return (0); 
}

extern void moniteur_voie_unique_entree_ouest(moniteur_voie_unique_t * moniteur){

  // On verrouille le mutex sur la voie 
  pthread_mutex_lock(&moniteur->mutexVoie);

  // S'il y a au moins un train Est-Ouest sur la voie, le train Ouest-Est ne peut pas entrer sur la voie donc il est mis en attente.  
  if(moniteur->nbTrainEstOuest > 0){
    moniteur->nbTrainOuestEstAttente++; 
    pthread_cond_wait(&moniteur->condAccesOuest, &moniteur->mutexVoie); 
  }

  // On incrémente le nombre de trains Ouest-Est
  moniteur->nbTrainOuestEst++; 

  // S'il y a des trains Ouest-Est en attente, on débloque la voie du côté Ouest pour que les trains passent 
  if(moniteur->nbTrainOuestEstAttente > 0){
    moniteur->nbTrainOuestEstAttente--; 
    pthread_cond_signal(&moniteur->condAccesOuest); 
  }

  // On déverrouille le mutex sur la voie 
  pthread_mutex_unlock(&moniteur->mutexVoie);
}

extern void moniteur_voie_unique_sortie_est(moniteur_voie_unique_t * moniteur){

  // On verrouille le mutex sur la voie 
  pthread_mutex_lock(&moniteur->mutexVoie); 

  // On décrémente le nombre de trains Ouest-Est
  moniteur->nbTrainOuestEst--;
  // S'il n'y a pas de trains Ouest-Est sur la voie et qu'il y a des trains Est-Ouest en attente, on ouvre la voie côté Est 
  if((moniteur->nbTrainOuestEst == 0) && (moniteur->nbTrainEstOuestAttente > 0)){
    moniteur->nbTrainEstOuestAttente--; 
    pthread_cond_signal(&moniteur->condAccesEst); 
  }

  // On déverrouille le mutex sur la voie 
  pthread_mutex_unlock(&moniteur->mutexVoie); 
}

extern void moniteur_voie_unique_entree_est(moniteur_voie_unique_t * moniteur){

  // On verrouille le mutex sur la voie 
   pthread_mutex_lock(&moniteur->mutexVoie);

  // S'il y a au moins un train Ouest-Est sur la voie, le train Est-Ouest ne peut donc pas entrer sur la voie donc il est mis en attente.  
  if(moniteur->nbTrainOuestEst > 0){
    moniteur->nbTrainEstOuestAttente++; 
    pthread_cond_wait(&moniteur->condAccesEst, &moniteur->mutexVoie); 
  }

  // On incrémente le nombre de trains Est-Ouest
  moniteur->nbTrainEstOuest++; 
  // S'il y a des trains Est-Ouest en attente, on débloque la voie du côté Est pour que les trains passent 
  if(moniteur->nbTrainEstOuestAttente > 0){
    moniteur->nbTrainEstOuestAttente--; 
    pthread_cond_signal(&moniteur->condAccesEst); 
  }

  // On déverrouille le mutex sur la voie 
  pthread_mutex_unlock(&moniteur->mutexVoie);
}

extern void moniteur_voie_unique_sortie_ouest(moniteur_voie_unique_t * moniteur){

  // On verrouille le mutex sur la voie 
  pthread_mutex_lock(&moniteur->mutexVoie); 

  // On décrémente le nombre de trains Est-Ouest
  moniteur->nbTrainEstOuest--;
  // S'il n'y a pas de trains Est-Ouest sur la voie et qu'il y a des trains Ouest-Est en attente, on ouvre la voie côté Ouest
  if((moniteur->nbTrainEstOuest == 0) && (moniteur->nbTrainOuestEstAttente > 0)){
    moniteur->nbTrainOuestEstAttente--; 
    pthread_cond_signal(&moniteur->condAccesOuest); 
  }

  // On déverrouille le mutex sur la voie 
  pthread_mutex_unlock(&moniteur->mutexVoie); 
}

/*
 * Fonctions set/get 
 */

extern 
voie_unique_t * moniteur_voie_unique_get(moniteur_voie_unique_t * const moniteur){
  return (moniteur->voie_unique); 
}


extern 
train_id_t moniteur_max_trains_get(moniteur_voie_unique_t * const moniteur){
  return (moniteur->nbMaxTrainsSurLaVoie);
}

/*
 * Fonction de déplacement d'un train 
 */

extern
int moniteur_voie_unique_extraire(moniteur_voie_unique_t * moniteur, train_t * train, zone_t zone){
  return (voie_unique_extraire(moniteur->voie_unique, (*train), zone, train_sens_get(train))); 
}

extern
int moniteur_voie_unique_inserer(moniteur_voie_unique_t * moniteur, train_t * train, zone_t zone){ 
  return (voie_unique_inserer(moniteur->voie_unique, (*train), zone, train_sens_get(train)));
}

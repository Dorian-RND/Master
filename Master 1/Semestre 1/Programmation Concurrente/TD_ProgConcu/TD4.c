/*
Pb du coiffeur endormi 
0)  Si coiffeur n’a pas de clients —> il s’endort
1)  Si coiffeur libre —> il coiffe le client 
2)  Si coiffeur occupé —> le client attend sur un chaise
3)  Si coiffeur occupé + toutes les chaises sont occupées —> le client s’en va 

But : Faire le code 
du coiffeur 
Des clients
=> S’exécute en même temps —> RDV de procédure 

On suppose que : 
coiffer ( )
se_fait_coiffer ( )
*/

// 1ère solution : outils de bas niveau —> les sémaphores avec P + V
//Variabels Globales
Scoiff = 1
cpt_client = 0
Scpt_client = 0
mutex //protège la variable cpt_client
NB_CHAISE 5

coiffeur(){
    while(pas fermeture){
        P(Scpt_client);
        P(Scoiff);
        coiffer();
    }
}

client(){
    P(mutex);
    if(cpt_client >= NB_CHAISE){
        V(mutex);
        exit(0);
    }
    else{
        cpt_client ++;
        V(Scpt_client);
    }
    V(mutex);
    P(Scoiff);
    se_fait_coiffer();
    P(mutex);
    cpt_client--;
    V(mutex);
}

//2ème solution : les moniteurs avec signal + wait
/*
procedures: 
    - debut_coiffure();
    - fin_coiffure();
    - debut_se_fait_coiffer();
    fin_se_fait_coiffer();
*/
NB_CHAISES 5
cpt_client = 0
boolean coiff_occupe = FAUX
cond Ccoiffeur;
cond Cclient;

coiffeur(){
    while(...){
        debut_coiffure();
        coiffer();
        fin_coiffure();
    }
}

client(){
    debut_se_fait_coiffer();
    se_fait_coiffer();
    fin_se_fait_coiffer();
}

debut_coiffure(){
    if(cpt_client == 0){
        wait(Ccoiffure);
    }
    coiff_occupe = VRAI;
}

fin_coiffure(){
    coiff_occupe = FAUX;
    if(cpt_client > 0){
        signal(Cclient);
    }
}

debut_se_fait_coiffer(){
    if(cpt_client >= NB_CHAISES){
        exit(0);
    }
    cpt_client++;
    if(cpt_client == 1){
        signal(Ccoiffeur);
    }
    if(coiff_occupe){
        wait(Cclient);
    }
}

fin_se_fait_coiffer(){
    cpt_client --;
}



/* Avec les Threads */
#define NB_CHAISES 5
int cpt_clients; 
boolean_t coiff_occupe;
pthread_cond_t Ccoiffeur, Cclient;
pthread_mutex_t mutex;

int init_moniteur(){
    cpt_clients = 0;
    coiff_occupe = FALSE;
    pthread_cond_init(&Ccoiffeur, NULL);
    pthread_cond_init(&Cclient, NULL);
    pthread_mutex_init(&mutex, NULL);
}

int debut_se_fait_coiffe(){
    pthread_mutex_lock(&mutex);
    if(cpt_clients > NB_CHAISES)
        exit(0);
    cpt_clients ++;
    if(cpt_clients == 1)
        phtread_cond_signal(Ccoiffeur);
    if(coiff_occupe)
        phtread_cond_wait(&Cclient, &mutex);
    pthread_mutex_unlock(&mutex);
}

int debut_coiffure(){
    pthread_mutex_lock(&mutex);
    if(cpt_clients == 0)
        pthread_cond_wait(&Ccoiffeur, &mutex);
    coiff_occupe = TRUE;
    pthread_mutex_unlock(&mutex);
}

int fin_coiffure(){
    pthread_mutex_lock(&mutex);
    cpt_clients --;
    coiff_occupe = FALSE;
    if(cpt_clients > 0)
        pthread_cond_signal(&Cclient);
    phtread_mutex_unlock(&mutex);
}

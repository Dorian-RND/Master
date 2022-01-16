/* Exercice 1 */
creeListe(N, Res):- creeListe(N, [], Res),!.
creeListe(0, Liste, Liste).
creeListe(N, Liste, Res):- M is N-1, creeListe(M, [M|Liste], Res). 

/* Exercice 2 */
maxDeListe([X], X).
maxDeListe([X|Reste], X):- maxDeListe(Reste, MaxDuReste), X > MaxDuReste.
maxDeListe([X|Reste], MaxDuReste):- maxDeListe(Reste, MaxDuReste), X =< MaxDuReste.

/* Exercice 3 
milieuEtudiant([Note],[Nom], Note, Nom).
milieuEtudiant([Note|ResteNote], [Nom|ResteNom], Note, Nom):- milieuEtudiant(ResteNote, ResteNom, MaxDesAutresNotes, _), Note > MaxDesAutresNotes.
milieuEtudiant([Note|ResteNote], [_|ResteNom], MaxDesAutresNotes, NomDuMaxDesAutres):- milieuEtudiant(ResteNote, ResteNom, ).
*/
/* Exercice 4 */
meilleurPaire([Note, Nom], [Note, Nom]).
meilleurPaire([[Note, Nom]|Autres], [Nom, Note]):- meilleurPaire(Autres, [MaxDesAutres, _]), Note > MaxDesAutres.
meilleurPaire([[Note, _], Autres], [MaxDesAutres, LesAutres]):- meilleurPaire(Autres, [MaxDesAutres, LesAutres]), Note =< MaxDesAutres.

/* Exercice 5 */
carre(Nb, Carre):- Carre is Nb*Nb.
construitCarree([Nb], [Nb,C]):- C is Nb*Nb.
construitCarre([Nb|Reste1], [[Nb,C]|Reste2]):-construitCarre(Reste1, Reste2), C is Nb*Nb.

/* Exercice 6 */
eleve2(Nb, Puis, Res):- Res is Nb^Puis.
eleve(X,1,X):-!.
eleve(X,N,XpuissN):- Nmoins1 is N-1, eleve(X, Nmoins1, XpuissNmoins1), XpuissN is X*XpuissNmoins1.

/* Exercice 7 */
eleveListe([],_, []):- !.
eleveListe([X|ResteX], N, [[X,XpuissN]|ResteEtudiant]):- eleve(X,N,XpuissN), eleveListe(ResteX, N, ResteEtudiant).

/* Exercice 8 */
donneLesBornes()


% Predicat : ligneDeGrille(NumLigne, Grille, Ligne).
% Satisfait si Ligne est la ligne numero NumLigne dans la Grille
% tes : grilleDeDepart(X),ligneDeGrille(1,X,E).
% donnne la ligne souhaiter dans la variable Test
ligneDeGrille(1, [Test |_], Test).
ligneDeGrille(NumLigne, [_|Reste],Test) :- succNum(I, NumLigne),
		ligneDeGrille(I,Reste,Test).

% Predicat : caseDeLigne(Col, Liste, Valeur).
% Satisfait si Valeur est dans la colonne Col de la Liste
% test : grilleDeDepart(X),caseDeLigne(a,X,E).
% donne la collone de la grille de jeu
caseDeLigne(a, [A|_],A).
caseDeLigne(Col, [_|Reste],Test) :- succAlpha(I, Col),caseDeLigne(I,Reste, Test).



% Predicat : afficheCaseDeGrille(Colonne,Ligne,Grille) .
% grilleDeDepart(X),afficheCaseDeGrille(a,2,X).
%-
%X = [[-, -, -], [-, -, -], [-, -, -]] ;
% affiche la valeur de la case
afficheCaseDeGrille(C,L,G) :- caseDeGrille(C,L,G,Case),write(Case).


% Predicat : leCoupEstValide/3
%leCoupEstValide(a,2,[[-, -, -], [-, -, -], [-, -, -]]).
% true

% test grilleDeDepart(X),leCoupEstValide(a,2,[[-, -, -], [a, -, -], [-, -, -]]).
% X = [[-, -, -], [x, -, -], [-, -, -]] ;
% =True
% permet de verifier si dans la case selectioner l element est un -
leCoupEstValide(C,L,G) :- caseDeGrille(C,L,G,-).


% Predicat : ligneFaite/2
%
% version bof
% ligneFaiteBof(x,[x,x,x]).
% ligneFaiteBof(o,[o,o,o]).
% ligneFaiteBof(-,[-,-,-]).

%ligneFaite(1,[-,-,-]).
%false.

%?- ligneFaite(1,[1,1,1]).
%true 

% permet de verifier si tuoutes les valeur de la ligne corresponde a la valeur que l ont lui à renseigné
ligneFaite(Val, [Val]).
ligneFaite(Val, [Val|R]) :- ligneFaite(Val, R).


% Predicat : ligneExiste/3
% ?- ligneExiste(x,[[x,-,x],[x,x,x],[-,o,-]],V).
% V = 2 ;
% ligneExiste(-,[[-,-,-],[x,x,x],[-,o,-]],V).
%V = 1
% nous donne le numero de la ligne ou les valeur correpondent 
ligneExiste(Val, [L1|_], 1) :- ligneFaite(Val, L1).
ligneExiste(Val, [_|R], NumLigne) :- succNum(I,NumLigne), ligneExiste(Val, R, I).



% Predicat : colonneExiste/3
%colonneExiste(-,[[-,-,-],[x,x,x],[-,o,-]],V).
%false.

%?- colonneExiste(-,[[-,-,-],[-,x,x],[-,o,-]],V).
%V = a
% Donne le numero de la colonne ou les valeur sont egaux 
colonneExiste(Val, [[Val|_],[Val|_],[Val|_]], a).
colonneExiste(Val, [[_|R1],[_|R2],[_|R3]], NomCol) :-
	succAlpha(I,NomCol),
	colonneExiste(Val, [R1,R2,R3], I).


% Predicats diagonaleDG/2 et diagonaleGD/2
%diagonaleGD(-,[[-,-,-],[-,x,x],[-,o,-]]).
%false.

%?- diagonaleGD(-,[[-,-,x],[-,-,x],[-,o,-]]).
%true.
diagonaleGD(Val, [[Val,_,_],[_,Val,_],[_,_,Val]]).

%diagonaleDG(-,[[-,-,x],[-,-,x],[-,o,-]]).
%false.

%?- diagonaleDG(-,[[x,-,-],[-,-,x],[-,o,-]]).
%true
% permet de verifier si les valeur dans les diagonal sont egaux
diagonaleDG(Val, [[_,_,Val],[_,Val,_],[Val,_,_]]).


% Predicat partieGagnee/2
partieGagnee(Val, G) :- ligneExiste(Val, G, _).
partieGagnee(Val, G) :- colonneExiste(Val, G, _).
partieGagnee(Val, G) :- diagonaleGD(Val, G).
partieGagnee(Val, G) :- diagonaleDG(Val, G).




% toutesLesCasesValides(Grille, LC1, C, LC2).
% Se verifie si l'on peut jouer dans la case C de Grille et que la liste
% LC1 est une liste composee de toutes les cases de LC2 et de C.
% Permet de dire si la case C est une case ou l'on peut jouer, en evitant
% de jouer deux fois dans la meme case.
toutesLesCasesValides(Grille, LC1, C, LC2) :-
	coordonneesOuListe(Col, Lig, C),
	leCoupEstValide(Col, Lig, Grille),
	duneListeALautre(LC1, C, LC2).

toutesLesCasesDepart([[a,1],[a,2],[a,3],[b,1],[b,2],[b,3],[c,1],[c,2],[c,3]]).

grilleDeDepart([[-,-,-],[-,-,-],[-,-,-]]).

campCPU(x).





joueLeCoup(Case, Valeur, GrilleDep, GrilleArr) :-
	coordonneesOuListe(Col, Lig, Case),
	leCoupEstValide(Col, Lig, GrilleDep),
	coupJoueDansGrille(Col, Lig, Valeur, GrilleDep, GrilleArr),
	nl, afficheGrille(GrilleArr), nl.




% Predicat : moteur/3
% Usage : moteur(Grille,ListeCoups,Camp) prend en parametre une grille dans
% laquelle tous les coups sont jouables et pour laquelle
% Camp doit jouer.


% cas gagnant pour le joueur
moteur(Grille,_,Camp):-
	partieGagnee(Camp, Grille), nl,
	write('le camp '), write(Camp), write(' a gagne').

% cas gagnant pour le joueur adverse
moteur(Grille,_,Camp):-
	campAdverse(CampGagnant, Camp),
	partieGagnee(CampGagnant, Grille), nl,
	write('le camp '), write(CampGagnant), write(' a gagne').

% cas de match nul, plus de coups jouables possibles
moteur(_,[],_) :-nl, write('game over').

% cas ou l ordinateur doit jouer
moteur(Grille, [Premier|ListeCoupsNew], Camp) :-
	campCPU(Camp),
	joueLeCoup(Premier, Camp, Grille, GrilleArr),
	campAdverse(AutreCamp, Camp),
	moteur(GrilleArr, ListeCoupsNew, AutreCamp).

% cas ou c est l utilisateur qui joue
moteur(Grille, ListeCoups, Camp) :-
	campCPU(CPU),
	campAdverse(Camp, CPU),
	saisieUnCoup(Col,Lig),
	joueLeCoup([Col,Lig], Camp, Grille, GrilleArr),
	toutesLesCasesValides(Grille, ListeCoups, [Col, Lig], ListeCoupsNew),
	moteur(GrilleArr, ListeCoupsNew, CPU).


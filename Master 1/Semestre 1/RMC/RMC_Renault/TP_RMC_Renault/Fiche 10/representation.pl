%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Predicat : succNum/2
% Usage : succNum(X,Y) est satisfait si Y est le successeur de X
succNum(1,2).
succNum(2,3).
succNum(3,4).
succNum(4,5).
succNum(5,6).
succNum(6,7).
succNum(7,8).
	

% Predicat : succAlpha/2
% Usage : succAlpha(X,Y) est satisfait si Y est le successeur de X 
%	 (ordre lexico-graphique)
succAlpha(a,b).
succAlpha(b,c).
succAlpha(c,d).
succAlpha(d,e).
succAlpha(f,g).
succAlpha(g,h).



%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Predicat caseDeGrille/4
% usage : caseDeGrille(NumColonne,NumLigne,Grille,Case) est satisfait si la
%         Case correspond bien a l intersection de la de la colonne NumColonne
%	  et de la ligne NumLigne dans le Grille

caseDeGrille(NumColonne,NumLigne,Grille,Case):-
	ligneDansGrille(NumLigne,Grille,Ligne),
	caseDansLigne(NumColonne,Ligne,Case),!.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Predicat : afficheLigne/1
% Usage : affigheLigne(Ligne)est satisfait si Ligne est une ligne d une grille de
%         morpion et qu elle l affiche proprement a l ecran.
afficheLigne([Tete]):-
	tab(1),write('| '),write(Tete),writeln(' |'),!.
afficheLigne([Tete|Suite]):-
	tab(1),write('| '),write(Tete),
	afficheLigne(Suite).
          






%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% ligneDansGrille : verifier qu une ligne existe dans une grille;
% caseDansLigne : verifier qu une case existe dans une ligne;
% La methode consiste a tronquer la liste jusqu a arriver a la position qui nous interesse;
% on decremente les index a chaque recursion, 
% la position qui nous interesse est en position 1 ou a.


ligneDansGrille(1,[Tete|_],Tete).
ligneDansGrille(NumLig,[_|Reste],Lig):-
	succNum(I,NumLig),
	ligneDansGrille(I,Reste,Lig).


caseDansLigne(a,[Tete|_],Tete).
caseDansLigne(Col,[_|Reste],Case):-
	succAlpha(I,Col),
	caseDansLigne(I,Reste,Case).




%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% coordonneesOuListe(NomCol, NumLigne, Liste).
coordonneesOuListe(NomCol, NumLigne, [NomCol, NumLigne]).






%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Predicat : duneListeALautre/3
% Usage  : duneListeALautre(LC1,C,LC2) est satisfait si la liste LC1 est composee
%          de toutes le cases de la liste LC2 plus la case C

duneListeALautre([Tete|Suite],Tete,Suite):-!.

duneListeALautre([Tete|Suite],C,[Tete|Suite2]):-
	duneListeALautre(Suite,C,Suite2).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Predicat : campAdverse/2
% Usage : campAdverse(Camp,campAdverse) permet de trouver le camp advairse d un camp

campAdverse(x,o).
campAdverse(o,x).


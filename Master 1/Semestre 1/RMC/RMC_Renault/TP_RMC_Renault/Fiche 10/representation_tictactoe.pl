

% Predicat : afficheGrille/1
% test : afficheGrille([[1,2,3],[4,5,6],[7,8,9]]).
% affiche la grille de jeux avec des saut de ligne
afficheGrille([[A1,B1,C1],[A2,B2,C2],[A3,B3,C3]]) :-
	afficheLigne([A1,B1,C1]), nl,
	afficheLigne([A2,B2,C2]), nl,
	afficheLigne([A3,B3,C3]).











saisieUnCoup(NomCol,NumLig) :-
	writeln("entrez le nom de la colonne a jouer (a,b,c) :"),
	read(NomCol), nl,
	writeln("entrez le numero de ligne a jouer (1, 2 ou 3) :"),
	read(NumLig),nl.





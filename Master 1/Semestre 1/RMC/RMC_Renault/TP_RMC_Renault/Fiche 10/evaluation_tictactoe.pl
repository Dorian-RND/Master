% Predicat : coupJoueDansLigne/4
% version bof
coupJoueDansLigneBof(a, Val, [-, X, Y], [Val, X, Y]).
coupJoueDansLigneBof(b, Val, [X, -, Y], [X, Val, Y]).
coupJoueDansLigneBof(c, Val, [X, Y, -], [X, Y, Val]).

% version recursive
coupJoueDansLigne(a, Val, [-|Reste],[Val|Reste]).
coupJoueDansLigne(NomCol, Val, [X|Reste1],[X|Reste2]):-
        succAlpha(I,NomCol),
        coupJoueDansLigne(I, Val, Reste1, Reste2).

% Predicat : coupJoueDansGrille/5
coupJoueDansGrille(NCol,1,Val,[A|Reste],[B|Reste]):- coupJoueDansLigne(NCol, Val, A, B).
coupJoueDansGrille(NCol, NLig, Val, [X|Reste1], [X|Reste2]):- succNum(I, NLig),
                    coupJoueDansGrille(NCol, I, Val, Reste1, Reste2).
/* On met la marque du joueur 'x' en a2
 * Test : ?- coupJoueDansGrille(a,2,x,[[-,-,x],[-,o,-],[-,-,-]],V).
 * V = [[-, -, x], [x, o, -], [-, -, -]] .
 */
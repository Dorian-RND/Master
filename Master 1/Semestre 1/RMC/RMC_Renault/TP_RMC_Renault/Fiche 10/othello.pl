% PLANCHENAULT Simon, PROUDY Julien, MOTTIER Anaïs, DUBIN Baptiste
% Predicat : lanceJeu/0
% Usage :  lanceJeu permet de lancer une partie.
loadModules:-
    consult(evaluation_tictactoe),
    consult(regles_tictactoe),
    consult(representation),
    consult(representation_tictactoe).
  lanceJeu:-
    loadModules,
    menuPrincipale.
  
  
  menuPrincipale:-
    grilleDeDepart(G),
    toutesLesCasesDepart(ListeCoups),
    afficheGrille(G),nl,
    writeln("L ordinateur est les x et vous etes les o."),
    writeln("Quel camp doit debuter la partie ? "),read(Camp),
    toutesLesCasesDepart(ListeCoups),
    moteur(G,ListeCoups,Camp).
    
  
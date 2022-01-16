(deffacts faits-initiaux
    (statut pompe active)
    (statut vanne ouverte)
    (alerte pompe non)
    (statut alarme active))

(defrule regle1
    (declare (salience 5))
    (statut ?element active)
    =>
    (printout t "Je déclenche la règle 1" crlf))

(defrule regle2
    (statut ?element ouverte)
    (alerte ?element non)
    =>
    (printout t "Je déclenche la règle 2" crlf))
    
(defrule regle3
    (declare (salience 10))
    (statut ?element ouverte)
    (alerte ? non)
    =>
    (printout t "Je déclenche la règle 3" crlf))

(defrule regle4
    (statut $?etat)
    =>
    (printout t "Je déclenche la règle 4" crlf))
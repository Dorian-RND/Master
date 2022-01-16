;Etudiant: Dorian RENARD

(deftemplate etat
	(slot cruche_trois (type INTEGER) (default 0) (range 0 3))
	(slot cruche_quatre (type INTEGER) (default 0) (range 0 4))
    (slot pere (type FACT-ADDRESS SYMBOL) (default nil))
    (slot action (type STRING) (default "Situation initiale -> ")))

(deffacts fait-initiaux
	(etat))

;remplir la cruche à 4 litres
(defrule remplir-cruche_quatre
    ?x <- (etat (cruche_quatre ~4))
    =>
    (duplicate ?x (cruche_quatre 4) (pere ?x) (action "Remplir la cruche de 4 litres -> "))
    )

;remplir la cruche à 3 litres
(defrule remplir-cruche_trois
	?x <- (etat (cruche_trois ~3))
    =>
    (duplicate ?x (cruche_trois 3) (pere ?x) (action "Remplir la cruche de 3 litres -> "))
    )

;vider la cruche de 4 litres à 0
(defrule vider-cruche_quatre
	?x <- (etat (cruche_quatre ~0))
    =>
    (duplicate ?x (cruche_quatre 0) (pere ?x) (action "Vider la cruche de 4 litres -> ")))

;vider la cruche de 3 litres à 0
(defrule vider-cruche_trois
	?x <- (etat (cruche_trois ~0))
    =>
    (duplicate ?x (cruche_trois 0) (pere ?x) (action "Vider la cruche de 3 litres -> ")))

;transverser le contenue de la cruche de 3 litres dans la cruche de 4 litres
(defrule transvase-cruche_trois
	?fait <- (etat (cruche_quatre ?q&~4) (cruche_trois ?t&~0))
    =>
    (if (and (<= (+ ?q (- 4 ?q)) 4) (>= (- ?t (- 4 ?q)) 0)) then 
        (assert(etat (cruche_trois (- ?t (- 4 ?q))) (cruche_quatre (+ ?q (- 4 ?q))) (pere ?fait) (action "Transverser 3 -> 4")))
    else 
        (assert(etat (cruche_trois 0) (cruche_quatre (+ ?t ?q)) (pere ?fait) (action "Transverser 3 -> 4")))
    )
)

;transverser le contenue de la cruche de 4 litres dans la cruche de 3 litres
(defrule transvase-cruche_quatre
	?fait <- (etat (cruche_quatre ?q&~0) (cruche_trois ?t&~3))
    =>
    (if (and (<= (+ ?q (- 3 ?q)) 3) (>= (- ?t (- 3 ?q)) 0)) then 
        (assert(etat (cruche_quatre (- ?q (- 3 ?t))) (cruche_trois (+ ?t (- 3 ?t))) (pere ?fait) (action "Transverser 3 -> 4")))
    else 
        (assert(etat (cruche_quatre 0) (cruche_trois (+ ?t ?q)) (pere ?fait) (action "Transverser 3 -> 4")))
    )
)

(deffunction afficher_solution (?fact)
	(if (neq ?fact nil) then
		(bind ?pere (fact-slot-value ?fact pere))
		(bind ?action (fact-slot-value ?fact action))
		(bind ?t (fact-slot-value ?fact cruche_trois))
		(bind ?q (fact-slot-value ?fact cruche_quatre))
		(afficher_solution ?pere)
		(printout t (implode$ (create$ ?t ?q :)) ?action crlf)
	)
)

;quand on trouve une solution
(defrule fini
	(declare (salience -10))
	(or
		?ref <- (etat (cruche_quatre 2))
		?ref <- (etat (cruche_trois 2))
	)
	=>
	(printout t crlf "Nouvelle solution:" crlf)
	(afficher_solution ?ref)
	(halt)
)

;effacer les doublons
(defrule doublon
	(declare (salience 20))
	?p1 <- (etat (cruche_quatre ?q) (cruche_trois ?t))
	?p2 <- (etat (cruche_quatre ?q) (cruche_trois ?t))
	(test (> (fact-index ?p1) (fact-index ?p2)))
	=>
	(retract ?p1)
)

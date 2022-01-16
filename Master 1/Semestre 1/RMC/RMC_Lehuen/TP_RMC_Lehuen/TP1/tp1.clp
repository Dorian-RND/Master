;Etudiant : Dorian RENARD

(deffacts fait-initiaux
    (id 1 Eugenie F )
    (id 2 Albert H )
    (id 3 Martine F )
    (id 4 Christiane F )
    (id 5 Daniel H )
    (id 6 Herve H )
    (id 7 Laurent H )
    (id 8 Nicolas H )
    (id 9 Stephanie F )
    (id 10 Thierry H )
    (id 11 Sylvain H )
    (id 12 Philippe H )
    (id 13 Eric H )

    (parents pere 3 2)
    (parents pere 4 2)
    (parents pere 5 2)
    (parents pere 12 5)
    (parents pere 13 5)

    (parents mere 3 1)
    (parents mere 4 1)
    (parents mere 5 1)
    (parents mere 6 3)
    (parents mere 7 3)
    (parents mere 8 3)
    (parents mere 9 4)
    (parents mere 10 4)
    (parents mere 11 4)

    (texte pere " est le père de ")
    (texte mere " est la mère de ")
    (texte frere " est le frere de ")
    (texte soeur " est la soeur de ")
    (texte papy " est le grand-père de ")
    (texte mamie " est la grand-mère de ")
    (texte oncle " est le tonton de ")
    (texte tante " est la tata de ")
    (texte cousin " est le cousin de ")
    (texte cousine "est la cousine de "))


;; 2 regles d'affichage qu'on abandonne apres modification de la base afin de ne plus
;; avoir qu'une seule regle d'affichage

;;(defrule affiche-mere
;;    (mere ?e ?m) => (format t "%s est la mere de %s %n" ?m ?e)
;;)

;;(defrule affiche-pere
;;    (pere ?e ?p) => (format t "%s est le pere de %s %n" ?p ?e)
;;)

(defrule affiche-parent
    (parents ?relation ?id1 ?id2)
    (texte ?relation ?txt)
    (id ?id1 ?enfant ?)
    (id ?id2 ?parent ?)
    =>
    (format t "%s %s %s %n" ?parent ?txt ?enfant ))

(defrule parent
    (declare (salience 100))
    (or
        (parents pere ?e ?p)
        (parents mere ?e ?p)
    )
    =>
    (assert (parent ?e ?p) ))
(defrule frere-de
    (id ?moi ? H)
    (parent ?moi ?parent)
    (parent ?frere ?parent)
    (test (neq ?moi ?frere))
    =>
    (assert (parents frere ?frere ?moi)))

(defrule soeur-de
    (id ?moi ? F)
    (parent ?moi ?parent)
    (parent ?soeur ?parent)
    (test (neq ?moi ?soeur))
    =>
    (assert (parents soeur ?soeur ?moi)))

(defrule papy-de
    (id ?papy ? H)
    (parent ?enfant ?papy)
    (parent ?petitenfant ?enfant)
    =>
    (assert (parents papy ?petitenfant ?papy)))

(defrule mamie-de
    (id ?mamie ? F)
    (parent ?enfant ?mamie)
    (parent ?petitenfant ?enfant)
    =>
    (assert (parents mamie ?petitenfant ?mamie)))

(defrule tonton-de
    (id ?oncle ? H)
    (or 
        (parents frere ?fr ?oncle)
        (parents soeur ?fr ?oncle)
    )
    (parent ?filleul ?fr)
    =>
    (assert (parents oncle ?filleul ?oncle)))

(defrule tata-de
    (id ?tante ? F)
    (or 
        (parents frere ?fr ?tante)
        (parents soeur ?fr ?tante)
    )
    (parent ?filleul ?fr)
    =>
    (assert (parents tante ?filleul ?tante)))

(defrule cousin-de 
    (id ?cousin ? H)
    (parents papy ?cous ?papy)
    (parents papy ?moi ?papy)
    (test (neq ?cous ?moi))
    =>
    (assert (parents cousin ?cous ?moi)))

(defrule cousine-de 
    (id ?cousine ? F)
    (parents papy ?cous ?papy)
    (parents papy ?moi ?papy)
    (test (neq ?cous ?moi))
    =>
    (assert (parents cousine ?cous ?moi)))
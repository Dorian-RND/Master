;Etudiant: Dorian RENARD

;On définit les faits initiaux
(deffacts fait-initiaux
    (animal Bozo)
    (propriete Bozo poilu)
    (propriete Bozo poche_ventrale)
    (propriete Bozo grands_bonds)

    (animal Bobby)
    (propriete Bobby poilu)
    (propriete Bobby poche_ventrale)
    (propriete Bobby grimpe_arbres)

    (animal Betty)
    (propriete Betty poilu)
    (manger Betty carnivore)
    (propriete Betty vole)

    (animal Baloo)
    (propriete Baloo poilu)
    (manger Baloo carnivore)
    (propriete Baloo pond_oeufs)

    (animal Koko)
    (manger Koko feuilles_eucalyptus)
    (manger Koko pousse_bambou)
    
    (texte phalanger " est un phalanger")
    (texte souris " est une souris")
    (texte chauve-souris " est une chauve-souris")
    (texte ornithorynque " est un ornithorynque")
    (texte koala " est un koala")
    (texte kangourou " est un kangourou")
    (texte faucon " est un faucon")
    (texte opossum " est un opossum")

    (valmanger manger carnivore pousse_bambou feuilles_eucalyptus)
    (valpropriete propriete poilu poche_ventrale grands_bonds pond_oeufs grimpe_arbres vole phalanges_longues petit queue_prehensile longue_queue))

;#######################
;#On définit les règles#
;#######################

;Un animal qui est poilu donne du lait à ses petits
(defrule donne_lait
    (animal ?nom)
    (propriete ?nom poilu)
    =>
    (assert (propriete ?nom donne_lait)))

;Un animal qui porte ses petits dans une poche ventrale et qui a les phalanges longues est un phalanger 
(defrule phalanger
    (animal ?nom)
    (propriete ?nom poche_ventrale)
    (propriete ?nom phalanges_longues)
    =>
    (assert (identification ?nom phalanger)))

;Le marsupial qui grimpe aux arbres est un koala 
(defrule koala
    (animal ?nom)
    (famille ?nom marsupial)
    (propriete ?nom grimpe_arbres)
    =>
    (assert (identification ?nom koala)))

;Un animal qui donne du lait et qui porte ses petits dans une poche ventrale est un marsupial
(defrule marsupial
    (animal ?nom)
    (propriete ?nom donne_lait)
    (propriete ?nom poche_ventrale)
    =>
    (assert (famille ?nom marsupial)))

;Un marsupial qui fait de grands bonds est un kangourou
(defrule kangourou
    (famille ?nom marsupial)
    (propriete ?nom grands_bonds)
    =>
    (assert (identification ?nom kangourou)))

;Tout les animaux qui donnent du lait sont des mammifères
(defrule mammifere
    (animal ?nom)
    (propriete ?nom donne_lait)
    =>
    (assert (famille ?nom mammifere)))

;Un animal qui se nourit exclusivement de feuilles d'eucalyptus est un koala
(defrule koala2
    (animal ?nom)
    (propriete ?nom feuilles_eucalyptus)
	(not (propriete ?name ~feuilles_eucalyptus))
    =>
    (assert (identification ?nom koala)))

;Un animal qui vole, est carnivore et donne du lait est une chauve-souris
(defrule chauve-souris
    (animal ?nom)
    (propriete ?nom vole)
    (manger ?nom carnivore)
    (propriete ?nom donne_lait)
    =>
    (assert (identification ?nom chauve-souris)))

;Un oiseau carnivore est un faucon
(defrule faucon
    (famille ?nom oiseau)
    (manger ?nom carnivore)
    =>
    (assert (identification ?nom faucon)))

;Un marsupial carnivore qui à la queue préhensible est un opossum
(defrule opossum
    (famille ?nom marsupial)
    (manger ?nom carnivore)
    (propriete ?nom queue_prehensile)
    =>
    (assert (identification ?nom opossum)))

;Si un animal est un marsupial alors il provient de l'hémisphère sud
(defrule hemisphere_sud
    (animal ?nom)
    (famille ?nom marsupial)
    =>
    (assert (propriete ?nom hemisphere_sud)))

;Un oiseau est un animal qui vole et pond des oeufs
(defrule oiseau
    (animal ?nom)
    (propriete ?nom vole)
    (propriete ?nom pond_oeufs)
    =>
    (assert (famille ?nom oiseau)))

;Un mammifère qui pond des oeufs est un ornithorynque
(defrule ornithorynque
    (famille ?nom mammifere)
    (propriete ?nom pond_oeufs)
    =>
    (assert (identification ?nom ornithorynque)))

;Un petit mammifère qui possède une longue queue est une souris
(defrule souris
    (famille ?nom mammifere)
    (propriete ?nom petit)
    (propriete ?nom longue_queue)
    =>
    (assert (identification ?nom souris)))

;Règle qui permet d'identifié un animal
(defrule identification
    (declare (salience -10))
    (identification ?nom ?x)
    (texte ?x ?affiche)
    =>
    (printout t ?nom ?affiche crlf))

;Règle qui permet de connaître les animaux non identifié
(defrule non_identifie
    (declare (salience -10))
    (animal ?nom)
    (not (identification ?nom ?))
    =>
    (assert (non_identifie ?nom))
    (format t "Je n'ai pas pu identifier %s %n" ?nom))

;Règle qui permet de poser des questions sur l'alimentation des animaux
(defrule question-hypothese_manger
    (declare (salience -5))
    (non_identifie ?nom)
    (valmanger ?prop $? ?propvalmanger $?)
    =>
    (printout t  ?nom  " mange t-il " ?propvalmanger " question (oui/non) ? ")
    (if (eq (readline) "oui") 
    then (assert (manger ?nom ?propvalmanger))))

;Règle qui permet de poser des questions sur les propriétés des animaux
(defrule question-hypothese_propriete
    (declare (salience -5))
    (non_identifie ?nom)
    (valpropriete ?prop $? ?propval $?)
    =>
    (printout t  ?nom  " a t'il comme propriete " ?propval " question (oui/non) ? ")
    (if (eq (readline) "oui") 
    then (assert (propriete ?nom ?propval))))



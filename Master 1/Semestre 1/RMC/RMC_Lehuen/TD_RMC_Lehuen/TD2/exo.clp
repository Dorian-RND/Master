(deftemplate chemin
    (multislot liste)
    (slot distance (type INTEGER) (default 0)))

(deffacts graphe
    (arc a b 3)
    (arc a e 2)
    (arc b c 5)
    (arc b g 4)
    (arc b g 6)
    (arc c d 1)
    (arc d b 2)
    (arc d f 3)
    (arc e f 4)
    (arc e f 8)
    (arc f g 3))

(deffacts faits-initiaux
    (chemin (liste a)))

(defrule avancer
    (chemin (liste $?liste ?x)
            (distance ?dist))
    (or (arc ?x ?y ?d)
        (arc ?y ?x ?d))
    (test (not (member$ ?y ?liste)))
    =>
    (assert (chemin (liste ?liste ?x ?y)
                    (distance (+ ?dist ?d)))))

(defrule terminer
    (chemin (liste ?deb $?liste ?x&~deb)
            (distance ?dist))
    (or (arc ?x ?deb ?d)
        (arc ?deb ?x ?d))
    =>
    (assert (chemin (liste ?deb ?liste ?x ?deb)
                        (distance (+ ?dist ?d)))))

(defrule supprimer-non-circuit
    (declare (salience -10))
    ?p <- (chemin (liste ?deb $? ?x&~?deb))
    =>
    (retract ?p))

(defrule supprimer-petit
    (declare (salience -20))
    (chemin (liste $?liste1))
    ?p <-  (chemin (liste $?liste2))
    (test (> (length$ ?liste1) (length$ ?liste2)))
    =>
    (retract ?p))

(defrule supprimer-long
    (declare (salience -30))
    (chemin (distance ?d1))
    ?p <- (chemin (distance ?d2))
    (test (< ?d1 ?d2))
    =>
    (retract ?p))

(define (domain inputs)

(:requirements :strips :typing :equality)

(:types
    object
    action
)


(:predicates
)

(:functions
    (time)
    (complextime)

    ;(get_close_to ?o - object)
    ;(pickup ?o - object)
    ;(open ?o - object)
    ;(close ?o - object)
    ;(put ?o - object)
    ;(drop ?o - object)

    (basicaction ?a - action ?o - object)

    (put_into ?o1 - object ?o2 - object)

    (last)
)

(:action basicaction_1
    :parameters (?a - action ?o - object)
    :precondition (and
                    (= (basicaction ?a ?o) (time))
                    (= (complextime) 0)
                    )
    :effect (and 
            (assign (basicaction ?a ?o) 0)
            (increase (time) 1)
            )
)

(:action basicaction_2
    :parameters (?a - action ?o - object)
    :precondition (and
                    (= (basicaction ?a ?o) (complextime))
                    (not (= (complextime) 0))
                    )
    :effect (and 
            (assign (basicaction ?a ?o) 0)
            (decrease (complextime) 1)
            )
)



(:action put_into
    :parameters (?o1 ?o2 - object)
    :precondition (and
                    (= (put_into ?o1 ?o2) (time))
                    (= (complextime) 0)
                    )
    :effect (and
                (assign (basicaction pickup ?o1) 3)
                (assign (basicaction open ?o2) 2)
                (assign (basicaction put ?o2) 1)
                (assign (put_into ?o1 ?o2) 0)
                (increase (time) 1)
                (increase (complextime) 3)
            )
)

;(:action get_close_to_1
;    :parameters (?o - object)
;    :precondition (and
;                    (= (get_close_to ?o) (time))
;                    (= (complextime) 0)
;                    )
;    :effect (and 
;            (assign (get_close_to ?o) 0)
;            (increase (time) 1)
;            )
;)
;
;(:action get_close_to_2
;    :parameters (?o - object)
;    :precondition (and
;                    (= (get_close_to ?o) (complextime))
;                    (not (= (complextime) 0))
;                    )
;    :effect (and 
;            (assign (get_close_to ?o) 0)
;            (decrease (complextime) 1)
;            )
;)
;
;(:action pickup_1
;    :parameters (?o - object)
;    :precondition (and
;                    (= (pickup ?o) (time))
;                    (= (complextime) 0)
;                    )
;    :effect (and 
;            (assign (pickup ?o) 0)
;            (increase (time) 1)
;            )
;)
;
;(:action pickup_2
;    :parameters (?o - object)
;    :precondition (and
;                    (= (pickup ?o) (complextime))
;                    (not (= (complextime) 0))
;                    )
;    :effect (and 
;            (assign (pickup ?o) 0)
;            (decrease (complextime) 1)
;            )
;)
;
;(:action open_1
;    :parameters (?o - object)
;    :precondition (and
;                    (= (open ?o) (time))
;                    (= (complextime) 0)
;                    )
;    :effect (and 
;            (assign (open ?o) 0)
;            (increase (time) 1)
;            )
;)
;
;(:action open_2
;    :parameters (?o - object)
;    :precondition (and
;                    (= (open ?o) (complextime))
;                    (not (= (complextime) 0))
;                    )
;    :effect (and 
;            (assign (open ?o) 0)
;            (decrease (complextime) 1)
;            )
;)
;
;(:action close_1
;    :parameters (?o - object)
;    :precondition (and
;                    (= (close ?o) (time))
;                    (= (complextime) 0)
;                    )
;    :effect (and 
;            (assign (close ?o) 0)
;            (increase (time) 1)
;            )
;)
;
;(:action close_2
;    :parameters (?o - object)
;    :precondition (and
;                    (= (close ?o) (complextime))
;                    (not (= (complextime) 0))
;                    )
;    :effect (and 
;            (assign (close ?o) 0)
;            (decrease (complextime) 1)
;            )
;)
;
;(:action put_1
;    :parameters (?o - object)
;    :precondition (and
;                    (= (put ?o) (time))
;                    (= (complextime) 0)
;                    )
;    :effect (and 
;            (assign (put ?o) 0)
;            (increase (time) 1)
;            )
;)
;
;(:action put_2
;    :parameters (?o - object)
;    :precondition (and
;                    (= (put ?o) (complextime))
;                    (not (= (complextime) 0))
;                    )
;    :effect (and 
;            (assign (put ?o) 0)
;            (decrease (complextime) 1)
;            )
;)
;
;(:action drop_1
;    :parameters (?o - object)
;    :precondition (and
;                    (= (drop ?o) (time))
;                    (= (complextime) 0)
;                    )
;    :effect (and 
;            (assign (drop ?o) 0)
;            (increase (time) 1)
;            )
;)
;
;(:action drop_2
;    :parameters (?o - object)
;    :precondition (and
;                    (= (drop ?o) (complextime))
;                    (not (= (complextime) 0))
;                    )
;    :effect (and 
;            (assign (drop ?o) 0)
;            (decrease (complextime) 1)
;            )
;)
;
;
;(:action put_into
;    :parameters (?o1 ?o2 - object)
;    :precondition (and
;                    (= (put_into ?o1 ?o2) (time))
;                    (= (complextime) 0)
;                    )
;    :effect (and
;                (assign (pickup ?o1) 3)
;                (assign (open ?o2) 2)
;                (assign (put ?o2) 1)
;                (assign (put_into ?o1 ?o2) 0)
;                (increase (time) 1)
;                (increase (complextime) 3)
;            )
;)


)
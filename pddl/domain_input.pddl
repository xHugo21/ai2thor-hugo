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

    (basicaction ?a - action ?o - object)

    (put_into ?o1 - object ?o2 - object)

    (last)
)

; ACCIONES SIMPLES
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

; ACCIONES COMPLEJAS
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

)
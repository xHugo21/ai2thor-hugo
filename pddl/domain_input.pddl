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

    (put_object_into ?o1 - object ?o2 - object)
    (put_into_fridge ?o - object)
    (fry_potato)

    (last)
)

; BASIC ACTIONS
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

; COMPLEX ACTIONS
(:action put_object_into
    :parameters (?o1 ?o2 - object)
    :precondition (and
                    (= (put_object_into ?o1 ?o2) (time))
                    (= (complextime) 0)
                    )
    :effect (and
                (assign (basicaction pickup ?o1) 3)
                (assign (basicaction open ?o2) 2)
                (assign (basicaction put ?o2) 1)
                (assign (put_object_into ?o1 ?o2) 0)
                (increase (time) 1)
                (increase (complextime) 3)
            )
)

(:action put_into_fridge
    :parameters (?o - object)
    :precondition (and
                    (= (put_into_fridge ?o) (time))
                    (= (complextime) 0)
                    )
    :effect (and
                (assign (basicaction pickup ?o) 3)
                (assign (basicaction open fridge) 2)
                (assign (basicaction put fridge) 1)
                (assign (put_into_fridge ?o) 0)
                (increase (time) 1)
                (increase (complextime) 3)
            )
)

(:action fry_potato
    :parameters ()
    :precondition (and
                    (= (fry_potato) (time))
                    (= (complextime) 0)
                    )
    :effect (and
                (assign (basicaction pickup potato) 3)
                (assign (basicaction put pan) 2)
                (assign (basicaction cook potato) 1)
                (assign (fry_potato) 0)
                (increase (time) 1)
                (increase (complextime) 3)
            )
)

)
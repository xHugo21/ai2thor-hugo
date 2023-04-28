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
    (fry_object ?o1 - object)
    (boil_object ?o1 - object)

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
; Put an object inside another
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

; Fry an object
(:action fry_object
    :parameters (?o1 - object)
    :precondition (and
                    (= (fry_object ?o1) (time))
                    (= (complextime) 0)
                    )
    :effect (and
                (assign (basicaction pickup pan) 5)
                (assign (basicaction put stoveburner) 4)
                (assign (basicaction pickup ?o1) 3)
                (assign (basicaction put pan) 2)
                (assign (basicaction cook ?o1) 1)
                (assign (fry_object ?o1) 0)
                (increase (time) 1)
                (increase (complextime) 5)
            )
)

; Boil an object
(:action boil_object
    :parameters (?o1 - object)
    :precondition (and
                    (= (boil_object ?o1) (time))
                    (= (complextime) 0)
                    )
    :effect (and
                (assign (basicaction pickup pot) 5)
                (assign (basicaction put stoveburner) 4)
                (assign (basicaction pickup ?o1) 3)
                (assign (basicaction put pot) 2)
                (assign (basicaction cook ?o1) 1)
                (assign (boil_object ?o1) 0)
                (increase (time) 1)
                (increase (complextime) 5)
            )
)

; Slice an object using a knife
(:action real_slice
    :parameters (?o1 - object)
    :precondition (and
                    (= (real_slice ?o1) (time))
                    (= (complextime) 0)
                    )
    :effect (and
                (assign (basicaction pickup knife) 2)
                (assign (basicaction slice ?o1) 1)
                (assign (real_slice ?o1) 0)
                (increase (time) 1)
                (increase (complextime) 2)
            )
)



)
(define (problem example) (:domain inputs)
(:objects 
    fridge saltshaker tomato - object
    get_close_to pickup open close drop put - action
)

(:init
    ; DEJAR COMO ESTÁ
    (= (time) 1)
    (= (complextime) 0)

    ; MODIFICAR A PARTIR DE AQUÍ
    (= (basicaction pickup saltshaker) 1)
    (= (put_into saltshaker fridge) 2)
    ;(= (basicaction open fridge) 3)

    (= (last) 3)
)

(:goal (and
    ;(forall (?o - object) (= (get_close_to ?o) 0))
    ;(forall (?o - object) (= (pickup ?o) 0))
    ;(forall (?o - object) (= (open ?o) 0))
    ;(forall (?o - object) (= (close ?o) 0))
    ;(forall (?o - object) (= (put ?o) 0))
    ;(forall (?o - object) (= (drop ?o) 0))

    ;(forall (?a - action ?o - object) (= (basicaction ?a ?o) 0))

    ;(forall (?o1 ?o2 - object) (= (put_into ?o1 ?o2) 0))

    (= (time) (last))
    (= (complextime) 0)
))


)

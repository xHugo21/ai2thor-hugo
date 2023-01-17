; Dominio para generar planes sobre entornos de AI2THOR

(define (domain domain_ai2thor)

  ; Eliminar aquellos que sean innecesarios
  (:requirements :strips :fluents :typing :equality)

  (:types
    ;breakable cookable dirtyable fillable moveable openable pickupable receptacle sliceable toggleable usedup - object
    object
    position
  )

  (:predicates
    (holding ?o - object)
    (open ?o - object)
    (closed ?o - object)
    ;(is-standing)
  )

  (:functions
    ;(distance ?o - object)
    (facing)
    (inclination)
    (agent-at-x)
    ;(agent-at-y)
    (agent-at-z)
    (object-at-x ?o - object)
    (object-at-y ?o - object)
    (object-at-z ?o - object)
    (posiblepos-x ?p - position)
    (posiblepos-z ?p - position)
    (interactablepose-x ?p - position ?o - object)
    (interactablepose-z ?p - position ?o - object)
    (interactablepose-facing ?p - position ?o - object)
    (interactablepose-inclination ?p - position ?o - object)
  )

  ; Permite mover al agente hacia delante un tamaño gridSize (default=0.25). facing=0 -> z+0.25
  (:action move-ahead-0
    :parameters (?p - position)
    :precondition (and
      (= (facing) 0)
      (= (agent-at-x) (posiblepos-x ?p))
      (= (+ (agent-at-z) 0.25) (posiblepos-z ?p))
    )
    :effect (and
      (increase (agent-at-z) 0.25)
    )
  )

  ; Permite mover al agente hacia delante un tamaño gridSize (default=0.25). facing=90 -> x+0.25
  (:action move-ahead-90
    :parameters (?p - position)
    :precondition (and
      (= (facing) 90)
      (= (agent-at-z) (posiblepos-z ?p))
      (= (+ (agent-at-x) 0.25) (posiblepos-x ?p))
    )
    :effect (and
      (increase (agent-at-x) 0.25)
    )
  )

  ; Permite mover al agente hacia delante un tamaño gridSize (default=0.25). facing=180 -> z-0.25
  (:action move-ahead-180
    :parameters (?p - position)
    :precondition (and
      (= (facing) 180)
      (= (agent-at-x) (posiblepos-x ?p))
      (= (- (agent-at-z) 0.25) (posiblepos-z ?p))
    )
    :effect (and
      (decrease (agent-at-z) 0.25)
    )
  )

  ; Permite mover al agente hacia delante un tamaño gridSize (default=0.25). facing=270 -> x-0.25
  (:action move-ahead-270
    :parameters (?p - position)
    :precondition (and
      (= (facing) 270)
      (= (agent-at-z) (posiblepos-z ?p))
      (= (- (agent-at-x) 0.25) (posiblepos-x ?p))
    )
    :effect (and
      (decrease (agent-at-x) 0.25)
    )
  )

  ; Permite al agente rotar hacia la izquierda, modificando la funcion facing en -90
  (:action rotate-left
    :parameters ()
    ;:precondition ()
    :effect (and
      (decrease (facing) 90)
    )
  )

  ; Permite al agente rotar hacia la derecha, modificando la funcion facing en +90
  (:action rotate-right
    :parameters ()
    ;:precondition ()
    :effect (and
      (increase (facing) 90)
    )
  )

  ; Permite al agente mirar hacia arriba. Sube la cabeza del agente. facing-30. Max: facing=-30
  (:action lookup
    :parameters ()
    :precondition (and
      (> (inclination) -30)
    )
    :effect (and
      (decrease (inclination) 30)
    )
  )

  ; Permite al agente mirar hacia abajo. Baja la cabeza del agente. facing+30. Max: facing=+60
  (:action lookdown
    :parameters ()
    :precondition (and
      (< (inclination) 60)
    )
    :effect (and
      (increase (inclination) 30)
    )
  )

  ;; Permite al agente levantarse
  ;(:action standup
  ;  :parameters()
  ;  :precondition (and
  ;                (not (is-standing))
  ;  )
  ;  :effect (and
  ;          (is-standing)
  ;  )
  ;)
;
  ;; Permite al agente agacharse
  ;(:action crouch
  ;  :parameters()
  ;  :precondition (and
  ;                (is-standing)
  ;  )
  ;  :effect (and
  ;          (not (is-standing))
  ;  )
  ;)

  ; Permite al agente recoger un objeto
  (:action pickup
    :parameters (?o - object ?p - position)
    :precondition (and
      ;(< (distance ?o) 1.5)
      (= (agent-at-x) (interactablepose-x ?p ?o))
      (= (agent-at-z) (interactablepose-z ?p ?o))
      (= (facing) (interactablepose-facing ?p ?o))
      (= (inclination) (interactablepose-inclination ?p ?o))
    )
    :effect (and
      (holding ?o)
    )
  )

  ; Permite al agente tirar un objeto
  (:action drop
    :parameters (?o - object)
    :precondition (and
      (holding ?o)
    )
    :effect (and
      (not (holding ?o))
    )
  )

  (:action open
    :parameters (?o - object ?p - position)
    :precondition (and
      (= (agent-at-x) (interactablepose-x ?p ?o))
      (= (agent-at-z) (interactablepose-z ?p ?o))
      (= (facing) (interactablepose-facing ?p ?o))
      (= (inclination) (interactablepose-inclination ?p ?o))
    )
    :effect (and
      (open ?o)
    )
  )

  (:action close
    :parameters (?o - object ?p - position)
    :precondition (and
      (= (agent-at-x) (interactablepose-x ?p ?o))
      (= (agent-at-z) (interactablepose-z ?p ?o))
      (= (facing) (interactablepose-facing ?p ?o))
      (= (inclination) (interactablepose-inclination ?p ?o))
    )
    :effect (and
      (closed ?o)
    )
  )

)
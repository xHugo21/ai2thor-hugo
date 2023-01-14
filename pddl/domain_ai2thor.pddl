; Dominio para generar planes sobre entornos de AI2THOR

(define (domain domain_ai2thor)
  
  ; Eliminar aquellos que sean innecesarios
  (:requirements :typing :adl)

  (:types
    breakable cookable dirtyable fillable moveable openable pickupable receptacle sliceable toggleable usedup - object
    position
  )

  (:predicates
    (holding ?o - object)
  )

  (:functions
    (distance ?o - object)
    (facing)
    (inclination)
    (agent-at-x)
    (agent-at-y)
    (agent-at-z)
    (object-at-x ?o - object)
    (object-at-y ?o - object)
    (object-at-z ?o - object)
    (posiblepos-x ?p - position)
    (posiblepos-y ?p - position)
    (posiblepos-z ?p - position)
  )

  ; Permite mover al agente hacia delante un tamaño gridSize (definido en la inicialización del controlador). Solo se puede mover a posiciones válidas
  (:action move-ahead
   :parameters ()
   :condition (and
                
              )
   :effect (and
            
           )
  )

  ; Permite al agente rotar hacia la izquierda
  (:action rotate-left
   :parameters ()
   :condition (and
                
              )
   :effect (and
            
           )
  )

  ; Permite al agente rotar hacia la derecha
  (:action rotate-right
   :parameters ()
   :condition (and
                
              )
   :effect (and
            
           )
  )

  ; Permite al agente recoger un objeto
  (:action pickup
   :parameters ()
   :condition (and
                
              )
   :effect (and
            
           )
  )

  ; Permite al agente tirar un objeto
  (:action drop
   :parameters ()
   :condition (and
                
              )
   :effect (and
            
           )
  )

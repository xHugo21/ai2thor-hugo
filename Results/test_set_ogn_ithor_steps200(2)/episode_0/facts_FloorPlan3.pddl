(define (problem ithor-appleinbox)
(:domain ithor)
(:objects
fridge_0 - fridge
saltshaker_0 - saltshaker
saltshaker_1 - saltshaker
sidetable_0 - sidetable
sidetable_1 - sidetable
)
(:init
(close_to fridge_0)
(close_to fridge_0)
(close_to saltshaker_0)
(close_to saltshaker_0)
(close_to saltshaker_1)
(close_to saltshaker_1)
(close_to sidetable_0)
(close_to sidetable_0)
(close_to sidetable_1)
(close_to sidetable_1)
(discovered fridge_0)
(discovered saltshaker_0)
(discovered saltshaker_1)
(discovered sidetable_0)
(discovered sidetable_1)
(hand_free )
(inspected fridge_0)
(inspected fridge_0)
(inspected saltshaker_0)
(inspected saltshaker_1)
(inspected saltshaker_1)
(inspected sidetable_0)
(inspected sidetable_1)
(inspected sidetable_1)
(open fridge_0)
(openable fridge_0)
(pickupable saltshaker_0)
(pickupable saltshaker_1)
(receptacle fridge_0)
(receptacle sidetable_0)
(receptacle sidetable_1)
(viewing fridge_0)
(viewing saltshaker_1)
(viewing sidetable_1)
)
(:goal
(and
(exists (?o1 - fridge) (and (viewing ?o1) (close_to ?o1))))
))
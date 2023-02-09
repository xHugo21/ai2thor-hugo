(define (problem ithor-appleinbox)
(:domain ithor)
(:objects
drawer_0 - drawer
fridge_0 - fridge
saltshaker_0 - saltshaker
saltshaker_1 - saltshaker
sidetable_0 - sidetable
countertop_0 - countertop
garbagecan_0 - garbagecan
)
(:init
(close_to drawer_0)
(close_to drawer_0)
(close_to fridge_0)
(close_to fridge_0)
(close_to saltshaker_0)
(close_to saltshaker_0)
(discovered countertop_0)
(discovered drawer_0)
(discovered fridge_0)
(discovered garbagecan_0)
(discovered saltshaker_0)
(discovered saltshaker_1)
(discovered sidetable_0)
(hand_free )
(inspected drawer_0)
(inspected fridge_0)
(inspected fridge_0)
(inspected saltshaker_0)
(on saltshaker_1 countertop_0)
(open fridge_0)
(openable drawer_0)
(openable fridge_0)
(pickupable saltshaker_0)
(pickupable saltshaker_1)
(receptacle countertop_0)
(receptacle drawer_0)
(receptacle fridge_0)
(receptacle garbagecan_0)
(receptacle sidetable_0)
(viewing countertop_0)
(viewing fridge_0)
(viewing garbagecan_0)
(viewing saltshaker_1)
)
(:goal 
(and 
(exists (?o1 - saltshaker) (and (viewing ?o1) (close_to ?o1)))) 
))

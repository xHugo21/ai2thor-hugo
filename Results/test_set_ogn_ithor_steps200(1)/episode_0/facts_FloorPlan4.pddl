(define (problem ithor-appleinbox)
(:domain ithor)
(:objects
apple_0 - apple
apple_1 - apple
apple_2 - apple
apple_3 - apple
apple_4 - apple
apple_5 - apple
apple_6 - apple
apple_7 - apple
countertop_0 - countertop
countertop_1 - countertop
countertop_2 - countertop
houseplant_0 - houseplant
pot_0 - pot
pot_1 - pot
potato_0 - potato
sink_0 - sink
stoveburner_0 - stoveburner
stoveburner_1 - stoveburner
stoveburner_2 - stoveburner
window_0 - window
window_1 - window
window_2 - window
fridge_0 - fridge
peppershaker_0 - peppershaker
saltshaker_0 - saltshaker
stoveknob_0 - stoveknob
stoveknob_1 - stoveknob
garbagecan_0 - garbagecan
garbagecan_1 - garbagecan
diningtable_0 - diningtable
diningtable_1 - diningtable
lightswitch_0 - lightswitch
)
(:init
(close_to apple_0)
(close_to apple_0)
(close_to apple_1)
(close_to apple_1)
(close_to apple_2)
(close_to apple_2)
(close_to apple_3)
(close_to apple_3)
(close_to apple_4)
(close_to apple_4)
(close_to apple_5)
(close_to apple_5)
(close_to apple_6)
(close_to apple_6)
(close_to apple_7)
(close_to apple_7)
(close_to fridge_0)
(close_to fridge_0)
(close_to pot_1)
(close_to pot_1)
(discovered apple_0)
(discovered apple_1)
(discovered apple_2)
(discovered apple_3)
(discovered apple_4)
(discovered apple_5)
(discovered apple_6)
(discovered apple_7)
(discovered countertop_0)
(discovered countertop_1)
(discovered countertop_2)
(discovered diningtable_0)
(discovered diningtable_1)
(discovered fridge_0)
(discovered garbagecan_0)
(discovered garbagecan_1)
(discovered houseplant_0)
(discovered lightswitch_0)
(discovered peppershaker_0)
(discovered pot_0)
(discovered pot_1)
(discovered potato_0)
(discovered saltshaker_0)
(discovered sink_0)
(discovered stoveburner_0)
(discovered stoveburner_1)
(discovered stoveburner_2)
(discovered stoveknob_0)
(discovered stoveknob_1)
(discovered window_0)
(discovered window_1)
(discovered window_2)
(hand_free )
(inspected apple_0)
(inspected apple_1)
(inspected apple_2)
(inspected apple_3)
(inspected apple_4)
(inspected apple_5)
(inspected apple_6)
(inspected apple_7)
(inspected apple_7)
(inspected countertop_0)
(inspected countertop_1)
(inspected fridge_0)
(inspected fridge_0)
(inspected houseplant_0)
(inspected pot_0)
(inspected pot_1)
(inspected potato_0)
(inspected stoveburner_0)
(inspected stoveburner_2)
(inspected stoveknob_0)
(inspected stoveknob_1)
(open fridge_0)
(openable fridge_0)
(pickupable apple_0)
(pickupable apple_1)
(pickupable apple_2)
(pickupable apple_3)
(pickupable apple_4)
(pickupable apple_5)
(pickupable apple_6)
(pickupable apple_7)
(pickupable peppershaker_0)
(pickupable pot_0)
(pickupable pot_1)
(pickupable potato_0)
(pickupable saltshaker_0)
(receptacle countertop_0)
(receptacle countertop_1)
(receptacle countertop_2)
(receptacle diningtable_0)
(receptacle diningtable_1)
(receptacle fridge_0)
(receptacle garbagecan_0)
(receptacle garbagecan_1)
(receptacle pot_0)
(receptacle pot_1)
(receptacle stoveburner_0)
(receptacle stoveburner_1)
(receptacle stoveburner_2)
(viewing apple_7)
(viewing fridge_0)
(viewing garbagecan_0)
)
(:goal
(and
(exists (?o1 - fridge) (and (viewing ?o1) (close_to ?o1))))
))
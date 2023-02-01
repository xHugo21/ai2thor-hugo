(define (problem ithor-appleinbox)
(:domain ithor)
(:objects
butterknife_0 - butterknife
countertop_0 - countertop
countertop_1 - countertop
countertop_2 - countertop
countertop_3 - countertop
drawer_0 - drawer
fridge_0 - fridge
fridge_1 - fridge
garbagecan_0 - garbagecan
garbagecan_1 - garbagecan
peppershaker_0 - peppershaker
pot_0 - pot
saltshaker_0 - saltshaker
stoveburner_0 - stoveburner
stoveburner_1 - stoveburner
stoveburner_2 - stoveburner
stoveknob_0 - stoveknob
stoveknob_1 - stoveknob
window_0 - window
window_1 - window
window_2 - window
window_3 - window
window_4 - window
window_5 - window
apple_0 - apple
potato_0 - potato
houseplant_0 - houseplant
ladle_0 - ladle
sink_0 - sink
)
(:init
(close_to apple_0)
(close_to apple_0)
(close_to butterknife_0)
(close_to butterknife_0)
(close_to countertop_0)
(close_to countertop_0)
(close_to countertop_2)
(close_to countertop_2)
(close_to countertop_3)
(close_to countertop_3)
(close_to drawer_0)
(close_to drawer_0)
(close_to houseplant_0)
(close_to houseplant_0)
(close_to ladle_0)
(close_to ladle_0)
(close_to pot_0)
(close_to pot_0)
(close_to potato_0)
(close_to potato_0)
(close_to stoveburner_1)
(close_to stoveburner_1)
(close_to stoveburner_2)
(close_to stoveburner_2)
(close_to stoveknob_0)
(close_to stoveknob_0)
(close_to stoveknob_1)
(close_to stoveknob_1)
(discovered apple_0)
(discovered butterknife_0)
(discovered countertop_0)
(discovered countertop_1)
(discovered countertop_2)
(discovered countertop_3)
(discovered drawer_0)
(discovered fridge_0)
(discovered fridge_1)
(discovered garbagecan_0)
(discovered garbagecan_1)
(discovered houseplant_0)
(discovered ladle_0)
(discovered peppershaker_0)
(discovered pot_0)
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
(discovered window_3)
(discovered window_4)
(discovered window_5)
(hand_free )
(inspected apple_0)
(inspected apple_0)
(inspected butterknife_0)
(inspected countertop_0)
(inspected countertop_2)
(inspected countertop_3)
(inspected countertop_3)
(inspected drawer_0)
(inspected houseplant_0)
(inspected houseplant_0)
(inspected ladle_0)
(inspected pot_0)
(inspected pot_0)
(inspected potato_0)
(inspected potato_0)
(inspected stoveburner_1)
(inspected stoveburner_1)
(inspected stoveburner_2)
(inspected stoveburner_2)
(inspected stoveknob_0)
(on window_0 countertop_0)
(on window_0 countertop_1)
(on window_0 drawer_0)
(on window_0 stoveburner_0)
(on window_0 stoveburner_1)
(open fridge_0)
(open fridge_1)
(openable drawer_0)
(openable fridge_0)
(openable fridge_1)
(pickupable apple_0)
(pickupable butterknife_0)
(pickupable ladle_0)
(pickupable peppershaker_0)
(pickupable pot_0)
(pickupable potato_0)
(pickupable saltshaker_0)
(receptacle countertop_0)
(receptacle countertop_1)
(receptacle countertop_2)
(receptacle countertop_3)
(receptacle drawer_0)
(receptacle fridge_0)
(receptacle fridge_1)
(receptacle garbagecan_0)
(receptacle garbagecan_1)
(receptacle pot_0)
(receptacle stoveburner_0)
(receptacle stoveburner_1)
(receptacle stoveburner_2)
(viewing apple_0)
(viewing countertop_3)
(viewing houseplant_0)
(viewing pot_0)
(viewing potato_0)
(viewing sink_0)
(viewing stoveburner_1)
(viewing stoveburner_2)
(viewing window_4)
(viewing window_5)
)
(:goal
(and
(exists (?o1 - apple) (and (viewing ?o1) (close_to ?o1))))
))
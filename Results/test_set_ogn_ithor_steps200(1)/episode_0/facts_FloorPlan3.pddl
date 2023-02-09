(define (problem ithor-appleinbox)
(:domain ithor)
(:objects
countertop_0 - countertop
countertop_1 - countertop
countertop_2 - countertop
countertop_3 - countertop
countertop_4 - countertop
fork_0 - fork
microwave_0 - microwave
peppershaker_0 - peppershaker
saltshaker_3 - saltshaker
spoon_0 - spoon
stoveburner_0 - stoveburner
stoveburner_1 - stoveburner
stoveburner_2 - stoveburner
stoveburner_3 - stoveburner
stoveburner_4 - stoveburner
kettle_0 - kettle
spatula_0 - spatula
stoveknob_0 - stoveknob
toaster_0 - toaster
apple_0 - apple
cabinet_0 - cabinet
cabinet_1 - cabinet
drawer_0 - drawer
faucet_0 - faucet
lettuce_0 - lettuce
papertowelroll_0 - papertowelroll
pot_0 - pot
potato_0 - potato
sink_0 - sink
window_0 - window
window_1 - window
)
(:init
(close_to countertop_1)
(close_to countertop_1)
(close_to countertop_2)
(close_to countertop_2)
(close_to countertop_3)
(close_to countertop_3)
(close_to countertop_4)
(close_to countertop_4)
(close_to saltshaker_3)
(close_to saltshaker_3)
(close_to stoveburner_0)
(close_to stoveburner_0)
(close_to stoveburner_1)
(close_to stoveburner_1)
(close_to stoveburner_2)
(close_to stoveburner_2)
(close_to stoveburner_3)
(close_to stoveburner_3)
(close_to stoveburner_4)
(close_to stoveburner_4)
(close_to stoveknob_0)
(close_to stoveknob_0)
(discovered apple_0)
(discovered cabinet_0)
(discovered cabinet_1)
(discovered countertop_0)
(discovered countertop_1)
(discovered countertop_2)
(discovered countertop_3)
(discovered countertop_4)
(discovered drawer_0)
(discovered faucet_0)
(discovered fork_0)
(discovered kettle_0)
(discovered lettuce_0)
(discovered microwave_0)
(discovered papertowelroll_0)
(discovered peppershaker_0)
(discovered pot_0)
(discovered potato_0)
(discovered saltshaker_3)
(discovered sink_0)
(discovered spatula_0)
(discovered spoon_0)
(discovered stoveburner_0)
(discovered stoveburner_1)
(discovered stoveburner_2)
(discovered stoveburner_3)
(discovered stoveburner_4)
(discovered stoveknob_0)
(discovered toaster_0)
(discovered window_0)
(discovered window_1)
(hand_free )
(inspected countertop_0)
(inspected countertop_1)
(inspected countertop_2)
(inspected countertop_4)
(inspected countertop_4)
(inspected fork_0)
(inspected microwave_0)
(inspected peppershaker_0)
(inspected saltshaker_3)
(inspected saltshaker_3)
(inspected spoon_0)
(inspected stoveburner_0)
(inspected stoveburner_1)
(inspected stoveburner_2)
(inspected stoveburner_4)
(inspected stoveknob_0)
(open cabinet_0)
(open microwave_0)
(openable cabinet_0)
(openable cabinet_1)
(openable drawer_0)
(openable kettle_0)
(openable microwave_0)
(pickupable apple_0)
(pickupable fork_0)
(pickupable kettle_0)
(pickupable lettuce_0)
(pickupable papertowelroll_0)
(pickupable peppershaker_0)
(pickupable pot_0)
(pickupable potato_0)
(pickupable saltshaker_3)
(pickupable spatula_0)
(pickupable spoon_0)
(receptacle cabinet_0)
(receptacle cabinet_1)
(receptacle countertop_0)
(receptacle countertop_1)
(receptacle countertop_2)
(receptacle countertop_3)
(receptacle countertop_4)
(receptacle drawer_0)
(receptacle microwave_0)
(receptacle pot_0)
(receptacle stoveburner_0)
(receptacle stoveburner_1)
(receptacle stoveburner_2)
(receptacle stoveburner_3)
(receptacle stoveburner_4)
(receptacle toaster_0)
(viewing cabinet_1)
(viewing countertop_4)
(viewing kettle_0)
(viewing lettuce_0)
(viewing papertowelroll_0)
(viewing pot_0)
(viewing saltshaker_3)
(viewing sink_0)
(viewing toaster_0)
(viewing window_1)
)
(:goal
(and
(exists (?o1 - saltshaker) (and (viewing ?o1) (close_to ?o1))))
))
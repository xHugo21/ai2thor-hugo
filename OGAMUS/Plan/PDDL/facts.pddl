(define (problem ithor-appleinbox)
(:domain ithor)
(:objects
apple_0 - apple
cabinet_0 - cabinet
cabinet_1 - cabinet
countertop_0 - countertop
drawer_0 - drawer
drawer_1 - drawer
egg_0 - egg
faucet_0 - faucet
faucet_1 - faucet
faucet_2 - faucet
houseplant_0 - houseplant
kettle_0 - kettle
lettuce_0 - lettuce
papertowelroll_0 - papertowelroll
pot_0 - pot
potato_0 - potato
sink_0 - sink
sink_1 - sink
toaster_0 - toaster
window_0 - window
window_1 - window
window_2 - window
)
(:init
(close_to apple_0)
(close_to apple_0)
(close_to countertop_0)
(close_to countertop_0)
(close_to drawer_0)
(close_to drawer_0)
(close_to egg_0)
(close_to egg_0)
(close_to lettuce_0)
(close_to lettuce_0)
(close_to pot_0)
(close_to pot_0)
(close_to potato_0)
(close_to potato_0)
(close_to sink_1)
(close_to sink_1)
(close_to toaster_0)
(close_to toaster_0)
(discovered apple_0)
(discovered cabinet_0)
(discovered cabinet_1)
(discovered countertop_0)
(discovered drawer_0)
(discovered drawer_1)
(discovered egg_0)
(discovered faucet_0)
(discovered faucet_1)
(discovered faucet_2)
(discovered houseplant_0)
(discovered kettle_0)
(discovered lettuce_0)
(discovered papertowelroll_0)
(discovered pot_0)
(discovered potato_0)
(discovered sink_0)
(discovered sink_1)
(discovered toaster_0)
(discovered window_0)
(discovered window_1)
(discovered window_2)
(hand_free )
(inspected apple_0)
(inspected apple_0)
(inspected countertop_0)
(inspected countertop_0)
(inspected drawer_0)
(inspected egg_0)
(inspected egg_0)
(inspected lettuce_0)
(inspected lettuce_0)
(inspected pot_0)
(inspected pot_0)
(inspected potato_0)
(inspected potato_0)
(inspected sink_1)
(inspected sink_1)
(inspected toaster_0)
(inspected toaster_0)
(openable cabinet_0)
(openable cabinet_1)
(openable drawer_0)
(openable drawer_1)
(openable kettle_0)
(pickupable apple_0)
(pickupable egg_0)
(pickupable kettle_0)
(pickupable lettuce_0)
(pickupable papertowelroll_0)
(pickupable pot_0)
(pickupable potato_0)
(receptacle cabinet_0)
(receptacle cabinet_1)
(receptacle countertop_0)
(receptacle drawer_0)
(receptacle drawer_1)
(receptacle pot_0)
(receptacle toaster_0)
(viewing apple_0)
(viewing cabinet_1)
(viewing countertop_0)
(viewing egg_0)
(viewing faucet_2)
(viewing lettuce_0)
(viewing papertowelroll_0)
(viewing pot_0)
(viewing potato_0)
(viewing sink_1)
(viewing toaster_0)
(viewing window_2)
)
(:goal
(and
(exists (?o1 - apple) (and (viewing ?o1) (holding ?o1) (close_to ?o1))))
))
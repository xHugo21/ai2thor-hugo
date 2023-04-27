(define (problem ithor-appleinbox)
(:domain ithor)
(:objects
cabinet_0 - cabinet
cabinet_1 - cabinet
cabinet_2 - cabinet
cabinet_3 - cabinet
countertop_0 - countertop
countertop_1 - countertop
fridge_0 - fridge
fridge_1 - fridge
toaster_0 - toaster
coffeemachine_0 - coffeemachine
drawer_0 - drawer
drawer_1 - drawer
faucet_0 - faucet
houseplant_0 - houseplant
lettuce_0 - lettuce
mug_0 - mug
sink_0 - sink
sinkbasin_0 - sinkbasin
soapbottle_0 - soapbottle
window_0 - window
)
(:init
(close_to cabinet_0)
(close_to cabinet_0)
(close_to fridge_0)
(close_to fridge_0)
(close_to fridge_1)
(close_to fridge_1)
(close_to toaster_0)
(discovered cabinet_0)
(discovered cabinet_1)
(discovered cabinet_2)
(discovered cabinet_3)
(discovered coffeemachine_0)
(discovered countertop_0)
(discovered countertop_1)
(discovered drawer_0)
(discovered drawer_1)
(discovered faucet_0)
(discovered fridge_0)
(discovered fridge_1)
(discovered houseplant_0)
(discovered lettuce_0)
(discovered mug_0)
(discovered sink_0)
(discovered sinkbasin_0)
(discovered soapbottle_0)
(discovered toaster_0)
(discovered window_0)
(hand_free )
(inspected fridge_0)
(inspected fridge_1)
(inspected toaster_0)
(openable cabinet_0)
(openable cabinet_1)
(openable cabinet_2)
(openable cabinet_3)
(openable drawer_0)
(openable drawer_1)
(openable fridge_0)
(openable fridge_1)
(pickupable lettuce_0)
(pickupable mug_0)
(pickupable soapbottle_0)
(receptacle cabinet_0)
(receptacle cabinet_1)
(receptacle cabinet_2)
(receptacle cabinet_3)
(receptacle coffeemachine_0)
(receptacle countertop_0)
(receptacle countertop_1)
(receptacle drawer_0)
(receptacle drawer_1)
(receptacle fridge_0)
(receptacle fridge_1)
(receptacle mug_0)
(receptacle sinkbasin_0)
(receptacle toaster_0)
(viewing cabinet_1)
(viewing cabinet_2)
(viewing cabinet_3)
(viewing coffeemachine_0)
(viewing countertop_1)
(viewing drawer_0)
(viewing drawer_1)
(viewing faucet_0)
(viewing fridge_1)
(viewing houseplant_0)
(viewing lettuce_0)
(viewing mug_0)
(viewing sink_0)
(viewing sinkbasin_0)
(viewing soapbottle_0)
(viewing toaster_0)
(viewing window_0)
)
(:goal
(and
(exists (?o1 - potato) (and (viewing ?o1) (close_to ?o1))))
))
(define (problem ithor-appleinbox)
(:domain ithor)
(:objects
shelf_0 - shelf
shelf_1 - shelf
shelf_2 - shelf
shelf_3 - shelf
shelf_4 - shelf
shelf_5 - shelf
shelf_6 - shelf
shelf_7 - shelf
shelf_8 - shelf
shelf_9 - shelf
shelf_10 - shelf
shelf_11 - shelf
shelf_12 - shelf
shelf_13 - shelf
shelf_14 - shelf
toiletpaper_0 - toiletpaper
toiletpaper_1 - toiletpaper
toilet_0 - toilet
handtowel_0 - handtowel
handtowelholder_0 - handtowelholder
handtowelholder_1 - handtowelholder
lightswitch_0 - lightswitch
mirror_0 - mirror
mirror_1 - mirror
mirror_2 - mirror
sink_0 - sink
sink_1 - sink
sink_2 - sink
faucet_0 - faucet
faucet_1 - faucet
sinkbasin_0 - sinkbasin
soapbottle_0 - soapbottle
dishsponge_0 - dishsponge
papertowelroll_0 - papertowelroll
sidetable_0 - sidetable
towel_0 - towel
towelholder_0 - towelholder
towelholder_1 - towelholder
window_0 - window
window_1 - window
bathtub_0 - bathtub
bathtub_1 - bathtub
bathtub_2 - bathtub
bathtub_3 - bathtub
garbagecan_0 - garbagecan
garbagecan_1 - garbagecan
garbagecan_2 - garbagecan
garbagecan_3 - garbagecan
showerhead_0 - showerhead
soapbar_0 - soapbar
showercurtain_0 - showercurtain
bathtubbasin_0 - bathtubbasin
plunger_0 - plunger
scrubbrush_0 - scrubbrush
toiletpaperhanger_0 - toiletpaperhanger
)
(:init
(close_to bathtub_0)
(close_to bathtub_0)
(close_to bathtub_3)
(close_to bathtub_3)
(close_to showerhead_0)
(close_to showerhead_0)
(close_to soapbar_0)
(close_to soapbar_0)
(close_to toilet_0)
(close_to toilet_0)
(close_to toiletpaperhanger_0)
(close_to toiletpaperhanger_0)
(discovered bathtub_0)
(discovered bathtub_1)
(discovered bathtub_2)
(discovered bathtub_3)
(discovered bathtubbasin_0)
(discovered dishsponge_0)
(discovered faucet_0)
(discovered faucet_1)
(discovered garbagecan_0)
(discovered garbagecan_1)
(discovered garbagecan_2)
(discovered garbagecan_3)
(discovered handtowel_0)
(discovered handtowelholder_0)
(discovered handtowelholder_1)
(discovered lightswitch_0)
(discovered mirror_0)
(discovered mirror_1)
(discovered mirror_2)
(discovered papertowelroll_0)
(discovered plunger_0)
(discovered scrubbrush_0)
(discovered shelf_0)
(discovered shelf_1)
(discovered shelf_10)
(discovered shelf_11)
(discovered shelf_12)
(discovered shelf_13)
(discovered shelf_14)
(discovered shelf_2)
(discovered shelf_3)
(discovered shelf_4)
(discovered shelf_5)
(discovered shelf_6)
(discovered shelf_7)
(discovered shelf_8)
(discovered shelf_9)
(discovered showercurtain_0)
(discovered showerhead_0)
(discovered sidetable_0)
(discovered sink_0)
(discovered sink_1)
(discovered sink_2)
(discovered sinkbasin_0)
(discovered soapbar_0)
(discovered soapbottle_0)
(discovered toilet_0)
(discovered toiletpaper_0)
(discovered toiletpaper_1)
(discovered toiletpaperhanger_0)
(discovered towel_0)
(discovered towelholder_0)
(discovered towelholder_1)
(discovered window_0)
(discovered window_1)
(hand_free )
(inspected dishsponge_0)
(inspected faucet_0)
(inspected handtowel_0)
(inspected handtowelholder_0)
(inspected handtowelholder_1)
(inspected mirror_0)
(inspected mirror_1)
(inspected mirror_2)
(inspected papertowelroll_0)
(inspected sink_0)
(inspected sink_1)
(inspected sink_2)
(inspected sinkbasin_0)
(inspected soapbottle_0)
(inspected toilet_0)
(inspected toilet_0)
(inspected toiletpaperhanger_0)
(inspected toiletpaperhanger_0)
(on showerhead_0 shelf_10)
(on showerhead_0 shelf_11)
(on showerhead_0 shelf_8)
(on showerhead_0 shelf_9)
(open toilet_0)
(openable showercurtain_0)
(openable toilet_0)
(pickupable dishsponge_0)
(pickupable handtowel_0)
(pickupable papertowelroll_0)
(pickupable plunger_0)
(pickupable scrubbrush_0)
(pickupable soapbar_0)
(pickupable soapbottle_0)
(pickupable toiletpaper_0)
(pickupable toiletpaper_1)
(pickupable towel_0)
(receptacle bathtub_0)
(receptacle bathtub_1)
(receptacle bathtub_2)
(receptacle bathtub_3)
(receptacle bathtubbasin_0)
(receptacle garbagecan_0)
(receptacle garbagecan_1)
(receptacle garbagecan_2)
(receptacle garbagecan_3)
(receptacle handtowelholder_0)
(receptacle handtowelholder_1)
(receptacle shelf_0)
(receptacle shelf_1)
(receptacle shelf_10)
(receptacle shelf_11)
(receptacle shelf_12)
(receptacle shelf_13)
(receptacle shelf_14)
(receptacle shelf_2)
(receptacle shelf_3)
(receptacle shelf_4)
(receptacle shelf_5)
(receptacle shelf_6)
(receptacle shelf_7)
(receptacle shelf_8)
(receptacle shelf_9)
(receptacle sidetable_0)
(receptacle sinkbasin_0)
(receptacle toilet_0)
(receptacle toiletpaperhanger_0)
(receptacle towelholder_0)
(receptacle towelholder_1)
(viewing garbagecan_3)
(viewing plunger_0)
(viewing scrubbrush_0)
(viewing toilet_0)
(viewing toiletpaperhanger_0)
)
(:goal
(and
(exists (?o1 - toilet) (and (viewing ?o1) (close_to ?o1))))
))
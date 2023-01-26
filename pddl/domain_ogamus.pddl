(define (domain ithor)
(:requirements :typing)
(:types
receptacle pickupable openable - object

alarmclock aluminumfoil apple baseballbat book boots basketball bottle bowl box bread - pickupable
butterknife candle cd cellphone peppershaker cloth creditcard cup dishsponge dumbbell egg - pickupable
fork handtowel kettle keychain knife ladle laptop lettuce mug newspaper pan papertowel - pickupable
papertowerlroll pen pencil papershaker pillow plate plunger pot potato remotecontrol - pickupable
saltshaker scrubbrush soapbar soapbottle spatula spoon spraybottle statue tabletopdecor - pickupable
teddybear tennisracket tissuebox toiletpaper tomato towel vase watch wateringcan winebottle - pickupable

armchair bathtub bathtubbasin bed bowl box cabinet coffeemachine coffeetable countertop - receptacle
desk diningtable drawer fridge garbagebag handtowelholder laundryhamper microwave mug - receptacle
ottoman pan plate pot safe shelf sidetable sinkbasin sofa toaster toilet toiletpaperhanger - receptacle
towelholder tvstand stoveburner - receptacle

blinds book box cabinet drawer fridge kettle laptop microwave safe showercurtain showerdoor toilet - openable
)

(:predicates
        (hand_free)
		(holding ?o - object)
		(on ?o1 ?o2 - object)
		(close_to ?o - object)
		(open ?o - object)
		(discovered ?o - object)
		(openable ?o - object)
		(pickupable ?o - object)
		(receptacle ?o - object)
		(viewing ?o - object)
		(inspected ?o - object)
)

(:action pickup
		:parameters (?param_1 - pickupable)
		:precondition (and
		                    (discovered ?param_1)
		                    (close_to ?param_1)
		                    (hand_free)
		                    (pickupable ?param_1)
		                    (viewing ?param_1)
		                    (inspected ?param_1)
		              )
		:effect
		        (and
		            (not (hand_free))
		            (holding ?param_1)
		         )
)

(:action open
		:parameters (?param_1 - openable)
		:precondition (and
		                    (discovered ?param_1)
		                    (close_to ?param_1)
		                    (hand_free)
		                    (openable ?param_1)
		                    (viewing ?param_1)
		                    (inspected ?param_1)
		              )
		:effect
		        (and
		            (open ?param_1)
		         )
)

(:action close
		:parameters (?param_1 - openable)
		:precondition (and
		                    (discovered ?param_1)
		                    (close_to ?param_1)
		                    (hand_free)
		                    (openable ?param_1)
		                    (viewing ?param_1)
		                    (inspected ?param_1)
		              )
		:effect
		        (and
		            (not (open ?param_1))
		         )
)

;(:action putinto
;		:parameters (?param_1 ?param_2 - object)
;		:precondition (and
;		                    (discovered ?param_1)
;		                    (discovered ?param_2)
;		                    (holding ?param_1)
;		                    (close_to ?param_2)
;		                    (open ?param_2)
;		                    (receptacle ?param_2)
;		                    (viewing ?param_2)
;		                    (inspected ?param_1)
;		                    (inspected ?param_2)
;		              )
;		:effect
;		        (and
;		            (hand_free)
;		            (not (holding ?param_1))
;		            (on ?param_1 ?param_2)
;		         )
;)
;
;(:action puton
;		:parameters (?param_1 ?param_2 - object)
;		:precondition (and
;		                    (discovered ?param_1)
;		                    (discovered ?param_2)
;		                    (holding ?param_1)
;		                    (close_to ?param_2)
;		                    (receptacle ?param_2)
;		                    (viewing ?param_2)
;		                    (inspected ?param_2)
;		              )
;		:effect
;		        (and
;		            (hand_free)
;		            (not (holding ?param_1))
;		            (on ?param_1 ?param_2)
;		         )
;)
;
;
;
;(:action get_close_and_look_at_receptacle
;		:parameters (?param_1 - object)
;		:precondition (and
;		                (discovered ?param_1)
;		                (not (hand_free))
;		                (receptacle ?param_1)
;		                (inspected ?param_1)
;		                (or
;		                (not (close_to ?param_1))
;		                (not (viewing ?param_1))
;		                )
;		              )
;		:effect
;		        (and
;		            (close_to ?param_1)
;		            (viewing ?param_1)
;		            (forall (?x - object) (not (viewing ?x)))
;		         )
;)
;
;
;
;
;(:action get_close_and_look_at_pickupable
;		:parameters (?param_1 - object)
;		:precondition (and
;		                (discovered ?param_1)
;		                (pickupable ?param_1)
;		                (inspected ?param_1)
;		                (or
;		                (not (close_to ?param_1))
;		                (not (viewing ?param_1))
;		                )
;		              )
;		:effect
;		        (and
;		            (close_to ?param_1)
;		            (viewing ?param_1)
;		            (forall (?x - object) (not (viewing ?x)))
;		         )
;)
;
;
;(:action inspect
;		:parameters (?param_1 - object)
;		:precondition (and
;		                (discovered ?param_1)
;		                (not (inspected ?param_1))
;		              )
;		:effect
;		        (and
;		            (inspected ?param_1)
;		            (close_to ?param_1)
;		            (viewing ?param_1)
;		            (forall (?x - object) (not (viewing ?x)))
;		         )
;)
;
;
;(:action stop
;		:parameters ()
;		:precondition (and)
;		:effect (and)
;)

)


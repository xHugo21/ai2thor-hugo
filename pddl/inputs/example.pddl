(define (problem example) (:domain inputs)
(:objects 
    ; LEAVE IT AS IT IS

    ; Objects that can be picked up
    ;alarmclock aluminumfoil apple baseballbat book boots basketball bottle bowl box bread - pickupable 
    ;butterknife candle cd cellphone peppershaker cloth creditcard cup dishsponge dumbbell egg - pickupable 
    ;fork handtowel kettle keychain knife ladle laptop lettuce mug newspaper pan papertowel - pickupable 
    ;papertowerlroll pen pencil papershaker pillow plate plunger pot potato remotecontrol - pickupable 
    ;saltshaker scrubbrush soapbar soapbottle spatula spoon spraybottle statue tabletopdecor - pickupable 
    ;teddybear tennisracket tissuebox toiletpaper tomato towel vase watch wateringcan winebottle - pickupable 

    ; Objects that can be a receptacle (contain other objects)
    ;armchair bathtub bathtubbasin bed bowl box cabinet coffeemachine coffeetable countertop - receptacle 
    ;desk diningtable drawer fridge garbagebag handtowelholder laundryhamper microwave mug - receptacle
    ;ottoman pan plate pot safe shelf sidetable sinkbasin sofa toaster toilet toiletpaperhanger - receptacle
    ;towelholder tvstand stoveburner - receptacle

    ; Objects that can be opened / closed
    ;blinds book box cabinet drawer fridge kettle laptop microwave safe showercurtain showerdoor toilet - openable

    ; Objects that can be broken
    ; bottle bowl cellphone cup egg laptop mirror mug plate - breakable
    ; showerdoor statue vase window winebottle - breakable

    ; Objects that can be cooked
    ; breadsliced eggcracked potato potatosliced - cookable

    ; Objets that can be sliced
    ; apple bread egg lettuce potato tomato - sliceable

    ; Basic actions allowed
    get_close_to - action ; (= (basicaction get_close_to <cualquier objeto>) <orden>)
    pickup - action ; (= (basicaction pickup <pickupable>) <orden>)
    open - action ; (= (basicaction open <openable>) <orden>)
    close - action ; (= (basicaction close <openable>) <orden>)
    break - action ; (= (basicaction break <breakable>) <orden>)
    cook - action ; (= (basicaction cook <cookable>) <orden>)
    slice - action ; (= (basicaction slice <sliceable>) <orden>)
    drop - action ; (= (basicaction drop <pickupable>) <orden>)
    put - action ; (= (basicaction put <receptacle>) <orden>)


    ; MODIFY THIS SECTION ADDING OBJECTS USED
    saltshaker fridge potato pan - object

)

(:init
    ; LEAVE IT AS IT IS
    (= (time) 1)
    (= (complextime) 0)

    ; MODIFY FROM HERE MANTAINING AN ORDER BETWEEN ACTIONS
    ;(= (basicaction pickup saltshaker) 1)
    ;(= (put_object_into saltshaker fridge) 2)
    ;(= (put_into_fridge saltshaker) 1)
    (= (fry_potato) 1)
    ;(= (basicaction pickup tomato) 2)

    (= (last) 2) ; MUST BE ESTABLISHED TO THE NUMBER NEXT TO THE LAST ACTION
)

; LEAVE IT AS IT IS
(:goal (and
    (= (time) (last))
    (= (complextime) 0)
))


)

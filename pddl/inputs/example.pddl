(define (problem example) (:domain inputs)
(:objects 
    
    ;alarmclock aluminumfoil apple baseballbat book boots basketball bottle bowl box bread - pickupable ; <- Objects capable of being picked up.
    ;butterknife candle cd cellphone peppershaker cloth creditcard cup dishsponge dumbbell egg - pickupable ; <- Objects capable of being picked up.
    ;fork handtowel kettle keychain knife ladle laptop lettuce mug newspaper pan papertowel - pickupable ; <- Objects capable of being picked up.
    ;papertowerlroll pen pencil papershaker pillow plate plunger pot potato remotecontrol - pickupable ; <- Objects capable of being picked up.
    ;saltshaker scrubbrush soapbar soapbottle spatula spoon spraybottle statue tabletopdecor - pickupable ; <- Objects capable of being picked up.
    ;teddybear tennisracket tissuebox toiletpaper tomato towel vase watch wateringcan winebottle - pickupable ; <- Objects capable of being picked up.

    ;armchair bathtub bathtubbasin bed bowl box cabinet coffeemachine coffeetable countertop - receptacle ; <- Objects capable of having pickupable items inside. Targets of put basic action
    ;desk diningtable drawer fridge garbagebag handtowelholder laundryhamper microwave mug - receptacle ; <- Objects capable of having pickupable items inside. Targets of put basic action
    ;ottoman pan plate pot safe shelf sidetable sinkbasin sofa toaster toilet toiletpaperhanger - receptacle ; <- Objects capable of having pickupable items inside. Targets of put basic action
    ;towelholder tvstand stoveburner - receptacle

    ;blinds book box cabinet drawer fridge kettle laptop microwave safe showercurtain showerdoor toilet - openable ; <- Objects capable of being opened or closed.

    ; All posible basic actions
    get_close_to pickup open close drop put - action

    ; Write here the objects from above that you are going to use
    saltshaker tomato fridge - object

)

(:init
    ; DEJAR COMO ESTÁ
    (= (time) 1)
    (= (complextime) 0)

    ; MODIFICAR A PARTIR DE AQUÍ. AÑADIR LISTA DE ACCIONES BÁSICAS Y COMPLEJAS Y MODIFICAR LAST. *NO REPETIR ACCIONES IDÉNTICAS
    ;(= (basicaction pickup saltshaker) 1)
    (= (put_into saltshaker fridge) 1)
    ;(= (basicaction pickup tomato) 2)

    (= (last) 2) ; Debe establecerse a un número superior a la última acción escrita
)

(:goal (and
    (= (time) (last))
    (= (complextime) 0)
))


)

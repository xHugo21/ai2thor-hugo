(define (problem example) (:domain inputs)
(:objects 
    
    ; MODIFICAR SOLO LOS OBJETOS DEL FINAL EN ESTE APARTADO

    ; Objetos que pueden ser recogidos
    ;alarmclock aluminumfoil apple baseballbat book boots basketball bottle bowl box bread - pickupable 
    ;butterknife candle cd cellphone peppershaker cloth creditcard cup dishsponge dumbbell egg - pickupable 
    ;fork handtowel kettle keychain knife ladle laptop lettuce mug newspaper pan papertowel - pickupable 
    ;papertowerlroll pen pencil papershaker pillow plate plunger pot potato remotecontrol - pickupable 
    ;saltshaker scrubbrush soapbar soapbottle spatula spoon spraybottle statue tabletopdecor - pickupable 
    ;teddybear tennisracket tissuebox toiletpaper tomato towel vase watch wateringcan winebottle - pickupable 

    ; Objetos que pueden incluir otros objetos
    ;armchair bathtub bathtubbasin bed bowl box cabinet coffeemachine coffeetable countertop - receptacle 
    ;desk diningtable drawer fridge garbagebag handtowelholder laundryhamper microwave mug - receptacle
    ;ottoman pan plate pot safe shelf sidetable sinkbasin sofa toaster toilet toiletpaperhanger - receptacle
    ;towelholder tvstand stoveburner - receptacle

    ; Objetos que pueden ser abiertos / cerrados
    ;blinds book box cabinet drawer fridge kettle laptop microwave safe showercurtain showerdoor toilet - openable

    ; Objetos que pueden ser rotos
    ; bottle bowl cellphone cup egg laptop mirror mug plate - breakable
    ; showerdoor statue vase window winebottle - breakable

    ; Objetos que pueden ser cocinados
    ; breadsliced eggcracked potato potatosliced - cookable

    ; Objetos que pueden ser cortados
    ; apple bread egg lettuce potato tomato - sliceable

    ; Todas las acciones básicas permitidas
    get_close_to - action ; (= (basicaction get_close_to <cualquier objeto>) <orden>)
    pickup - action ; (= (basicaction pickup <pickupable>) <orden>)
    open - action ; (= (basicaction open <openable>) <orden>)
    close - action ; (= (basicaction close <openable>) <orden>)
    break - action ; (= (basicaction break <breakable>) <orden>)
    cook - action ; (= (basicaction cook <cookable>) <orden>)
    slice - action ; (= (basicaction slice <sliceable>) <orden>)
    drop - action ; (= (basicaction drop <pickupable>) <orden>)
    put - action ; (= (basicaction put <receptacle>) <orden>)

    ; Escribir aquí todos los objetos que van a ser empleados
    saltshaker fridge - object

)

(:init
    ; DEJAR COMO ESTÁ
    (= (time) 1)
    (= (complextime) 0)

    ; MODIFICAR A PARTIR DE AQUÍ. AÑADIR LISTA DE ACCIONES BÁSICAS Y COMPLEJAS Y MODIFICAR LAST. *NO REPETIR ACCIONES IDÉNTICAS
    (= (basicaction pickup saltshaker) 1)
    ;(= (put_object_into saltshaker fridge) 1)
    ;(= (put_into_fridge saltshaker) 1)
    ;(= (basicaction pickup tomato) 2)

    (= (last) 2) ; Debe establecerse a un número superior a la última acción escrita
)

(:goal (and
    ; NO MODIFICAR
    (= (time) (last))
    (= (complextime) 0)
))


)

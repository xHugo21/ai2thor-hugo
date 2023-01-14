(define (problem problem1)
    (:domain domain_ai2thor)
    (:objects
       Apple|-00.47|+01.15|+00.48 - object
       Book|+00.15|+01.10|+00.62 - object
       Bottle|+01.54|+00.89|-02.54 - object
       Bowl|+00.27|+01.10|-00.75 - object
       Bread|-00.52|+01.17|-00.03 - object
       ButterKnife|-00.41|+01.11|-00.46 - object
       Cabinet|+00.68|+00.50|-02.20 - object
       Cabinet|-01.18|+00.50|-02.20 - object
       Cabinet|-01.55|+00.50|+00.38 - object
       Cabinet|+00.72|+02.02|-02.46 - object
       Cabinet|-01.85|+02.02|+00.38 - object
       Cabinet|+00.68|+02.02|-02.46 - object
       Cabinet|-01.55|+00.50|-01.97 - object
       Cabinet|-01.69|+02.02|-02.46 - object
       Cabinet|-00.73|+02.02|-02.46 - object
       CoffeeMachine|-01.98|+00.90|-00.19 - object
       CounterTop|+00.69|+00.95|-02.48 - object
       CounterTop|-00.08|+01.15|00.00 - object
       CounterTop|-01.87|+00.95|-01.21 - object
       CreditCard|-00.46|+01.10|+00.87 - object
       Cup|+00.37|+01.64|-02.58 - object
       DishSponge|-01.94|+00.75|-01.71 - object
       Drawer|-01.56|+00.66|-00.20 - object
       Drawer|+00.95|+00.83|-02.20 - object
       Drawer|+00.95|+00.56|-02.20 - object
       Drawer|-01.56|+00.84|+00.20 - object
       Drawer|+00.95|+00.22|-02.20 - object
       Drawer|+00.95|+00.71|-02.20 - object
       Drawer|+00.95|+00.39|-02.20 - object
       Drawer|-01.56|+00.33|-00.20 - object
       Drawer|-01.56|+00.84|-00.20 - object
       Egg|-02.04|+00.81|+01.24 - object
       Faucet|-02.15|+00.91|-01.50 - object
       Floor|+00.00|+00.00|+00.00 - object
       Fork|+00.95|+00.77|-02.37 - object
       Fridge|-02.10|+00.00|+01.07 - object
       GarbageCan|-01.94|00.00|+02.03 - object
       HousePlant|-01.95|+00.89|-02.52 - object
       Kettle|+01.04|+00.90|-02.60 - object
       Knife|-01.70|+00.79|-00.22 - object
       Lettuce|-01.81|+00.97|-00.94 - object
       LightSwitch|+02.33|+01.31|-00.16 - object
       Microwave|-00.24|+01.69|-02.53 - object
       Mug|-01.76|+00.90|-00.62 - object
       Pan|+00.72|+00.90|-02.42 - object
       PaperTowelRoll|-02.06|+01.01|-00.81 - object
       PepperShaker|+00.30|+00.90|-02.47 - object
       Plate|+00.96|+01.65|-02.61 - object
       Pot|-01.22|+00.90|-02.36 - object
       Potato|-01.66|+00.93|-02.15 - object
       SaltShaker|+00.35|+00.90|-02.57 - object
       Shelf|+01.75|+00.17|-02.56 - object
       Shelf|+01.75|+00.88|-02.56 - object
       Shelf|+01.75|+00.55|-02.56 - object
       ShelvingUnit|+01.75|+00.01|-02.56 - object
       Sink|-01.90|+00.97|-01.50 - object
       Sink|-01.90|+00.97|-01.50|SinkBasin - object
       SoapBottle|-01.99|+00.90|-02.03 - object
       Spatula|+00.38|+00.91|-02.33 - object
       Spoon|+00.98|+00.77|-02.29 - object
       Statue|+01.96|+00.18|-02.54 - object
       Stool|+00.70|+00.00|-00.51 - object
       Stool|+00.74|+00.00|+00.56 - object
       StoveBurner|-00.47|+00.92|-02.37 - object
       StoveBurner|-00.04|+00.92|-02.58 - object
       StoveBurner|-00.47|+00.92|-02.58 - object
       StoveBurner|-00.04|+00.92|-02.37 - object
       StoveKnob|-00.48|+00.88|-02.19 - object
       StoveKnob|-00.02|+00.88|-02.19 - object
       StoveKnob|-00.33|+00.88|-02.19 - object
       StoveKnob|-00.18|+00.88|-02.19 - object
       Toaster|-01.84|+00.90|+00.13 - object
       Tomato|-00.39|+01.14|-00.81 - object
       Vase|+01.56|+00.56|-02.50 - object
       Vase|+01.99|+00.56|-02.49 - object
       Window|-02.23|+01.43|-01.20 - object
       WineBottle|-01.00|+01.65|-02.58 - object
    )

    (:init
       (= (distance Apple|-00.47|+01.15|+00.48) 0.7896325588226318)

       (= (facing) 270.0)

       (= (inclination) 0.0)

       (= (agent-at-x) -1.0)
       (= (agent-at-y) 0.900999128818512)
       (= (agent-at-z) 1.0)

       (= (posiblepos-x pos0) -1.0)
       (= (posiblepos-y pos0) 0.900999128818512)
       (= (posiblepos-z pos0) 1.0)

       (= (posiblepos-x pos1) -1.0)
       (= (posiblepos-y pos1) 0.900999128818512)
       (= (posiblepos-z pos1) 1.25)

       (= (posiblepos-x pos2) -1.25)
       (= (posiblepos-y pos2) 0.900999128818512)
       (= (posiblepos-z pos2) 1.0)

       (= (posiblepos-x pos3) -1.0)
       (= (posiblepos-y pos3) 0.900999128818512)
       (= (posiblepos-z pos3) 0.75)

       (= (posiblepos-x pos4) -0.75)
       (= (posiblepos-y pos4) 0.900999128818512)
       (= (posiblepos-z pos4) 1.25)

       (= (posiblepos-x pos5) -1.0)
       (= (posiblepos-y pos5) 0.900999128818512)
       (= (posiblepos-z pos5) 1.5)

       (= (posiblepos-x pos6) -1.25)
       (= (posiblepos-y pos6) 0.900999128818512)
       (= (posiblepos-z pos6) 1.25)

       (= (posiblepos-x pos7) -1.25)
       (= (posiblepos-y pos7) 0.900999128818512)
       (= (posiblepos-z pos7) 0.75)

       (= (posiblepos-x pos8) -1.0)
       (= (posiblepos-y pos8) 0.900999128818512)
       (= (posiblepos-z pos8) 0.5)

       (= (posiblepos-x pos9) -0.5)
       (= (posiblepos-y pos9) 0.900999128818512)
       (= (posiblepos-z pos9) 1.25)

       (= (posiblepos-x pos10) -0.75)
       (= (posiblepos-y pos10) 0.900999128818512)
       (= (posiblepos-z pos10) 1.5)

       (= (posiblepos-x pos11) -1.0)
       (= (posiblepos-y pos11) 0.900999128818512)
       (= (posiblepos-z pos11) 1.75)

       (= (posiblepos-x pos12) -1.25)
       (= (posiblepos-y pos12) 0.900999128818512)
       (= (posiblepos-z pos12) 1.5)

       (= (posiblepos-x pos13) -1.25)
       (= (posiblepos-y pos13) 0.900999128818512)
       (= (posiblepos-z pos13) 0.5)

       (= (posiblepos-x pos14) -1.0)
       (= (posiblepos-y pos14) 0.900999128818512)
       (= (posiblepos-z pos14) 0.25)

       (= (posiblepos-x pos15) -0.25)
       (= (posiblepos-y pos15) 0.900999128818512)
       (= (posiblepos-z pos15) 1.25)

       (= (posiblepos-x pos16) -0.5)
       (= (posiblepos-y pos16) 0.900999128818512)
       (= (posiblepos-z pos16) 1.5)

       (= (posiblepos-x pos17) -0.75)
       (= (posiblepos-y pos17) 0.900999128818512)
       (= (posiblepos-z pos17) 1.75)

       (= (posiblepos-x pos18) -1.0)
       (= (posiblepos-y pos18) 0.900999128818512)
       (= (posiblepos-z pos18) 2.0)

       (= (posiblepos-x pos19) -1.25)
       (= (posiblepos-y pos19) 0.900999128818512)
       (= (posiblepos-z pos19) 1.75)

       (= (posiblepos-x pos20) -1.25)
       (= (posiblepos-y pos20) 0.900999128818512)
       (= (posiblepos-z pos20) 0.25)

       (= (posiblepos-x pos21) -1.0)
       (= (posiblepos-y pos21) 0.900999128818512)
       (= (posiblepos-z pos21) 0.0)

       (= (posiblepos-x pos22) 0.0)
       (= (posiblepos-y pos22) 0.900999128818512)
       (= (posiblepos-z pos22) 1.25)

       (= (posiblepos-x pos23) -0.25)
       (= (posiblepos-y pos23) 0.900999128818512)
       (= (posiblepos-z pos23) 1.5)

       (= (posiblepos-x pos24) -0.5)
       (= (posiblepos-y pos24) 0.900999128818512)
       (= (posiblepos-z pos24) 1.75)

       (= (posiblepos-x pos25) -0.75)
       (= (posiblepos-y pos25) 0.900999128818512)
       (= (posiblepos-z pos25) 2.0)

       (= (posiblepos-x pos26) -1.25)
       (= (posiblepos-y pos26) 0.900999128818512)
       (= (posiblepos-z pos26) 2.0)

       (= (posiblepos-x pos27) -1.5)
       (= (posiblepos-y pos27) 0.900999128818512)
       (= (posiblepos-z pos27) 1.75)

       (= (posiblepos-x pos28) -1.25)
       (= (posiblepos-y pos28) 0.900999128818512)
       (= (posiblepos-z pos28) 0.0)

       (= (posiblepos-x pos29) -1.0)
       (= (posiblepos-y pos29) 0.900999128818512)
       (= (posiblepos-z pos29) -0.25)

       (= (posiblepos-x pos30) 0.25)
       (= (posiblepos-y pos30) 0.900999128818512)
       (= (posiblepos-z pos30) 1.25)

       (= (posiblepos-x pos31) 0.0)
       (= (posiblepos-y pos31) 0.900999128818512)
       (= (posiblepos-z pos31) 1.5)

       (= (posiblepos-x pos32) -0.25)
       (= (posiblepos-y pos32) 0.900999128818512)
       (= (posiblepos-z pos32) 1.75)

       (= (posiblepos-x pos33) -0.5)
       (= (posiblepos-y pos33) 0.900999128818512)
       (= (posiblepos-z pos33) 2.0)

       (= (posiblepos-x pos34) -1.5)
       (= (posiblepos-y pos34) 0.900999128818512)
       (= (posiblepos-z pos34) 2.0)

       (= (posiblepos-x pos35) -1.25)
       (= (posiblepos-y pos35) 0.900999128818512)
       (= (posiblepos-z pos35) -0.25)

       (= (posiblepos-x pos36) -1.0)
       (= (posiblepos-y pos36) 0.900999128818512)
       (= (posiblepos-z pos36) -0.5)

       (= (posiblepos-x pos37) 0.5)
       (= (posiblepos-y pos37) 0.900999128818512)
       (= (posiblepos-z pos37) 1.25)

       (= (posiblepos-x pos38) 0.25)
       (= (posiblepos-y pos38) 0.900999128818512)
       (= (posiblepos-z pos38) 1.5)

       (= (posiblepos-x pos39) 0.0)
       (= (posiblepos-y pos39) 0.900999128818512)
       (= (posiblepos-z pos39) 1.75)

       (= (posiblepos-x pos40) -0.25)
       (= (posiblepos-y pos40) 0.900999128818512)
       (= (posiblepos-z pos40) 2.0)

       (= (posiblepos-x pos41) -1.25)
       (= (posiblepos-y pos41) 0.900999128818512)
       (= (posiblepos-z pos41) -0.5)

       (= (posiblepos-x pos42) -1.0)
       (= (posiblepos-y pos42) 0.900999128818512)
       (= (posiblepos-z pos42) -0.75)

       (= (posiblepos-x pos43) 0.75)
       (= (posiblepos-y pos43) 0.900999128818512)
       (= (posiblepos-z pos43) 1.25)

       (= (posiblepos-x pos44) 0.5)
       (= (posiblepos-y pos44) 0.900999128818512)
       (= (posiblepos-z pos44) 1.5)

       (= (posiblepos-x pos45) 0.25)
       (= (posiblepos-y pos45) 0.900999128818512)
       (= (posiblepos-z pos45) 1.75)

       (= (posiblepos-x pos46) 0.0)
       (= (posiblepos-y pos46) 0.900999128818512)
       (= (posiblepos-z pos46) 2.0)

       (= (posiblepos-x pos47) -0.25)
       (= (posiblepos-y pos47) 0.900999128818512)
       (= (posiblepos-z pos47) 2.25)

       (= (posiblepos-x pos48) -1.25)
       (= (posiblepos-y pos48) 0.900999128818512)
       (= (posiblepos-z pos48) -0.75)

       (= (posiblepos-x pos49) -1.0)
       (= (posiblepos-y pos49) 0.900999128818512)
       (= (posiblepos-z pos49) -1.0)

       (= (posiblepos-x pos50) 1.0)
       (= (posiblepos-y pos50) 0.900999128818512)
       (= (posiblepos-z pos50) 1.25)

       (= (posiblepos-x pos51) 0.75)
       (= (posiblepos-y pos51) 0.900999128818512)
       (= (posiblepos-z pos51) 1.5)

       (= (posiblepos-x pos52) 0.5)
       (= (posiblepos-y pos52) 0.900999128818512)
       (= (posiblepos-z pos52) 1.75)

       (= (posiblepos-x pos53) 0.25)
       (= (posiblepos-y pos53) 0.900999128818512)
       (= (posiblepos-z pos53) 2.0)

       (= (posiblepos-x pos54) 0.0)
       (= (posiblepos-y pos54) 0.900999128818512)
       (= (posiblepos-z pos54) 2.25)

       (= (posiblepos-x pos55) -1.25)
       (= (posiblepos-y pos55) 0.900999128818512)
       (= (posiblepos-z pos55) -1.0)

       (= (posiblepos-x pos56) -1.0)
       (= (posiblepos-y pos56) 0.900999128818512)
       (= (posiblepos-z pos56) -1.25)

       (= (posiblepos-x pos57) 1.25)
       (= (posiblepos-y pos57) 0.900999128818512)
       (= (posiblepos-z pos57) 1.25)

       (= (posiblepos-x pos58) 1.0)
       (= (posiblepos-y pos58) 0.900999128818512)
       (= (posiblepos-z pos58) 1.5)

       (= (posiblepos-x pos59) 0.75)
       (= (posiblepos-y pos59) 0.900999128818512)
       (= (posiblepos-z pos59) 1.75)

       (= (posiblepos-x pos60) 0.5)
       (= (posiblepos-y pos60) 0.900999128818512)
       (= (posiblepos-z pos60) 2.0)

       (= (posiblepos-x pos61) -1.25)
       (= (posiblepos-y pos61) 0.900999128818512)
       (= (posiblepos-z pos61) -1.25)

       (= (posiblepos-x pos62) -0.75)
       (= (posiblepos-y pos62) 0.900999128818512)
       (= (posiblepos-z pos62) -1.25)

       (= (posiblepos-x pos63) -1.0)
       (= (posiblepos-y pos63) 0.900999128818512)
       (= (posiblepos-z pos63) -1.5)

       (= (posiblepos-x pos64) 1.5)
       (= (posiblepos-y pos64) 0.900999128818512)
       (= (posiblepos-z pos64) 1.25)

       (= (posiblepos-x pos65) 1.25)
       (= (posiblepos-y pos65) 0.900999128818512)
       (= (posiblepos-z pos65) 1.5)

       (= (posiblepos-x pos66) 1.25)
       (= (posiblepos-y pos66) 0.900999128818512)
       (= (posiblepos-z pos66) 1.0)

       (= (posiblepos-x pos67) 1.0)
       (= (posiblepos-y pos67) 0.900999128818512)
       (= (posiblepos-z pos67) 1.75)

       (= (posiblepos-x pos68) 0.75)
       (= (posiblepos-y pos68) 0.900999128818512)
       (= (posiblepos-z pos68) 2.0)

       (= (posiblepos-x pos69) -1.25)
       (= (posiblepos-y pos69) 0.900999128818512)
       (= (posiblepos-z pos69) -1.5)

       (= (posiblepos-x pos70) -0.5)
       (= (posiblepos-y pos70) 0.900999128818512)
       (= (posiblepos-z pos70) -1.25)

       (= (posiblepos-x pos71) -0.75)
       (= (posiblepos-y pos71) 0.900999128818512)
       (= (posiblepos-z pos71) -1.5)

       (= (posiblepos-x pos72) -1.0)
       (= (posiblepos-y pos72) 0.900999128818512)
       (= (posiblepos-z pos72) -1.75)

       (= (posiblepos-x pos73) 1.75)
       (= (posiblepos-y pos73) 0.900999128818512)
       (= (posiblepos-z pos73) 1.25)

       (= (posiblepos-x pos74) 1.5)
       (= (posiblepos-y pos74) 0.900999128818512)
       (= (posiblepos-z pos74) 1.5)

       (= (posiblepos-x pos75) 1.5)
       (= (posiblepos-y pos75) 0.900999128818512)
       (= (posiblepos-z pos75) 1.0)

       (= (posiblepos-x pos76) 1.25)
       (= (posiblepos-y pos76) 0.900999128818512)
       (= (posiblepos-z pos76) 1.75)

       (= (posiblepos-x pos77) 1.25)
       (= (posiblepos-y pos77) 0.900999128818512)
       (= (posiblepos-z pos77) 0.75)

       (= (posiblepos-x pos78) 1.0)
       (= (posiblepos-y pos78) 0.900999128818512)
       (= (posiblepos-z pos78) 2.0)

       (= (posiblepos-x pos79) -1.25)
       (= (posiblepos-y pos79) 0.900999128818512)
       (= (posiblepos-z pos79) -1.75)

       (= (posiblepos-x pos80) -0.25)
       (= (posiblepos-y pos80) 0.900999128818512)
       (= (posiblepos-z pos80) -1.25)

       (= (posiblepos-x pos81) -0.5)
       (= (posiblepos-y pos81) 0.900999128818512)
       (= (posiblepos-z pos81) -1.5)

       (= (posiblepos-x pos82) -0.75)
       (= (posiblepos-y pos82) 0.900999128818512)
       (= (posiblepos-z pos82) -1.75)

       (= (posiblepos-x pos83) 2.0)
       (= (posiblepos-y pos83) 0.900999128818512)
       (= (posiblepos-z pos83) 1.25)

       (= (posiblepos-x pos84) 1.75)
       (= (posiblepos-y pos84) 0.900999128818512)
       (= (posiblepos-z pos84) 1.5)

       (= (posiblepos-x pos85) 1.75)
       (= (posiblepos-y pos85) 0.900999128818512)
       (= (posiblepos-z pos85) 1.0)

       (= (posiblepos-x pos86) 1.5)
       (= (posiblepos-y pos86) 0.900999128818512)
       (= (posiblepos-z pos86) 1.75)

       (= (posiblepos-x pos87) 1.5)
       (= (posiblepos-y pos87) 0.900999128818512)
       (= (posiblepos-z pos87) 0.75)

       (= (posiblepos-x pos88) 1.25)
       (= (posiblepos-y pos88) 0.900999128818512)
       (= (posiblepos-z pos88) 2.0)

       (= (posiblepos-x pos89) 1.25)
       (= (posiblepos-y pos89) 0.900999128818512)
       (= (posiblepos-z pos89) 0.5)

       (= (posiblepos-x pos90) 0.0)
       (= (posiblepos-y pos90) 0.900999128818512)
       (= (posiblepos-z pos90) -1.25)

       (= (posiblepos-x pos91) -0.25)
       (= (posiblepos-y pos91) 0.900999128818512)
       (= (posiblepos-z pos91) -1.5)

       (= (posiblepos-x pos92) -0.5)
       (= (posiblepos-y pos92) 0.900999128818512)
       (= (posiblepos-z pos92) -1.75)

       (= (posiblepos-x pos93) 2.0)
       (= (posiblepos-y pos93) 0.900999128818512)
       (= (posiblepos-z pos93) 1.5)

       (= (posiblepos-x pos94) 2.0)
       (= (posiblepos-y pos94) 0.900999128818512)
       (= (posiblepos-z pos94) 1.0)

       (= (posiblepos-x pos95) 1.75)
       (= (posiblepos-y pos95) 0.900999128818512)
       (= (posiblepos-z pos95) 1.75)

       (= (posiblepos-x pos96) 1.75)
       (= (posiblepos-y pos96) 0.900999128818512)
       (= (posiblepos-z pos96) 0.75)

       (= (posiblepos-x pos97) 1.5)
       (= (posiblepos-y pos97) 0.900999128818512)
       (= (posiblepos-z pos97) 2.0)

       (= (posiblepos-x pos98) 1.5)
       (= (posiblepos-y pos98) 0.900999128818512)
       (= (posiblepos-z pos98) 0.5)

       (= (posiblepos-x pos99) 1.25)
       (= (posiblepos-y pos99) 0.900999128818512)
       (= (posiblepos-z pos99) 0.25)

       (= (posiblepos-x pos100) 0.25)
       (= (posiblepos-y pos100) 0.900999128818512)
       (= (posiblepos-z pos100) -1.25)

       (= (posiblepos-x pos101) 0.0)
       (= (posiblepos-y pos101) 0.900999128818512)
       (= (posiblepos-z pos101) -1.5)

       (= (posiblepos-x pos102) -0.25)
       (= (posiblepos-y pos102) 0.900999128818512)
       (= (posiblepos-z pos102) -1.75)

       (= (posiblepos-x pos103) 2.0)
       (= (posiblepos-y pos103) 0.900999128818512)
       (= (posiblepos-z pos103) 1.75)

       (= (posiblepos-x pos104) 2.0)
       (= (posiblepos-y pos104) 0.900999128818512)
       (= (posiblepos-z pos104) 0.75)

       (= (posiblepos-x pos105) 1.75)
       (= (posiblepos-y pos105) 0.900999128818512)
       (= (posiblepos-z pos105) 2.0)

       (= (posiblepos-x pos106) 1.75)
       (= (posiblepos-y pos106) 0.900999128818512)
       (= (posiblepos-z pos106) 0.5)

       (= (posiblepos-x pos107) 1.5)
       (= (posiblepos-y pos107) 0.900999128818512)
       (= (posiblepos-z pos107) 0.25)

       (= (posiblepos-x pos108) 1.25)
       (= (posiblepos-y pos108) 0.900999128818512)
       (= (posiblepos-z pos108) 0.0)

       (= (posiblepos-x pos109) 0.5)
       (= (posiblepos-y pos109) 0.900999128818512)
       (= (posiblepos-z pos109) -1.25)

       (= (posiblepos-x pos110) 0.25)
       (= (posiblepos-y pos110) 0.900999128818512)
       (= (posiblepos-z pos110) -1.5)

       (= (posiblepos-x pos111) 0.0)
       (= (posiblepos-y pos111) 0.900999128818512)
       (= (posiblepos-z pos111) -1.75)

       (= (posiblepos-x pos112) 2.0)
       (= (posiblepos-y pos112) 0.900999128818512)
       (= (posiblepos-z pos112) 2.0)

       (= (posiblepos-x pos113) 2.0)
       (= (posiblepos-y pos113) 0.900999128818512)
       (= (posiblepos-z pos113) 0.5)

       (= (posiblepos-x pos114) 1.75)
       (= (posiblepos-y pos114) 0.900999128818512)
       (= (posiblepos-z pos114) 0.25)

       (= (posiblepos-x pos115) 1.5)
       (= (posiblepos-y pos115) 0.900999128818512)
       (= (posiblepos-z pos115) 0.0)

       (= (posiblepos-x pos116) 1.0)
       (= (posiblepos-y pos116) 0.900999128818512)
       (= (posiblepos-z pos116) 0.0)

       (= (posiblepos-x pos117) 1.25)
       (= (posiblepos-y pos117) 0.900999128818512)
       (= (posiblepos-z pos117) -0.25)

       (= (posiblepos-x pos118) 0.75)
       (= (posiblepos-y pos118) 0.900999128818512)
       (= (posiblepos-z pos118) -1.25)

       (= (posiblepos-x pos119) 0.5)
       (= (posiblepos-y pos119) 0.900999128818512)
       (= (posiblepos-z pos119) -1.5)

       (= (posiblepos-x pos120) 0.25)
       (= (posiblepos-y pos120) 0.900999128818512)
       (= (posiblepos-z pos120) -1.75)

       (= (posiblepos-x pos121) 2.0)
       (= (posiblepos-y pos121) 0.900999128818512)
       (= (posiblepos-z pos121) 0.25)

       (= (posiblepos-x pos122) 1.75)
       (= (posiblepos-y pos122) 0.900999128818512)
       (= (posiblepos-z pos122) 0.0)

       (= (posiblepos-x pos123) 1.5)
       (= (posiblepos-y pos123) 0.900999128818512)
       (= (posiblepos-z pos123) -0.25)

       (= (posiblepos-x pos124) 1.0)
       (= (posiblepos-y pos124) 0.900999128818512)
       (= (posiblepos-z pos124) -0.25)

       (= (posiblepos-x pos125) 1.25)
       (= (posiblepos-y pos125) 0.900999128818512)
       (= (posiblepos-z pos125) -0.5)

       (= (posiblepos-x pos126) 1.0)
       (= (posiblepos-y pos126) 0.900999128818512)
       (= (posiblepos-z pos126) -1.25)

       (= (posiblepos-x pos127) 0.75)
       (= (posiblepos-y pos127) 0.900999128818512)
       (= (posiblepos-z pos127) -1.5)

       (= (posiblepos-x pos128) 0.5)
       (= (posiblepos-y pos128) 0.900999128818512)
       (= (posiblepos-z pos128) -1.75)

       (= (posiblepos-x pos129) 2.0)
       (= (posiblepos-y pos129) 0.900999128818512)
       (= (posiblepos-z pos129) 0.0)

       (= (posiblepos-x pos130) 1.75)
       (= (posiblepos-y pos130) 0.900999128818512)
       (= (posiblepos-z pos130) -0.25)

       (= (posiblepos-x pos131) 1.5)
       (= (posiblepos-y pos131) 0.900999128818512)
       (= (posiblepos-z pos131) -0.5)

       (= (posiblepos-x pos132) 1.25)
       (= (posiblepos-y pos132) 0.900999128818512)
       (= (posiblepos-z pos132) -0.75)

       (= (posiblepos-x pos133) 1.25)
       (= (posiblepos-y pos133) 0.900999128818512)
       (= (posiblepos-z pos133) -1.25)

       (= (posiblepos-x pos134) 1.0)
       (= (posiblepos-y pos134) 0.900999128818512)
       (= (posiblepos-z pos134) -1.0)

       (= (posiblepos-x pos135) 1.0)
       (= (posiblepos-y pos135) 0.900999128818512)
       (= (posiblepos-z pos135) -1.5)

       (= (posiblepos-x pos136) 0.75)
       (= (posiblepos-y pos136) 0.900999128818512)
       (= (posiblepos-z pos136) -1.75)

       (= (posiblepos-x pos137) 2.0)
       (= (posiblepos-y pos137) 0.900999128818512)
       (= (posiblepos-z pos137) -0.25)

       (= (posiblepos-x pos138) 1.75)
       (= (posiblepos-y pos138) 0.900999128818512)
       (= (posiblepos-z pos138) -0.5)

       (= (posiblepos-x pos139) 1.5)
       (= (posiblepos-y pos139) 0.900999128818512)
       (= (posiblepos-z pos139) -0.75)

       (= (posiblepos-x pos140) 1.25)
       (= (posiblepos-y pos140) 0.900999128818512)
       (= (posiblepos-z pos140) -1.0)

       (= (posiblepos-x pos141) 1.5)
       (= (posiblepos-y pos141) 0.900999128818512)
       (= (posiblepos-z pos141) -1.25)

       (= (posiblepos-x pos142) 1.25)
       (= (posiblepos-y pos142) 0.900999128818512)
       (= (posiblepos-z pos142) -1.5)

       (= (posiblepos-x pos143) 1.0)
       (= (posiblepos-y pos143) 0.900999128818512)
       (= (posiblepos-z pos143) -1.75)

       (= (posiblepos-x pos144) 2.0)
       (= (posiblepos-y pos144) 0.900999128818512)
       (= (posiblepos-z pos144) -0.5)

       (= (posiblepos-x pos145) 1.75)
       (= (posiblepos-y pos145) 0.900999128818512)
       (= (posiblepos-z pos145) -0.75)

       (= (posiblepos-x pos146) 1.5)
       (= (posiblepos-y pos146) 0.900999128818512)
       (= (posiblepos-z pos146) -1.0)

       (= (posiblepos-x pos147) 1.75)
       (= (posiblepos-y pos147) 0.900999128818512)
       (= (posiblepos-z pos147) -1.25)

       (= (posiblepos-x pos148) 1.5)
       (= (posiblepos-y pos148) 0.900999128818512)
       (= (posiblepos-z pos148) -1.5)

       (= (posiblepos-x pos149) 1.25)
       (= (posiblepos-y pos149) 0.900999128818512)
       (= (posiblepos-z pos149) -1.75)

       (= (posiblepos-x pos150) 2.0)
       (= (posiblepos-y pos150) 0.900999128818512)
       (= (posiblepos-z pos150) -0.75)

       (= (posiblepos-x pos151) 1.75)
       (= (posiblepos-y pos151) 0.900999128818512)
       (= (posiblepos-z pos151) -1.0)

       (= (posiblepos-x pos152) 2.0)
       (= (posiblepos-y pos152) 0.900999128818512)
       (= (posiblepos-z pos152) -1.25)

       (= (posiblepos-x pos153) 1.75)
       (= (posiblepos-y pos153) 0.900999128818512)
       (= (posiblepos-z pos153) -1.5)

       (= (posiblepos-x pos154) 1.5)
       (= (posiblepos-y pos154) 0.900999128818512)
       (= (posiblepos-z pos154) -1.75)

       (= (posiblepos-x pos155) 2.0)
       (= (posiblepos-y pos155) 0.900999128818512)
       (= (posiblepos-z pos155) -1.0)

       (= (posiblepos-x pos156) 2.0)
       (= (posiblepos-y pos156) 0.900999128818512)
       (= (posiblepos-z pos156) -1.5)

       (= (posiblepos-x pos157) 1.75)
       (= (posiblepos-y pos157) 0.900999128818512)
       (= (posiblepos-z pos157) -1.75)

       (= (posiblepos-x pos158) 1.5)
       (= (posiblepos-y pos158) 0.900999128818512)
       (= (posiblepos-z pos158) -2.0)

       (= (posiblepos-x pos159) 2.0)
       (= (posiblepos-y pos159) 0.900999128818512)
       (= (posiblepos-z pos159) -1.75)

       (= (posiblepos-x pos160) 1.75)
       (= (posiblepos-y pos160) 0.900999128818512)
       (= (posiblepos-z pos160) -2.0)

       (= (posiblepos-x pos161) 2.0)
       (= (posiblepos-y pos161) 0.900999128818512)
       (= (posiblepos-z pos161) -2.0)

       (= (object-at-x Apple|-00.47|+01.15|+00.48) -0.4651542901992798)
       (= (object-at-y Apple|-00.47|+01.15|+00.48) 1.151225209236145)
       (= (object-at-z Apple|-00.47|+01.15|+00.48) 0.4757418632507324)

       (= (object-at-x Book|+00.15|+01.10|+00.62) 0.15499889850616455)
       (= (object-at-y Book|+00.15|+01.10|+00.62) 1.0992614030838013)
       (= (object-at-z Book|+00.15|+01.10|+00.62) 0.6169992685317993)

       (= (object-at-x Bottle|+01.54|+00.89|-02.54) 1.5399900674819946)
       (= (object-at-y Bottle|+01.54|+00.89|-02.54) 0.8861729502677917)
       (= (object-at-z Bottle|+01.54|+00.89|-02.54) -2.539963960647583)

       (= (object-at-x Bowl|+00.27|+01.10|-00.75) 0.2731872797012329)
       (= (object-at-y Bowl|+00.27|+01.10|-00.75) 1.1010472774505615)
       (= (object-at-z Bowl|+00.27|+01.10|-00.75) -0.7532863020896912)

       (= (object-at-x Bread|-00.52|+01.17|-00.03) -0.5223836302757263)
       (= (object-at-y Bread|-00.52|+01.17|-00.03) 1.1704490184783936)
       (= (object-at-z Bread|-00.52|+01.17|-00.03) -0.028279822319746017)

       (= (object-at-x ButterKnife|-00.41|+01.11|-00.46) -0.41180741786956787)
       (= (object-at-y ButterKnife|-00.41|+01.11|-00.46) 1.1053317785263062)
       (= (object-at-z ButterKnife|-00.41|+01.11|-00.46) -0.462016224861145)

       (= (object-at-x Cabinet|+00.68|+00.50|-02.20) 0.6800000071525574)
       (= (object-at-y Cabinet|+00.68|+00.50|-02.20) 0.4949999749660492)
       (= (object-at-z Cabinet|+00.68|+00.50|-02.20) -2.200000047683716)

       (= (object-at-x Cabinet|-01.18|+00.50|-02.20) -1.1799999475479126)
       (= (object-at-y Cabinet|-01.18|+00.50|-02.20) 0.4949999749660492)
       (= (object-at-z Cabinet|-01.18|+00.50|-02.20) -2.200000047683716)

       (= (object-at-x Cabinet|-01.55|+00.50|+00.38) -1.5499999523162842)
       (= (object-at-y Cabinet|-01.55|+00.50|+00.38) 0.4949999749660492)
       (= (object-at-z Cabinet|-01.55|+00.50|+00.38) 0.3799999952316284)

       (= (object-at-x Cabinet|+00.72|+02.02|-02.46) 0.7199999690055847)
       (= (object-at-y Cabinet|+00.72|+02.02|-02.46) 2.015000104904175)
       (= (object-at-z Cabinet|+00.72|+02.02|-02.46) -2.4600000381469727)

       (= (object-at-x Cabinet|-01.85|+02.02|+00.38) -1.8499999046325684)
       (= (object-at-y Cabinet|-01.85|+02.02|+00.38) 2.015000104904175)
       (= (object-at-z Cabinet|-01.85|+02.02|+00.38) 0.3799999952316284)

       (= (object-at-x Cabinet|+00.68|+02.02|-02.46) 0.6800000071525574)
       (= (object-at-y Cabinet|+00.68|+02.02|-02.46) 2.015000104904175)
       (= (object-at-z Cabinet|+00.68|+02.02|-02.46) -2.4600000381469727)

       (= (object-at-x Cabinet|-01.55|+00.50|-01.97) -1.5499999523162842)
       (= (object-at-y Cabinet|-01.55|+00.50|-01.97) 0.4949999749660492)
       (= (object-at-z Cabinet|-01.55|+00.50|-01.97) -1.9649999141693115)

       (= (object-at-x Cabinet|-01.69|+02.02|-02.46) -1.6852999925613403)
       (= (object-at-y Cabinet|-01.69|+02.02|-02.46) 2.015000104904175)
       (= (object-at-z Cabinet|-01.69|+02.02|-02.46) -2.4616000652313232)

       (= (object-at-x Cabinet|-00.73|+02.02|-02.46) -0.7278000116348267)
       (= (object-at-y Cabinet|-00.73|+02.02|-02.46) 2.015000104904175)
       (= (object-at-z Cabinet|-00.73|+02.02|-02.46) -2.4600000381469727)

       (= (object-at-x CoffeeMachine|-01.98|+00.90|-00.19) -1.9780069589614868)
       (= (object-at-y CoffeeMachine|-01.98|+00.90|-00.19) 0.8999993801116943)
       (= (object-at-z CoffeeMachine|-01.98|+00.90|-00.19) -0.18905173242092133)

       (= (object-at-x CounterTop|+00.69|+00.95|-02.48) 0.6930000185966492)
       (= (object-at-y CounterTop|+00.69|+00.95|-02.48) 0.9462000131607056)
       (= (object-at-z CounterTop|+00.69|+00.95|-02.48) -2.4839999675750732)

       (= (object-at-x CounterTop|-00.08|+01.15|00.00) -0.07900000363588333)
       (= (object-at-y CounterTop|-00.08|+01.15|00.00) 1.1470999717712402)
       (= (object-at-z CounterTop|-00.08|+01.15|00.00) -0.0010000000474974513)

       (= (object-at-x CounterTop|-01.87|+00.95|-01.21) -1.8680000305175781)
       (= (object-at-y CounterTop|-01.87|+00.95|-01.21) 0.9469000101089478)
       (= (object-at-z CounterTop|-01.87|+00.95|-01.21) -1.2059999704360962)

       (= (object-at-x CreditCard|-00.46|+01.10|+00.87) -0.463664710521698)
       (= (object-at-y CreditCard|-00.46|+01.10|+00.87) 1.1003444194793701)
       (= (object-at-z CreditCard|-00.46|+01.10|+00.87) 0.8652696013450623)

       (= (object-at-x Cup|+00.37|+01.64|-02.58) 0.37134993076324463)
       (= (object-at-y Cup|+00.37|+01.64|-02.58) 1.644809603691101)
       (= (object-at-z Cup|+00.37|+01.64|-02.58) -2.5791807174682617)

       (= (object-at-x DishSponge|-01.94|+00.75|-01.71) -1.9376826286315918)
       (= (object-at-y DishSponge|-01.94|+00.75|-01.71) 0.6766930818557739)
       (= (object-at-z DishSponge|-01.94|+00.75|-01.71) -1.7105848789215088)

       (= (object-at-x Drawer|-01.56|+00.66|-00.20) -1.5604000091552734)
       (= (object-at-y Drawer|-01.56|+00.66|-00.20) 0.6552000045776367)
       (= (object-at-z Drawer|-01.56|+00.66|-00.20) -0.19699999690055847)

       (= (object-at-x Drawer|+00.95|+00.83|-02.20) 0.949999988079071)
       (= (object-at-y Drawer|+00.95|+00.83|-02.20) 0.8327999711036682)
       (= (object-at-z Drawer|+00.95|+00.83|-02.20) -2.197000026702881)

       (= (object-at-x Drawer|+00.95|+00.56|-02.20) 0.949999988079071)
       (= (object-at-y Drawer|+00.95|+00.56|-02.20) 0.5600000023841858)
       (= (object-at-z Drawer|+00.95|+00.56|-02.20) -2.2011001110076904)

       (= (object-at-x Drawer|-01.56|+00.84|+00.20) -1.5609999895095825)
       (= (object-at-y Drawer|-01.56|+00.84|+00.20) 0.8396000266075134)
       (= (object-at-z Drawer|-01.56|+00.84|+00.20) 0.2045000046491623)

       (= (object-at-x Drawer|+00.95|+00.22|-02.20) 0.949999988079071)
       (= (object-at-y Drawer|+00.95|+00.22|-02.20) 0.22100000083446503)
       (= (object-at-z Drawer|+00.95|+00.22|-02.20) -2.2009999752044678)

       (= (object-at-x Drawer|+00.95|+00.71|-02.20) 0.949999988079071)
       (= (object-at-y Drawer|+00.95|+00.71|-02.20) 0.7081000208854675)
       (= (object-at-z Drawer|+00.95|+00.71|-02.20) -2.200200080871582)

       (= (object-at-x Drawer|+00.95|+00.39|-02.20) 0.949999988079071)
       (= (object-at-y Drawer|+00.95|+00.39|-02.20) 0.39399999380111694)
       (= (object-at-z Drawer|+00.95|+00.39|-02.20) -2.2009999752044678)

       (= (object-at-x Drawer|-01.56|+00.33|-00.20) -1.5604000091552734)
       (= (object-at-y Drawer|-01.56|+00.33|-00.20) 0.3310000002384186)
       (= (object-at-z Drawer|-01.56|+00.33|-00.20) -0.19699999690055847)

       (= (object-at-x Drawer|-01.56|+00.84|-00.20) -1.5609999895095825)
       (= (object-at-y Drawer|-01.56|+00.84|-00.20) 0.8396000266075134)
       (= (object-at-z Drawer|-01.56|+00.84|-00.20) -0.19709999859333038)

       (= (object-at-x Egg|-02.04|+00.81|+01.24) -2.037466049194336)
       (= (object-at-y Egg|-02.04|+00.81|+01.24) 0.8123339414596558)
       (= (object-at-z Egg|-02.04|+00.81|+01.24) 1.2446753978729248)

       (= (object-at-x Faucet|-02.15|+00.91|-01.50) -2.1510000228881836)
       (= (object-at-y Faucet|-02.15|+00.91|-01.50) 0.906000018119812)
       (= (object-at-z Faucet|-02.15|+00.91|-01.50) -1.4989999532699585)

       (= (object-at-x Floor|+00.00|+00.00|+00.00) 0.0)
       (= (object-at-y Floor|+00.00|+00.00|+00.00) 0.0)
       (= (object-at-z Floor|+00.00|+00.00|+00.00) 0.0)

       (= (object-at-x Fork|+00.95|+00.77|-02.37) 0.9492349624633789)
       (= (object-at-y Fork|+00.95|+00.77|-02.37) 0.7680965065956116)
       (= (object-at-z Fork|+00.95|+00.77|-02.37) -2.3652503490448)

       (= (object-at-x Fridge|-02.10|+00.00|+01.07) -2.0969998836517334)
       (= (object-at-y Fridge|-02.10|+00.00|+01.07) 0.0)
       (= (object-at-z Fridge|-02.10|+00.00|+01.07) 1.0720000267028809)

       (= (object-at-x GarbageCan|-01.94|00.00|+02.03) -1.934999704360962)
       (= (object-at-y GarbageCan|-01.94|00.00|+02.03) -0.0022865384817123413)
       (= (object-at-z GarbageCan|-01.94|00.00|+02.03) 2.029000997543335)

       (= (object-at-x HousePlant|-01.95|+00.89|-02.52) -1.9487520456314087)
       (= (object-at-y HousePlant|-01.95|+00.89|-02.52) 0.8894973993301392)
       (= (object-at-z HousePlant|-01.95|+00.89|-02.52) -2.5208652019500732)

       (= (object-at-x Kettle|+01.04|+00.90|-02.60) 1.0373425483703613)
       (= (object-at-y Kettle|+01.04|+00.90|-02.60) 0.9045257568359375)
       (= (object-at-z Kettle|+01.04|+00.90|-02.60) -2.5963099002838135)

       (= (object-at-x Knife|-01.70|+00.79|-00.22) -1.7018152475357056)
       (= (object-at-y Knife|-01.70|+00.79|-00.22) 0.790888249874115)
       (= (object-at-z Knife|-01.70|+00.79|-00.22) -0.22279910743236542)

       (= (object-at-x Lettuce|-01.81|+00.97|-00.94) -1.8069758415222168)
       (= (object-at-y Lettuce|-01.81|+00.97|-00.94) 0.9737613201141357)
       (= (object-at-z Lettuce|-01.81|+00.97|-00.94) -0.9429914951324463)

       (= (object-at-x LightSwitch|+02.33|+01.31|-00.16) 2.3299999237060547)
       (= (object-at-y LightSwitch|+02.33|+01.31|-00.16) 1.309000015258789)
       (= (object-at-z LightSwitch|+02.33|+01.31|-00.16) -0.1599999964237213)

       (= (object-at-x Microwave|-00.24|+01.69|-02.53) -0.23699797689914703)
       (= (object-at-y Microwave|-00.24|+01.69|-02.53) 1.689012885093689)
       (= (object-at-z Microwave|-00.24|+01.69|-02.53) -2.5251078605651855)

       (= (object-at-x Mug|-01.76|+00.90|-00.62) -1.7607921361923218)
       (= (object-at-y Mug|-01.76|+00.90|-00.62) 0.9000070691108704)
       (= (object-at-z Mug|-01.76|+00.90|-00.62) -0.6206092834472656)

       (= (object-at-x Pan|+00.72|+00.90|-02.42) 0.7211822271347046)
       (= (object-at-y Pan|+00.72|+00.90|-02.42) 0.9000632762908936)
       (= (object-at-z Pan|+00.72|+00.90|-02.42) -2.4154083728790283)

       (= (object-at-x PaperTowelRoll|-02.06|+01.01|-00.81) -2.0628509521484375)
       (= (object-at-y PaperTowelRoll|-02.06|+01.01|-00.81) 1.0074869394302368)
       (= (object-at-z PaperTowelRoll|-02.06|+01.01|-00.81) -0.814866840839386)

       (= (object-at-x PepperShaker|+00.30|+00.90|-02.47) 0.3000265657901764)
       (= (object-at-y PepperShaker|+00.30|+00.90|-02.47) 0.9000789523124695)
       (= (object-at-z PepperShaker|+00.30|+00.90|-02.47) -2.465442419052124)

       (= (object-at-x Plate|+00.96|+01.65|-02.61) 0.956000030040741)
       (= (object-at-y Plate|+00.96|+01.65|-02.61) 1.64607572555542)
       (= (object-at-z Plate|+00.96|+01.65|-02.61) -2.6061999797821045)

       (= (object-at-x Pot|-01.22|+00.90|-02.36) -1.2190016508102417)
       (= (object-at-y Pot|-01.22|+00.90|-02.36) 0.9009172916412354)
       (= (object-at-z Pot|-01.22|+00.90|-02.36) -2.355999708175659)

       (= (object-at-x Potato|-01.66|+00.93|-02.15) -1.6557015180587769)
       (= (object-at-y Potato|-01.66|+00.93|-02.15) 0.9333286881446838)
       (= (object-at-z Potato|-01.66|+00.93|-02.15) -2.150597095489502)

       (= (object-at-x SaltShaker|+00.35|+00.90|-02.57) 0.34677639603614807)
       (= (object-at-y SaltShaker|+00.35|+00.90|-02.57) 0.9000768661499023)
       (= (object-at-z SaltShaker|+00.35|+00.90|-02.57) -2.566875696182251)

       (= (object-at-x Shelf|+01.75|+00.17|-02.56) 1.7510097026824951)
       (= (object-at-y Shelf|+01.75|+00.17|-02.56) 0.1726451963186264)
       (= (object-at-z Shelf|+01.75|+00.17|-02.56) -2.5595011711120605)

       (= (object-at-x Shelf|+01.75|+00.88|-02.56) 1.751010775566101)
       (= (object-at-y Shelf|+01.75|+00.88|-02.56) 0.8809456825256348)
       (= (object-at-z Shelf|+01.75|+00.88|-02.56) -2.5601892471313477)

       (= (object-at-x Shelf|+01.75|+00.55|-02.56) 1.7510102987289429)
       (= (object-at-y Shelf|+01.75|+00.55|-02.56) 0.5529817342758179)
       (= (object-at-z Shelf|+01.75|+00.55|-02.56) -2.559494733810425)

       (= (object-at-x ShelvingUnit|+01.75|+00.01|-02.56) 1.751009464263916)
       (= (object-at-y ShelvingUnit|+01.75|+00.01|-02.56) 0.006781816482543945)
       (= (object-at-z ShelvingUnit|+01.75|+00.01|-02.56) -2.560004234313965)

       (= (object-at-x Sink|-01.90|+00.97|-01.50) -1.899999976158142)
       (= (object-at-y Sink|-01.90|+00.97|-01.50) 0.9649999737739563)
       (= (object-at-z Sink|-01.90|+00.97|-01.50) -1.5)

       (= (object-at-x Sink|-01.90|+00.97|-01.50|SinkBasin) -1.9081000089645386)
       (= (object-at-y Sink|-01.90|+00.97|-01.50|SinkBasin) 0.7591999769210815)
       (= (object-at-z Sink|-01.90|+00.97|-01.50|SinkBasin) -1.488800048828125)

       (= (object-at-x SoapBottle|-01.99|+00.90|-02.03) -1.9914913177490234)
       (= (object-at-y SoapBottle|-01.99|+00.90|-02.03) 0.8998796939849854)
       (= (object-at-z SoapBottle|-01.99|+00.90|-02.03) -2.0323102474212646)

       (= (object-at-x Spatula|+00.38|+00.91|-02.33) 0.38115280866622925)
       (= (object-at-y Spatula|+00.38|+00.91|-02.33) 0.907408595085144)
       (= (object-at-z Spatula|+00.38|+00.91|-02.33) -2.3329858779907227)

       (= (object-at-x Spoon|+00.98|+00.77|-02.29) 0.9829990863800049)
       (= (object-at-y Spoon|+00.98|+00.77|-02.29) 0.7667461037635803)
       (= (object-at-z Spoon|+00.98|+00.77|-02.29) -2.2869675159454346)

       (= (object-at-x Statue|+01.96|+00.18|-02.54) 1.9550410509109497)
       (= (object-at-y Statue|+01.96|+00.18|-02.54) 0.18092237412929535)
       (= (object-at-z Statue|+01.96|+00.18|-02.54) -2.5430257320404053)

       (= (object-at-x Stool|+00.70|+00.00|-00.51) 0.7027491331100464)
       (= (object-at-y Stool|+00.70|+00.00|-00.51) 0.000585019588470459)
       (= (object-at-z Stool|+00.70|+00.00|-00.51) -0.5144155621528625)

       (= (object-at-x Stool|+00.74|+00.00|+00.56) 0.7430251836776733)
       (= (object-at-y Stool|+00.74|+00.00|+00.56) 0.0005829930305480957)
       (= (object-at-z Stool|+00.74|+00.00|+00.56) 0.5636947751045227)

       (= (object-at-x StoveBurner|-00.47|+00.92|-02.37) -0.4652000069618225)
       (= (object-at-y StoveBurner|-00.47|+00.92|-02.37) 0.9150999784469604)
       (= (object-at-z StoveBurner|-00.47|+00.92|-02.37) -2.371999979019165)

       (= (object-at-x StoveBurner|-00.04|+00.92|-02.58) -0.03610000014305115)
       (= (object-at-y StoveBurner|-00.04|+00.92|-02.58) 0.9150999784469604)
       (= (object-at-z StoveBurner|-00.04|+00.92|-02.58) -2.576200008392334)

       (= (object-at-x StoveBurner|-00.47|+00.92|-02.58) -0.4652000069618225)
       (= (object-at-y StoveBurner|-00.47|+00.92|-02.58) 0.9150999784469604)
       (= (object-at-z StoveBurner|-00.47|+00.92|-02.58) -2.5759999752044678)

       (= (object-at-x StoveBurner|-00.04|+00.92|-02.37) -0.03610000014305115)
       (= (object-at-y StoveBurner|-00.04|+00.92|-02.37) 0.9150999784469604)
       (= (object-at-z StoveBurner|-00.04|+00.92|-02.37) -2.3722000122070312)

       (= (object-at-x StoveKnob|-00.48|+00.88|-02.19) -0.48350000381469727)
       (= (object-at-y StoveKnob|-00.48|+00.88|-02.19) 0.8813999891281128)
       (= (object-at-z StoveKnob|-00.48|+00.88|-02.19) -2.1875998973846436)

       (= (object-at-x StoveKnob|-00.02|+00.88|-02.19) -0.022000014781951904)
       (= (object-at-y StoveKnob|-00.02|+00.88|-02.19) 0.8813999891281128)
       (= (object-at-z StoveKnob|-00.02|+00.88|-02.19) -2.1875998973846436)

       (= (object-at-x StoveKnob|-00.33|+00.88|-02.19) -0.32899999618530273)
       (= (object-at-y StoveKnob|-00.33|+00.88|-02.19) 0.8813999891281128)
       (= (object-at-z StoveKnob|-00.33|+00.88|-02.19) -2.1875998973846436)

       (= (object-at-x StoveKnob|-00.18|+00.88|-02.19) -0.18310000002384186)
       (= (object-at-y StoveKnob|-00.18|+00.88|-02.19) 0.8813999891281128)
       (= (object-at-z StoveKnob|-00.18|+00.88|-02.19) -2.1875998973846436)

       (= (object-at-x Toaster|-01.84|+00.90|+00.13) -1.8350030183792114)
       (= (object-at-y Toaster|-01.84|+00.90|+00.13) 0.9000004529953003)
       (= (object-at-z Toaster|-01.84|+00.90|+00.13) 0.1340002715587616)

       (= (object-at-x Tomato|-00.39|+01.14|-00.81) -0.3850599229335785)
       (= (object-at-y Tomato|-00.39|+01.14|-00.81) 1.1370395421981812)
       (= (object-at-z Tomato|-00.39|+01.14|-00.81) -0.8119527101516724)

       (= (object-at-x Vase|+01.56|+00.56|-02.50) 1.5619902610778809)
       (= (object-at-y Vase|+01.56|+00.56|-02.50) 0.5563273429870605)
       (= (object-at-z Vase|+01.56|+00.56|-02.50) -2.499983310699463)

       (= (object-at-x Vase|+01.99|+00.56|-02.49) 1.9880303144454956)
       (= (object-at-y Vase|+01.99|+00.56|-02.49) 0.5554139018058777)
       (= (object-at-z Vase|+01.99|+00.56|-02.49) -2.486996650695801)

       (= (object-at-x Window|-02.23|+01.43|-01.20) -2.2300000190734863)
       (= (object-at-y Window|-02.23|+01.43|-01.20) 1.4299999475479126)
       (= (object-at-z Window|-02.23|+01.43|-01.20) -1.2039999961853027)

       (= (object-at-x WineBottle|-01.00|+01.65|-02.58) -1.001008152961731)
       (= (object-at-y WineBottle|-01.00|+01.65|-02.58) 1.6450198888778687)
       (= (object-at-z WineBottle|-01.00|+01.65|-02.58) -2.583832263946533)

    )

    (:goal (and
        (holding Apple|-00.47|+01.15|+00.48)
    ))
)

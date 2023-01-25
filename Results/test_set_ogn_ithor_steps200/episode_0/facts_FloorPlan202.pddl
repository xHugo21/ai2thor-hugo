(define (problem ithor-appleinbox)
(:domain ithor)
(:objects
armchair_0 - armchair
armchair_1 - armchair
armchair_2 - armchair
armchair_3 - armchair
armchair_4 - armchair
armchair_5 - armchair
armchair_6 - armchair
coffeetable_0 - coffeetable
curtains_0 - curtains
curtains_1 - curtains
curtains_2 - curtains
floorlamp_0 - floorlamp
houseplant_0 - houseplant
houseplant_1 - houseplant
houseplant_2 - houseplant
laptop_0 - laptop
painting_0 - painting
painting_1 - painting
sofa_0 - sofa
sofa_1 - sofa
)
(:init
(close_to armchair_0)
(close_to armchair_0)
(close_to armchair_1)
(close_to armchair_1)
(close_to armchair_2)
(close_to armchair_2)
(close_to armchair_3)
(close_to armchair_3)
(close_to armchair_4)
(close_to armchair_4)
(close_to armchair_5)
(close_to armchair_5)
(close_to armchair_6)
(close_to armchair_6)
(close_to coffeetable_0)
(close_to coffeetable_0)
(close_to curtains_0)
(close_to curtains_0)
(close_to curtains_1)
(close_to curtains_1)
(close_to curtains_2)
(close_to curtains_2)
(close_to laptop_0)
(close_to laptop_0)
(close_to sofa_0)
(close_to sofa_0)
(close_to sofa_1)
(discovered armchair_0)
(discovered armchair_1)
(discovered armchair_2)
(discovered armchair_3)
(discovered armchair_4)
(discovered armchair_5)
(discovered armchair_6)
(discovered coffeetable_0)
(discovered curtains_0)
(discovered curtains_1)
(discovered curtains_2)
(discovered floorlamp_0)
(discovered houseplant_0)
(discovered houseplant_1)
(discovered houseplant_2)
(discovered laptop_0)
(discovered painting_0)
(discovered painting_1)
(discovered sofa_0)
(discovered sofa_1)
(hand_free )
(inspected armchair_6)
(inspected armchair_6)
(inspected curtains_1)
(inspected laptop_0)
(inspected laptop_0)
(inspected sofa_1)
(inspected sofa_1)
(open laptop_0)
(openable laptop_0)
(pickupable laptop_0)
(receptacle armchair_0)
(receptacle armchair_1)
(receptacle armchair_2)
(receptacle armchair_3)
(receptacle armchair_4)
(receptacle armchair_5)
(receptacle armchair_6)
(receptacle coffeetable_0)
(receptacle sofa_0)
(receptacle sofa_1)
(viewing armchair_6)
(viewing laptop_0)
(viewing painting_1)
(viewing sofa_1)
)
(:goal
(and
(exists (?o1 - laptop) (and (viewing ?o1) (close_to ?o1))))
))
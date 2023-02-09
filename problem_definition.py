# Fichero que contiene la clase que gestiona los parámetros de entrada para la ejecución del problema
#TODO comprobación errores

# Imports
import os

class ProblemDefinition():
    def __init__(self):
        '''Clase que contiene todos los métodos para recoger los parámetros de entrada'''
        self.problem_list = []
        self.objective_list = []

    def method_selection(self):
        '''Método que permite seleccionar el metodo de resolucion del problema
            1. METADATA: Utiliza los metadatos devueltos por el simulador para conocer las posiciones de los objetos y posiciones válidas
            2. OGAMUS: Utiliza redes neuronales para reconocer los objetos de una escena'''
        bucle = True
        while bucle:
            print("----METODO----")
            print("[1] - METADATA")
            print("[2] - OGAMUS")
            print("--------------")
            self.method = int(input("Seleccione un método de resolución del problema: "))
            print("")
            if 1 <= self.method <= 2:
                bucle = False
                self.method = str(self.method)
            else:
                print("Por favor, introduce un método válido\n")

        return self.method
        

    def scene_selection(self):
        '''Método que permite seleccionar la escena a utilizar'''
        bucle = True
        while bucle:
            print("----ESCENAS----")
            print("[1-30] - Cocinas")
            print("[201-230] - Salas de estar")
            print("[301-330] - Dormitorios")
            print("[401-430] - Baños")
            print("---------------")
            self.scene_number = int(input("Seleccione una escena: "))
            print("")
            if (1 <= self.scene_number <= 30) or (201 <= self.scene_number <= 230) or (301 <= self.scene_number <= 330) or (401 <= self.scene_number <= 430):
                bucle = False
                self.scene_number = str(self.scene_number)
            else:
                print("Por favor, introduce una escena válida\n")

        return self.scene_number

    def paths_selection(self, method, iteracion):
        '''Método que permite seleccionar las rutas de los archivos del planificador, problema y output'''
        if method == '1':
            self.planner_path = "./pddl/cbp-roller/cbp-roller"
            self.problem_path = f'./pddl/problems/problem{iteracion}.pddl'
            self.output_path = f'./pddl/outputs/problem{iteracion}.txt'
        else:
            self.planner_path = './OGAMUS/Plan/PDDL/Planners/FF/ff'
            self.problem_path = f'./OGAMUS/Plan/PDDL/facts.pddl'
            self.output_path = f'./pddl/outputs/problem{iteracion}.txt'

        return self.planner_path, self.problem_path, self.output_path

    def problem_selection(self, event):
        '''Método que permite escoger el problema (acción) a realizar junto a su respectivo objetivo'''
        self.liquid = 'coffee'
        self.event = event
        bucle = True
        holding = False
        while bucle:
            print("----PROBLEMA----")
            print("[1] - Move Agent")
            print("[2] - Pickup Object")
            print("[3] - Open Object")
            print("[4] - Close Object")
            print("[5] - Break Object")
            print("[6] - Cook Object")
            print("[7] - Slice Object")
            print("[8] - Toggle On Object")
            print("[9] - Toggle Off Object")
            print("[10] - Dirty Object")
            print("[11] - Clean Object")
            print("[12] - Fill Object")
            print("[13] - Empty Object")
            print("[14] - Use Up Object")
            print("[15] - Drop Object (Requires holding an object)")
            print("[16] - Put Object (Requires holding an object)")
            print("----------------")
            aux = input("Seleccione un tipo de problema a resolver: ")
            print("")
            for obj in event.metadata['objects']:
                if obj['isPickedUp']:
                    holding = True
            if (1 <= int(aux) <= 14):
                bucle = False
            elif (15 <= int(aux) <= 16) and holding:
                bucle = False
            else:
                print("Por favor, introduce un problema válido\n")


        # Establecer parámetros en caso de que se seleccione problema de movimiento
        if str(aux) == '1':
            self.problem = "move"
            print("----OBJETIVO----")
            positions = event.metadata["actionReturn"]
            i = 0
            for pos in positions:
                print(f'[{i}] - {pos}')
                i += 1
            print("----------------")
            aux2 = input("Seleccione la posición objetivo: ")
            self.objective = positions[int(aux2)]
            print("")
            print(f'La posición objetivo seleccionada es: {self.objective}')

        # Establecer parámetros en caso de que se seleccione problema de pickup
        if str(aux) == '2':
            self.object_selection("pickup", "pickupable", "isPickedUp", False)

        if str(aux) == '3':
            self.object_selection("open", "openable", "isOpen", False)

        if str(aux) == '4':
            self.object_selection("close", "openable", "isOpen", True)
        
        if str(aux) == '5':
            self.object_selection("break", "breakable", "isBroken", False)
        
        if str(aux) == '6':
            self.object_selection("cook", "cookable", "isCooked", False)

        if str(aux) == '7':
            self.object_selection("slice", "sliceable", "isSliced", False)

        if str(aux) == '8':
            self.object_selection("toggleon", "toggleable", "isToggled", False)

        if str(aux) == '9':
            self.object_selection("toggleoff", "toggleable", "isToggled", True)

        if str(aux) == '10':
            self.object_selection("dirty", "dirtyable", "isDirty", False)

        if str(aux) == '11':
            self.object_selection("clean", "dirtyable", "isDirty", True)
        
        if str(aux) == '12':
            self.object_selection("fill", "canFillWithLiquid", "isFilledWithLiquid", False, True)

        if str(aux) == '13':
            self.object_selection("empty", "canFillWithLiquid", "isFilledWithLiquid", True)
        
        if str(aux) == '14':
            self.object_selection("useup", "canBeUsedUp", "isUsedUp", False)
        
        if str(aux) == '15':
            self.object_selection("drop", "pickupable", "isPickedUp", True)
        
        if str(aux) == '16':
            self.object_selection("put", "receptacle", "receptacle", True)

            
        return self.problem, self.objective, self.liquid

    def object_selection(self, problem_type, condition1, condition2, condition2_res, select_liquid=False):
        '''Método que encapsula la selección de objetivos de la mayoría de problemas'''
        self.problem = problem_type
        posible_objects = []
        for obj in self.event.metadata["objects"]:
            if (obj[condition1] == True) and (obj[condition2] == condition2_res):
                posible_objects.append(obj)
        if not posible_objects:
            print("No existen objetivos para esa tarea. Escoja otra o cambie de escena\n")
            return self.problem_selection(self.event)
        print("----OBJETIVO----")         
        i = 0
        for obj in posible_objects:
            print(f'[{i}] - {obj["objectId"]}')
            i += 1
        print("----------------")
        aux2 = input("Seleccione el objetivo: ")
        self.objective = posible_objects[int(aux2)]            
        print("")
        print(f'El objetivo seleccionado es: {self.objective["objectId"]}\n')

        if select_liquid:
            print("----PROBLEMA----")
            print("[1] - Coffee")
            print("[2] - Wine")
            print("[3] - Water")
            print("----------------")
            inp = input(f'Seleccione el líquido con el que rellenar el objeto: ')

            if str(inp) == '1':
                self.liquid = 'coffee'
            elif str(inp) == '2':
                self.liquid = 'wine'
            elif str(inp) == '3':
                self.liquid = 'water'

    def problem_selection_ogamus(self):
        
        bucle = True
        while bucle:
            print("----PROBLEMA----")
            print("[1] - Get Close To Object")
            print("[2] - Pickup Object")
            print("[3] - Open Object")
            print("[4] - Close Object")
            # print("[5] - Break Object")
            # print("[6] - Cook Object")
            # print("[7] - Slice Object")
            # print("[8] - Toggle On Object")
            # print("[9] - Toggle Off Object")
            # print("[10] - Dirty Object")
            # print("[11] - Clean Object")
            # print("[12] - Fill Object")
            # print("[13] - Empty Object")
            # print("[14] - Use Up Object")
            # print("[15] - Drop Object (Requires holding an object)")
            print("[5] - Drop Object (Requires holding an object)")
            print("[6] - Put Object (Requires holding an object)")
            print("----------------")
            aux = input("Seleccione un tipo de problema a resolver: ")
            print("")
            if (1 <= int(aux) <= 15):
                bucle = False
            else:
                print("Por favor, introduce un problema válido\n")
        
        problemas_ogamus = ["get_close_to", "pickup", "open", "close", "drop", "put"]
        self.problem = problemas_ogamus[int(aux)-1]
        self.problem_list.append(self.problem)

        self.object_selection_ogamus()

        print("")
        condition = input('Desea realizar otra acción sobre este entorno [Y/n]: ')
        print("")
        if condition == 'Y':
            self.problem_selection_ogamus()

        return self.problem_list, self.objective_list
    
    def object_selection_ogamus(self):
        if self.problem == "get_close_to":
            possible_objects = ["alarmclock", "aluminumfoil", "apple", "baseballbat", "book", "boots", "basketball",
                    "bottle", "bowl", "box", "bread", "butterknife", "candle", "cd", "cellphone", "peppershaker",
                    "cloth", "creditcard", "cup", "dishsponge", "dumbbell", "egg", "fork", "handtowel",
                    "kettle", "keychain", "knife", "ladle", "laptop", "lettuce", "mug", "newspaper",
                    "pan", "papertowel", "papertowelroll", "pen", "pencil", "papershaker", "pillow", "plate", "plunger",
                    "pot", "potato", "remotecontrol", "saltshaker", "scrubbrush", "soapbar", "soapbottle",
                    "spatula", "spoon", "spraybottle", "statue", "tabletopdecor", "teddybear", "tennisracket",
                    "tissuebox", "toiletpaper", "tomato", "towel", "vase", "watch", "wateringcan", "winebottle", "armchair", "bathtub", "bathtubbasin", "bed", "bowl", "box", "cabinet", "coffeemachine",
                   "coffeetable", "countertop", "desk", "diningtable", "drawer", "fridge", 
                   "garbagecan", "handtowelholder", "laundryhamper", "microwave", "mug", "ottoman", "pan",
                   "plate", "pot", "safe", "shelf", "sidetable", "sinkbasin", "sofa", "toaster",
                   "toilet", "toiletpaperhanger", "towelholder", "tvstand", "stoveburner", "blinds", "book", "box", "cabinet", "drawer", "fridge", "kettle", "laptop", "microwave",
                 "safe", "showercurtain", "showerdoor", "toilet"]

        elif self.problem == "pickup" or self.problem == "drop":
            possible_objects = ["alarmclock", "aluminumfoil", "apple", "baseballbat", "book", "boots", "basketball",
                    "bottle", "bowl", "box", "bread", "butterknife", "candle", "cd", "cellphone", "peppershaker",
                    "cloth", "creditcard", "cup", "dishsponge", "dumbbell", "egg", "fork", "handtowel",
                    "kettle", "keychain", "knife", "ladle", "laptop", "lettuce", "mug", "newspaper",
                    "pan", "papertowel", "papertowelroll", "pen", "pencil", "papershaker", "pillow", "plate", "plunger",
                    "pot", "potato", "remotecontrol", "saltshaker", "scrubbrush", "soapbar", "soapbottle",
                    "spatula", "spoon", "spraybottle", "statue", "tabletopdecor", "teddybear", "tennisracket",
                    "tissuebox", "toiletpaper", "tomato", "towel", "vase", "watch", "wateringcan", "winebottle"]
                    
        elif self.problem == "open" or self.problem == "close":
            possible_objects = ["blinds", "book", "box", "cabinet", "drawer", "fridge", "kettle", "laptop", "microwave",
                 "safe", "showercurtain", "showerdoor", "toilet"]

        elif self.problem == "put":
            possible_objects = ["armchair", "bathtub", "bathtubbasin", "bed", "bowl", "box", "cabinet", "coffeemachine",
                   "coffeetable", "countertop", "desk", "diningtable", "drawer", "fridge",
                   "garbagecan", "handtowelholder", "laundryhamper", "microwave", "mug", "ottoman", "pan",
                   "plate", "pot", "safe", "shelf", "sidetable", "sinkbasin", "sofa", "toaster",
                   "toilet", "toiletpaperhanger", "towelholder", "tvstand", "stoveburner"]

        print("----OBJETIVO----")
        i = 0
        for obj in possible_objects:
            print(f'[{i}] - {obj}')
            i += 1
        print("----------------")
        aux2 = input("Seleccione el objetivo: ")
        self.objective = possible_objects[int(aux2)]
        self.objective_list.append(self.objective)
    
    def problem_selection_ogamus_input(self, input):
        
        try:
            os.system(f'./pddl/cbp-roller/cbp-roller -o ./pddl/domain_input.pddl -f {input} > ./pddl/outputs/inputs.txt')
        except FileNotFoundError:
            raise Exception("Error al ejecutar el planificador (Archivo no encontrado)")

        with open('./pddl/outputs/inputs.txt', 'r') as f:
            raw_plan = f.read()
            print(raw_plan)
        
            start_index = raw_plan.find("0:")
            end_index = raw_plan.find("time")

            plan = raw_plan[start_index:end_index] # Trunca la parte exacta de los pasos del plan

        plan = plan.splitlines() # Divide el string en un array donde cada posición es una línea

        list_plan = []
        # Eliminar espacios en blanco
        for act in plan:
            if (act.find(":") == -1) or (not act):
                plan.remove(act)
            index = act.find(":")
            act = act[:index].replace(" ", "") + act[index:]
            list_plan.append(act)
        
        # Elimina la última posición si está vacía
        if list_plan[-1] == ' ':
            list_plan.pop()

        # print(list_plan)
        # Recorremos cada acción y la parseamos para añadirla a las listas de problemas y objetivos
        for act in list_plan:
            if act.find("BASICACTION") != -1:
                start_index = act.find("(") + 15
                act2 = act[start_index:]
                end_index = act2.find(" ")
                problem_name = act2[:end_index]
                end_index2 = act2.find(")")
                objective_name = act2[end_index+1:end_index2]
            
                self.problem_list.append(problem_name.lower())
                self.objective_list.append(objective_name.lower())
        
        print(" ")
        print(self.problem_list)
        print(self.objective_list)
        print(" ")        

        return self.problem_list, self.objective_list


        

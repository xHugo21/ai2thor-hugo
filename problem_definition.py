# Fichero que contiene la clase que gestiona los parámetros de entrada para la ejecución del problema
#TODO comprobación errores

class ProblemDefinition():
    def __init__(self):
        '''Clase que contiene todos los métodos para recoger los parámetros de entrada'''
        pass

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

    def paths_selection(self, iteracion):
        '''Método que permite seleccionar las rutas de los archivos del planificador, problema y output'''
        #bucle = True
        #while bucle:
        #    print("----PLANIFICADOR----")
        #    print("[1] - cbp-roller")
        #    print("--------------------")
        #    aux = input("Seleccione un planificador: ")
        #    print("")
        #    if aux == '1':
        #        self.planner_path = "../cbp-roller/cbp-roller"
        #        bucle = False
        #    else:
        #        print("Por favor, introduce un planificador válido\n")

        #aux = input("Introduzca el nombre del problema a generar: ")
        #self.problem_path = "./pddl/problems/" + aux + "_iter" + str(iteracion) + ".pddl"
        #print("")

        #self.output_path = "./pddl/outputs/" + aux + "_iter" + str(iteracion) + ".txt"
        #print(f'El fichero de salida del planificador tendrá el nombre {self.output_path}\n')
        

        self.planner_path = "../cbp-roller/cbp-roller"
        self.problem_path = f'./pddl/problems/problem1_iter{iteracion}.pddl'
        self.output_path = f'./pddl/outputs/problem1_iter{iteracion}.txt'


        return self.planner_path, self.problem_path, self.output_path

    def problem_selection(self, event):
        '''Método que permite escoger el problema (acción) a realizar junto a su respectivo objetivo'''
        self.liquid = 'coffee'
        self.event = event
        bucle = True
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
            print("[16] - Put Object (Requires holding an object) NOT IMPLEMENTED")
            print("----------------")
            aux = input("Seleccione un tipo de problema a resolver: ")
            print("")

            if (1 <= int(aux) <= 16):
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
        
        #if str(aux) == '16':
        #    self.object_selection("put", "receptacle", True, True)

            
        return self.problem, self.objective, self.liquid

    def object_selection(self, problem_type, condition1, condition2, condition2_res, select_liquid=False):
        '''Método que encapsula la selección de objetivos de la mayoría de problemas'''
        self.problem = problem_type
        posible_objects = []
        print("----OBJETIVO----")
        for obj in self.event.metadata["objects"]:
            if (obj[condition1] == True) and (obj[condition2] == condition2_res):
                posible_objects.append(obj)
        i = 0
        for obj in posible_objects:
            print(f'[{i}] - {obj["objectId"]}')
            i += 1
        print("----------------")
        aux2 = input("Seleccione el objetivo: ")
        self.objective = posible_objects[int(aux2)]            
        print("")
        print(f'El objetivo seleccionado es: {self.objective["objectId"]}')

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

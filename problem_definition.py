# Fichero que contiene la clase que gestiona los parámetros de entrada para la ejecución del problema
#TODO comprobación errores

class ProblemDefinition():
    def __init__(self):
        pass

    def scene_selection(self):
        print("----ESCENAS----")
        print("[1-30] - Cocinas")
        print("[201-230] - Salas de estar")
        print("[301-330] - Dormitorios")
        print("[401-430] - Baños")
        print("---------------")
        self.scene_number = str(input("Seleccione una escena: "))
        print("")

        return self.scene_number

    def paths_selection(self):
        '''
        print("----PLANIFICADOR----")
        print("[1] - cbp-roller")
        print("--------------------")
        aux = input("Seleccione un planificador: ")
        if str(aux) == '1':
            self.planner_path = "../cbp-roller/cbp-roller"
        print("")

        aux = input("Introduzca el nombre del problema a generar: ")
        self.problem_path = "./pddl/problems/" + aux + ".pddl"
        print("")

        self.output_path = "./pddl/outputs/plan_" + aux + ".txt"
        print(f'El fichero de salida del planificador tendrá el nombre plan_{aux}.txt')
        print("")
        '''

        self.planner_path = "../cbp-roller/cbp-roller"
        self.problem_path = "./pddl/problems/problem1.pddl"
        self.output_path = "./pddl/outputs/plan_problem1.txt"


        return self.planner_path, self.problem_path, self.output_path

    def problem_selection(self, event):
        self.liquid = 'coffee'
        self.event = event
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
        print("----------------")
        aux = input("Seleccione un tipo de problema a resolver: ")
        print("")

        # Establecer parámetros en caso de que se seleccione problema de movimiento
        if str(aux) == '1':
            self.problem = "movimiento"
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

            
        return self.problem, self.objective, self.liquid

    def object_selection(self, problem_type, condition1, condition2, condition2_res, select_liquid=False):
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

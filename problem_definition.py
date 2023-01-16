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


        return self.planner_path, self.problem_path, self.output_path

    def problem_selection(self, event):
        print("----PROBLEMA----")
        print("[1] - movimiento")
        print("[2] - pickup")
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
            aux = input("Seleccione la posición objetivo: ")
            self.objective = positions[int(aux)]
            print("")
            print(f'La posición objetivo seleccionada es: {self.objective}')

        # Establecer parámetros en caso de que se seleccione problema de pickup
        if str(aux) == '2':
            self.problem = "pickup"
            pickupable_objects = []
            print("----OBJETIVO----")
            for obj in event.metadata["objects"]:
                if obj["pickupable"] == True:
                    pickupable_objects.append(obj)
            i = 0
            for obj in pickupable_objects:
                print(f'[{i}] - {obj["objectId"]}')
                i += 1
            print("----------------")
            aux = input("Seleccione el objetivo pickup: ")
            self.objective = pickupable_objects[int(aux)]
            print("")
            print(f'El objetivo seleccionado es: {self.objective["objectId"]} distance: {self.objective["distance"]} visible: {self.objective["visible"]}')
            
        return self.problem, self.objective

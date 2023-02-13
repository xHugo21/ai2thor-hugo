# Fichero para parsear los datos de un entorno de AI2THOR a un problema PDDL
# Imports

class ParserAI2THORPDDL:
    '''Clase que contiene todos los métodos necesarios para parsear una escena del simulador AI2THOR a un problema PDDL'''
    def __init__(self, event, problem_path, problem, objective, controller):
        # Se almacenan los parámetros
        self.event = event
        self.metadata = self.event.metadata
        self.problem_path = problem_path
        self.objective = objective
        self.controller = controller

        # Se parsean los datos generales comunes en todos los problemas
        self.parse_general(problem)

        # Dependiendo del tipo de problema, se añaden unos parámetros u otros
        if problem == "move":
            self.parse_move_problem()
        elif problem == "drop":
            self.parse_drop_problem()
        # Si el problema es de un tipo distinto a movimiento o drop, entonces se añaden las interactable poses del objetivo
        else:
            self.parse_interactable_poses()
        
        # Se añaden los estados meta del resto de problemas
        if problem == "pickup":
            self.parse_general_problem("holding")
        elif problem == "open":
            self.parse_general_problem("opened")
        elif problem == "close":
            self.parse_general_problem("closed")
        elif problem == "break":
            self.parse_general_problem("broken")
        elif problem == "cook":
            self.parse_general_problem("cooked")
        elif problem == "slice":
            self.parse_general_problem("sliced")
        elif problem == "toggleon":
            self.parse_general_problem("toggledon")
        elif problem == "toggleoff":
            self.parse_general_problem("toggledoff")
        elif problem == "dirty":
            self.parse_general_problem("dirty")
        elif problem == "clean":
            self.parse_general_problem("cleaned")
        elif problem == "fill":
            self.parse_general_problem("filled")
        elif problem == "empty":
            self.parse_general_problem("emptied")
        elif problem == "useup":
            self.parse_general_problem("usedup")
        elif problem == "put":
            self.parse_put_problem()
        
        # Una vez parseado, se escribe sobre un fichero .pddl
        self.write_parsed_problem()

    def parse_general(self, problem):
        '''Método que transforma los datos del entorno seleccionado de AI2THOR en un problema en lenguaje PDDL'''

        # Extracción de datos importantes del agente
        agent_location = self.metadata["agent"]["position"] # Posición inicial del agente
        positions = self.metadata["actionReturn"] # Posiciones permitidas en el entorno. Return de acción GetReachablePositions
        agent_facing = self.metadata["agent"]["rotation"]["y"] # Orientación del agente. Valores posibles: 0, 90, 180, 270. Dependiendo del valor se incrementa o decrementa una coordenada distinta al ejecutar MoveAhead
        agent_inclination = self.metadata["agent"]["cameraHorizon"] # Inclinación vertical de la cámara del agente

        # Extracción de datos importantes de los objetos del entorno
        self.objects = self.metadata["objects"] # Contiene todos los objetos del entorno

        # Escritura del problema en formato PDDL
        self.problem = "(define (problem problem1)\n"
        self.problem += f"\t(:domain domain_{problem})\n"

        # Definición de las posiciones y objetos
        self.problem += "\t(:objects\n"
        i = 0
        for pos in positions:
            self.problem += f"\t\tpos{i} - position\n"
            i += 1
        if self.objective.get('objectId') != None:
            for obj in self.objects:
                if (obj['objectId'] == self.objective['objectId']) or (obj['isPickedUp']): 
                    self.problem += f"\t\t{obj['name']} - object\n"
        self.problem += "\t)\n\n"

        # Definición del estado inicial del problema
        self.problem += "\t(:init\n"

        # Definición de predicados o funciones relacionados con el agente
        self.problem += f"\t\t(= (facing) {agent_facing})\n\n"
        self.problem += f"\t\t(= (inclination) {agent_inclination})\n\n"
        self.problem += f"\t\t(= (agent-at-x) {agent_location['x']})\n"
        self.problem += f"\t\t(= (agent-at-z) {agent_location['z']})\n\n"

        # Definición de predicados o funciones que determinan las posibles posiciones del agente en el entorno
        i = 0
        for pos in positions:
            self.problem += f"\t\t(= (posiblepos-x pos{i}) {pos['x']})\n"
            self.problem += f"\t\t(= (posiblepos-z pos{i}) {pos['z']})\n\n"
            i += 1

        # Definición de predicados o funciones relacionados con los objetos
        for obj in self.objects:
            if obj["isPickedUp"]:
                self.held_obj = obj
                self.problem += f"\t\t(holding {obj['name']})\n\n"
        
        self.problem += "\t)\n\n"

    def parse_move_problem(self):
        # Definición del estado meta del problema
        self.problem += "\t(:goal (and\n"
        self.problem += f"\t\t(= (agent-at-x) {self.objective['x']})"    
        self.problem += f"\t\t(= (agent-at-z) {self.objective['z']})"    
        self.problem += "\t))\n"
        self.problem += ")\n"

    def parse_interactable_poses(self):
        # Añadir interactable poses del objeto
        
        event2 = self.controller.step(action="GetInteractablePoses", objectId=self.objective["objectId"], standings=[True], horizons=[0])
        interactable_poses = event2.metadata["actionReturn"]
        if not interactable_poses:
            event2 = self.controller.step(action="GetInteractablePoses", objectId=self.objective["objectId"], standings=[True], horizons=[0, 30])
            interactable_poses = event2.metadata["actionReturn"]
            if not interactable_poses:
                event2 = self.controller.step(action="GetInteractablePoses", objectId=self.objective["objectId"], standings=[True], horizons=[0, 30, 60])
                interactable_poses = event2.metadata["actionReturn"]
                if not interactable_poses:
                    event2 = self.controller.step(action="GetInteractablePoses", objectId=self.objective["objectId"], standings=[True])
                    interactable_poses = event2.metadata["actionReturn"]
                    if not interactable_poses:
                        print("Error. El objeto no se encuentra accesible para el agente\n")
                        print("Reinicie el programa e intente con otra acción\n")
                        exit()

        subproblem = ""
        i = 0
        if interactable_poses:
            for pose in interactable_poses:
                subproblem += f"\t\t(= (interactablepose-x pose{i} {self.objective['name']}) {pose['x']})\n"
                subproblem += f"\t\t(= (interactablepose-z pose{i} {self.objective['name']}) {pose['z']})\n"
                subproblem += f"\t\t(= (interactablepose-facing pose{i} {self.objective['name']}) {pose['rotation']})\n"
                subproblem += f"\t\t(= (interactablepose-inclination pose{i} {self.objective['name']}) {pose['horizon']})\n\n"
                i += 1
        start_index = self.problem.find("(:init\n")
        end_index = self.problem.find("\t\t(= (facing)")
        self.problem = self.problem[:start_index+8] + subproblem + self.problem[end_index:]

        # Añadir las interactable poses anteriormente creadas como objetos
        subproblem = ""
        for j in range (0, i):
            subproblem += f"\t\tpose{j} - position\n"
        start_index = self.problem.find("(:objects\n")
        end_index = self.problem.find("\t\tpos0")
        self.problem = self.problem[:start_index+11] + subproblem + self.problem[end_index:]

    def parse_general_problem(self, act):
        # Definición del estado meta
        # Definición del estado meta del problema pickup
            self.problem += "\t(:goal (and\n"
            self.problem += f"\t\t({act} {self.objective['name']})"    
            self.problem += "\t))\n"
            self.problem += ")\n"
    
    #def parse_pickup_problem(self):
    #    # Definición del estado meta del problema pickup
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(holding {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
#
    #def parse_open_problem(self):
    #    # Definición del estado meta del problema open
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(open {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
    #
    #def parse_close_problem(self):
    #    # Definición del estado meta del problema close
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(closed {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
#
    #def parse_break_problem(self):
    #    # Definición del estado meta del problema break
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(broken {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
#
    #def parse_cook_problem(self):
    #    # Definición del estado meta del problema cook
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(cooked {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
    #
    #def parse_slice_problem(self):
    #    # Definición del estado meta del problema slice
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(sliced {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
#
    #def parse_toggleon_problem(self):
    #    # Definición del estado meta del problema toggleon
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(toggledon {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
#
    #def parse_toggleoff_problem(self):
    #    # Definición del estado meta del problema toggleoff
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(toggledoff {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
#
    #def parse_dirty_problem(self):
    #    # Definición del estado meta del problema dirty
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(dirty {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
#
    #def parse_clean_problem(self):
    #    # Definición del estado meta del problema clean
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(cleaned {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
    #
    #def parse_fill_problem(self):
    #    # Definición del estado meta del problema fill
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(filled {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
#
    #def parse_empty_problem(self):
    #    # Definición del estado meta del problema empty
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(emptied {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
#
    #def parse_useup_problem(self):
    #    # Definición del estado meta del problema useup
    #    self.problem += "\t(:goal (and\n"
    #    self.problem += f"\t\t(usedup {self.objective['name']})"    
    #    self.problem += "\t))\n"
    #    self.problem += ")\n"
    

    def parse_drop_problem(self):
        # Definición del estado meta del problema drop
        self.problem += "\t(:goal (and\n"
        self.problem += f"\t\t(not (holding {self.objective['name']}))\n"    
        self.problem += "\t))\n"
        self.problem += ")\n"

    def parse_put_problem(self):
        # Definición del estado meta del problema put
        self.problem += "\t(:goal (and\n"
        self.problem += f"\t\t(put {self.held_obj['name']} {self.objective['name']}))\n"    
        self.problem += "\t))\n"
        #self.problem += ")\n"

    def write_parsed_problem(self):
        # Escritura del problema sobre fichero PDDL
        with open(self.problem_path, 'w') as f:
            f.write(self.problem)



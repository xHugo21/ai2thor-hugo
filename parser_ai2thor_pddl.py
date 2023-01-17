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

        # Se empieza a parsear el problema
        self.parse_general()

        # Dependiendo del tipo de problema, se añaden unos parámetros u otros mediante sus respectivos métodos
        if problem == "movimiento":
            self.parse_movement_problem()
        elif problem == "pickup":
            self.parse_interactable_poses()
            self.parse_pickup_problem()
        elif problem == "open":
            self.parse_interactable_poses()
            self.parse_open_problem()
        elif problem == "close":
            self.parse_interactable_poses()
            self.parse_close_problem()
        
        # Una vez parseado, se escribe sobre un fichero .pddl
        self.write_parsed_problem()

    def parse_general(self):
        '''Método que transforma los datos del entorno seleccionado de AI2THOR en un problema en lenguaje PDDL'''

        # Extracción de datos importantes del agente
        agent_location = self.metadata["agent"]["position"] # Posición inicial del agente
        positions = self.metadata["actionReturn"] # Posiciones permitidas en el entorno. Return de acción GetReachablePositions
        agent_facing = self.metadata["agent"]["rotation"]["y"] # Orientación del agente. Valores posibles: 0, 90, 180, 270. Dependiendo del valor se incrementa o decrementa una coordenada distinta al ejecutar MoveAhead
        agent_inclination = self.metadata["agent"]["cameraHorizon"] # Inclinación vertical de la cámara del agente
        #is_standing = self.metadata["agent"]["isStanding"]

        # Extracción de datos importantes de los objetos del entorno
        self.objects = self.metadata["objects"] # Contiene todos los objetos del entorno
        
        # Escritura del problema en formato PDDL
        self.problem = "(define (problem problem1)\n"
        self.problem += "\t(:domain domain_ai2thor)\n"

        # Definición de los objetos
        self.problem += "\t(:objects\n"
        i = 0
        for pos in positions:
            self.problem += f"\t\tpos{i} - position\n"
            i += 1
        for obj in self.objects:
            self.problem += f"\t\t{obj['name']} - object\n"
        self.problem += "\t)\n\n"

        # Definición del estado inicial del problema
        self.problem += "\t(:init\n"

        # Definición de predicados o funciones relacionados con el agente
        self.problem += f"\t\t(= (facing) {agent_facing})\n\n"
        self.problem += f"\t\t(= (inclination) {agent_inclination})\n\n"
        self.problem += f"\t\t(= (agent-at-x) {agent_location['x']})\n"
        # self.problem += f"       (= (agent-at-y) {agent_location['y']})\n"
        self.problem += f"\t\t(= (agent-at-z) {agent_location['z']})\n\n"

        #if is_standing:
        #    self.problem += f"\t\t(is-standing)\n\n"

        # Definición de predicados o funciones que determinan las posibles posiciones del agente en el entorno
        i = 0
        for pos in positions:
            self.problem += f"\t\t(= (posiblepos-x pos{i}) {pos['x']})\n"
            # self.problem += f"       (= (posiblepos-y pos{i}) {pos['y']})\n"
            self.problem += f"\t\t(= (posiblepos-z pos{i}) {pos['z']})\n\n"
            i += 1

        # Definición de predicados o funciones relacionados con los objetos
        for obj in self.objects:
            object_location = obj["position"]
            self.problem += f"\t\t(= (object-at-x {obj['name']}) {object_location['x']:.25f})\n" #TODO mirar si se puede solucionar de otra manera la notacion científica
            self.problem += f"\t\t(= (object-at-y {obj['name']}) {object_location['y']:.25f})\n"
            self.problem += f"\t\t(= (object-at-z {obj['name']}) {object_location['z']:.25f})\n\n"
        
        self.problem += "\t)\n\n"

    def parse_movement_problem(self):
        # Definición del estado meta del problema
        self.problem += "\t(:goal (and\n"
        self.problem += f"\t\t(= (agent-at-x) {self.objective['x']})"    
        # self.problem += f"       (= (agent-at-y) {self.objective['y']})"    
        self.problem += f"\t\t(= (agent-at-z) {self.objective['z']})"    
        self.problem += "\t))\n"
        self.problem += ")\n"

    def parse_interactable_poses(self):
        # Añadir interactable poses del objeto
        event2 = self.controller.step(action="GetInteractablePoses", objectId=self.objective["objectId"], standings=[True])
        interactable_poses = event2.metadata["actionReturn"]
        subproblem = ""
        i = 0
        for pose in interactable_poses:
            subproblem += f"\t\t(= (interactablepose-x pose{i} {self.objective['name']}) {pose['x']})\n"
            subproblem += f"\t\t(= (interactablepose-z pose{i} {self.objective['name']}) {pose['z']})\n"
            subproblem += f"\t\t(= (interactablepose-facing pose{i} {self.objective['name']}) {pose['rotation']})\n"
            subproblem += f"\t\t(= (interactablepose-inclination pose{i} {self.objective['name']}) {pose['horizon']})\n\n"
            i += 1
        
        start_index = self.problem.find("(:init\n")
        end_index = self.problem.find("\t\t(= (facing)")
        self.problem = self.problem[:start_index+8] + subproblem + self.problem[end_index:]

        # Añadir las interactable poses creadas como objetos
        subproblem = ""
        for j in range (0, i):
            subproblem += f"\t\tpose{j} - position\n"
        
        start_index = self.problem.find("(:objects\n")
        end_index = self.problem.find("\t\tpos0")
        self.problem = self.problem[:start_index+11] + subproblem + self.problem[end_index:]
    
    def parse_pickup_problem(self):
        # Definición del estado meta del problema
        self.problem += "\t(:goal (and\n"
        self.problem += f"\t\t(holding {self.objective['name']})"    
        self.problem += "\t))\n"
        self.problem += ")\n"

    def parse_open_problem(self):
        # Definición del estado meta del problema
        self.problem += "\t(:goal (and\n"
        self.problem += f"\t\t(open {self.objective['name']})"    
        self.problem += "\t))\n"
        self.problem += ")\n"
    
    def parse_close_problem(self):
        # Definición del estado meta del problema
        self.problem += "\t(:goal (and\n"
        self.problem += f"\t\t(closed {self.objective['name']})"    
        self.problem += "\t))\n"
        self.problem += ")\n"


    def write_parsed_problem(self):
        # Escritura del problema sobre fichero PDDL
        with open(self.problem_path, 'w') as f:
            f.write(self.problem)



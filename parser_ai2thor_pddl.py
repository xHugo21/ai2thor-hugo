# Fichero para parsear los datos de un entorno de AI2THOR a un problema PDDL
# Imports

class ParserAI2THORPDDL:
    def __init__(self, event, problem_path, problem, objective):
        self.event = event
        self.metadata = self.event.metadata
        self.problem_path = problem_path

        self.parse_general()

        if problem == "movimiento":
            self.parse_movement_problem(objective)
        elif problem == "pickup":
            pass
        
        self.write_parsed_problem()

    def parse_general(self):
        '''Función que transforma los datos del entorno seleccionado de AI2THOR en un problema en lenguaje PDDL'''

        # Extracción de datos importantes del agente
        agent_location = self.metadata["agent"]["position"] # Posición inicial del agente
        positions = self.metadata["actionReturn"] # Posiciones permitidas en el entorno. Return de acción GetReachablePositions
        agent_facing = self.metadata["agent"]["rotation"]["y"] # Orientación del agente. Valores posibles: 0, 90, 180, 270. Dependiendo del valor se incrementa o decrementa una coordenada distinta al ejecutar MoveAhead
        agent_inclination = self.metadata["agent"]["cameraHorizon"] # Inclinación vertical de la cámara del agente

        # Extracción de datos importantes de los objetos del entorno
        objects = self.metadata["objects"] # Contiene todos los objetos del entorno

        # Búsqueda del objeto "goal" u objetivo
        '''
        goal_object = objects[0]
        for obj in objects:
            if obj['objectId'] == 'Apple|-01.65|+00.81|+00.07':
                goal_object = obj # Se almacena en la variable goal_object
        '''
        
        # Escritura del problema en formato PDDL
        self.problem = "(define (problem problem1)\n"
        self.problem += "    (:domain domain_ai2thor)\n"

        # Definición de los objetos
        self.problem += "    (:objects\n"
        for obj in objects:
            self.problem += f"       {obj['name']} - object\n"
        i = 0
        for pos in positions:
            self.problem += f"       pos{i} - position\n"
            i += 1
        self.problem += "    )\n\n"

        # Definición del estado inicial del problema
        self.problem += "    (:init\n"

        # Definición de predicados o funciones relacionados con el agente
        '''
        problem += f"       (= (distance {goal_object['objectId']}) {goal_object['distance']})\n\n"
        '''
        self.problem += f"       (= (facing) {agent_facing})\n\n"
        self.problem += f"       (= (inclination) {agent_inclination})\n\n"
        self.problem += f"       (= (agent-at-x) {agent_location['x']})\n"
        self.problem += f"       (= (agent-at-y) {agent_location['y']})\n"
        self.problem += f"       (= (agent-at-z) {agent_location['z']})\n\n"

        # Definición de predicados o funciones que determinan las posibles posiciones del agente en el entorno
        i = 0
        for pos in positions:
            self.problem += f"       (= (posiblepos-x pos{i}) {pos['x']})\n"
            # self.problem += f"       (= (posiblepos-y pos{i}) {pos['y']})\n"
            self.problem += f"       (= (posiblepos-z pos{i}) {pos['z']})\n\n"
            i += 1

        # Definición de predicados o funciones relacionados con los objetos
        for obj in objects:
            object_location = obj["position"]
            self.problem += f"       (= (object-at-x {obj['name']}) {object_location['x']:.25f})\n" #TODO mirar si se puede solucionar de otra manera la notacion científica
            self.problem += f"       (= (object-at-y {obj['name']}) {object_location['y']:.25f})\n"
            self.problem += f"       (= (object-at-z {obj['name']}) {object_location['z']:.25f})\n\n"
        
        self.problem += "    )\n\n"

    def parse_movement_problem(self, objective):
        # Definición del estado meta del problema
        self.problem += "    (:goal (and\n"
        self.problem += f"       (= (agent-at-x) {objective['x']})"    
        self.problem += f"       (= (agent-at-y) {objective['y']})"    
        self.problem += f"       (= (agent-at-z) {objective['z']})"    
        self.problem += "    ))\n"
        self.problem += ")\n"


    def write_parsed_problem(self):
        # Escritura del problema sobre fichero PDDL
        with open(self.problem_path, 'w') as f:
            f.write(self.problem)



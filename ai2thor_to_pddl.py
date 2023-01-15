# Fichero para parsear los datos de un entorno de AI2THOR a un problema PDDL
# Imports
import numpy as np

def parse_ai2thor_pddl(event, problem_path):
    '''Función que transforma los datos del entorno seleccionado de AI2THOR en un problema en lenguaje PDDL'''

    # Extracción de datos importantes del agente
    agent_location = event.metadata["agent"]["position"] # Posición inicial del agente
    positions = event.metadata["actionReturn"] # Posiciones permitidas en el entorno. Return de acción GetReachablePositions
    agent_facing = event.metadata["agent"]["rotation"]["y"] # Orientación del agente. Valores posibles: 0, 90, 180, 270. Dependiendo del valor se incrementa o decrementa una coordenada distinta al ejecutar MoveAhead
    agent_inclination = event.metadata["agent"]["cameraHorizon"] # Inclinación vertical de la cámara del agente

    # Extracción de datos importantes de los objetos del entorno
    objects = event.metadata["objects"] # Contiene todos los objetos del entorno

    # Búsqueda del objeto "goal" u objetivo
    '''
    goal_object = objects[0]
    for obj in objects:
        if obj['objectId'] == 'Apple|-01.65|+00.81|+00.07':
            goal_object = obj # Se almacena en la variable goal_object
    '''
    
    # Escritura del problema en formato PDDL
    problem = "(define (problem problem1)\n"
    problem += "    (:domain domain_ai2thor)\n"

    # Definición de los objetos
    problem += "    (:objects\n"
    for obj in objects:
        problem += f"       {obj['name']} - object\n"
    i = 0
    for pos in positions:
        problem += f"       pos{i} - position\n"
        i += 1
    problem += "    )\n\n"

    # Definición del estado inicial del problema
    problem += "    (:init\n"

    # Definición de predicados o funciones relacionados con el agente
    '''
    problem += f"       (= (distance {goal_object['objectId']}) {goal_object['distance']})\n\n"
    '''
    problem += f"       (= (facing) {agent_facing})\n\n"
    problem += f"       (= (inclination) {agent_inclination})\n\n"
    problem += f"       (= (agent-at-x) {agent_location['x']})\n"
    problem += f"       (= (agent-at-y) {agent_location['y']})\n"
    problem += f"       (= (agent-at-z) {agent_location['z']})\n\n"

    # Definición de predicados o funciones que determinan las posibles posiciones del agente en el entorno
    i = 0
    for pos in positions:
        problem += f"       (= (posiblepos-x pos{i}) {pos['x']})\n"
        # problem += f"       (= (posiblepos-y pos{i}) {pos['y']})\n"
        problem += f"       (= (posiblepos-z pos{i}) {pos['z']})\n\n"
        i += 1

    # Definición de predicados o funciones relacionados con los objetos
    for obj in objects:
        object_location = obj["position"]
        problem += f"       (= (object-at-x {obj['name']}) {object_location['x']:.25f})\n" #TODO mirar si se puede solucionar de otra manera la notacion científica
        problem += f"       (= (object-at-y {obj['name']}) {object_location['y']:.25f})\n"
        problem += f"       (= (object-at-z {obj['name']}) {object_location['z']:.25f})\n\n"
    
    problem += "    )\n\n"

    # Definición del estado meta del problema
    problem += "    (:goal (and\n"
    # problem += f"        (holding {goal_object['objectId']})\n"
    problem += f"       (= (agent-at-x) -0.75)"    
    problem += "    ))\n"
    problem += ")\n"

    # Escritura del problema sobre fichero PDDL
    with open(problem_path, 'w') as f:
        f.write(problem)



# Fichero principal para la ejecución de pruebas
# Imports
from ai2thor.controller import Controller
import time
from problem_definition import ProblemDefinition
from parser_ai2thor_pddl import ParserAI2THORPDDL
from parser_pddl_ai2thor import ParserPDDLAI2THOR
from planificador import Planificador
from aux import printAgentStatus, printLastActionStatus, removeGeneratedFolders, createCamera, printObjectStatus

# Constantes
domain_path = "./pddl/domain_ai2thor.pddl"

# Limpiamos los directorios de problemas
removeGeneratedFolders()

# Pedimos al usuario que escoja una escena
inputs = ProblemDefinition()
scene_number = inputs.scene_selection()

# Inicialización del entorno
print("*INICIANDO ENTORNO*\n")
controller = Controller(agentMode="default",
                        visibilityDistance=1.5,
                        scene="FloorPlan" + str(scene_number),

                        # Tamaño de pasos
                        gridSize=0.25,
                        snapToGrid=True,
                        rotateStepDegrees=90,

                        # Modos de imagen
                        renderDepthImage=False,
                        renderInstanceSegmentation=False,

                        # Opciones de cámara
                        #width=720,
                        #height=405,
                        width=300,
                        height=300,
                        fieldOfView=90)
print("*ENTORNO INICIALIZADO SATISFACTORIAMENTE*\n")

# Creamos una cámara que extrae una foto general de la escena en ./images/scene.png
createCamera(controller)

iteracion = 0
bucle = 'Y'
while bucle == 'Y':
    # Pedimos al usuario que escoja el planificador, y el nombre de los ficheros que se van a generar
    planner_path, problem_path, output_path = inputs.paths_selection()

    # Ejecutamos una acción sobre el agente. En este caso la acción GetReachablePositions para que contenga la información de las posiciones que puede tomar el agente en el entorno.
    event = controller.step(action="GetReachablePositions")

    # Pedimos al usuario que indique el tipo de problema a resolver y el objetivo concreto dentro de ese problema
    problem, objective, liquid = inputs.problem_selection(event)

    '''
    count1 = 0
    for obj in event.metadata["actionReturn"]:
    posible_pos = event.metadata["actionReturn"]

    event = controller.step(action="GetInteractablePoses", objectId=objective["objectId"], standings=[True])
    printLastActionStatus(event)
    count = 0
    interactable_pos = event.metadata["actionReturn"]
    aux = False
    for i in range(0, len(interactable_pos)):
        for j in range (0, len(posible_pos)):
            if (interactable_pos[i]['x'] == posible_pos[j]['x']) and (interactable_pos[i]['z'] == posible_pos[j]['z']):
                aux = True
        print(f'{i} - {aux}')
        aux = False
    print(event.metadata["actionReturn"])
    '''

    # Parseo del entorno a fichero pddl
    ParserAI2THORPDDL(event, problem_path, problem, objective, controller)

    # Ejecución del planificador sobre el dominio y el problema dados. Si último parámetro = True -> se imprime plan por pantalla.
    plan = Planificador(planner_path, domain_path, problem_path, output_path, 0,  True)

    # Visualizar estado inicial
    # printAgentStatus(controller.last_event)

    # Parseo del plan para convertirlo en acciones ejecutables por el agente y ejecutarlas
    parsed = ParserPDDLAI2THOR(plan.get_plan(), controller, iteracion, liquid)

    '''
    for act in parsed.executable_actions:
        exec(act)
    '''

    # Visualizar estado final
    # printAgentStatus(controller.last_event)
    printLastActionStatus(controller.last_event)
    if problem == 'movimiento':
        printAgentStatus(controller.last_event)
    else:
        printObjectStatus(controller.last_event, objective)

    bucle = input('Desea realizar otra acción sobre este entorno [Y/n]: ')
    iteracion += 1
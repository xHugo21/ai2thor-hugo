# Fichero principal para la ejecución de pruebas
# Imports
from ai2thor.controller import Controller
import time
from problem_definition import ProblemDefinition
from parser_ai2thor_pddl import ParserAI2THORPDDL
from parser_pddl_ai2thor import ParserPDDLAI2THOR
from planificador import Planificador
from aux import printAgentStatus, printLastActionStatus, printObjectStatus

# Constantes
domain_path = "./pddl/domain_ai2thor.pddl"

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
                        width=300,
                        height=300,
                        fieldOfView=90)
print("*ENTORNO INICIALIZADO SATISFACTORIAMENTE*\n")

# Pedimos al usuario que escoja el planificador, y el nombre de los ficheros que se van a generar
planner_path, problem_path, output_path = inputs.paths_selection()

# Ejecutamos una acción sobre el agente. En este caso la acción GetReachablePositions para que contenga la información de las posiciones que puede tomar el agente en el entorno.
event = controller.step(action="GetReachablePositions")

# Pedimos al usuario que indique el tipo de problema a resolver y el objetivo concreto dentro de ese problema
problem, objective = inputs.problem_selection(event)

# Parseo del entorno a fichero pddl
ParserAI2THORPDDL(event, problem_path, problem, objective)

# Ejecución del planificador sobre el dominio y el problema dados. Si último parámetro = True -> se imprime plan por pantalla.
plan = Planificador(planner_path, domain_path, problem_path, output_path, True)

# Visualizar estado inicial
printAgentStatus(controller.last_event)

# Parseo del plan para convertirlo en acciones ejecutables por el agente y ejecutarlas
ParserPDDLAI2THOR(plan.get_plan(), controller)

# Visualizar estado final
printAgentStatus(controller.last_event)
# Fichero principal para la ejecución de pruebas
# Imports
from ai2thor.controller import Controller
import time
from parser_ai2thor_pddl import ParserAI2THORPDDL
from parser_pddl_ai2thor import ParserPDDLAI2THOR
from run_plan import run_plan, print_plan, save_plan
from aux import printAgentStatus, printLastActionStatus, printObjectStatus, printEveryPosibleLocation

# Variables globales
scene_number = 2 # Determina el escenario. Valores permitidos: [1, 30] -> cocinas, [201, 230] -> salas de estar, [301, 330] -> dormitorios, [401, 430] -> baños
planner_path = "../cbp-roller/cbp-roller"
domain_path = "./pddl/domain_ai2thor.pddl"
problem_path = "./pddl/problem1.pddl"
output_path = "./pddl/plan_problem1.txt"
problem = "movement"
objective = {'x': -2.75, 'y': 0.9009992480278015, 'z': 3.25}


# Inicialización del entorno
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
                        width=600,
                        height=600,
                        fieldOfView=90)

'''
event = controller.step("Done")
printAgentStatus(event)
event = controller.step("RotateLeft")
printAgentStatus(event)
event = controller.step("MoveAhead")
printAgentStatus(event)
time.sleep(1)
'''

# printEveryPosibleLocation(controller)


# Ejecutamos una acción sobre el agente. En este caso la acción GetReachablePositions para que contenga la información de las posiciones que puede tomar el agente en el entorno.
event = controller.step(action="GetReachablePositions")

# Parseo del entorno a fichero pddl
ParserAI2THORPDDL(event, problem_path, problem, objective)

# Ejecución del planificador sobre el dominio y el problema dados
run_plan(planner_path, domain_path, problem_path, output_path)

# Printeo del plan por pantalla
print_plan(output_path)

# Parseo del plan para convertirlo en acciones ejecutables por el agente
ParserPDDLAI2THOR(save_plan(output_path), controller)
# Ejecución de las acciones

# Visualizar estado final
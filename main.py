# Fichero principal para la ejecución de pruebas
# Imports
from ai2thor.controller import Controller
import time
from ai2thor_to_pddl import parse_ai2thor_pddl
from run_plan import run_plan, print_plan
from aux import printAgentStatus, printLastActionStatus, printObjectStatus

# Variables globales
scene_number = 2 # Determina el escenario. Valores permitidos: [1, 30] -> cocinas, [201, 230] -> salas de estar, [301, 330] -> dormitorios, [401, 430] -> baños
planner_path = "../cbp-roller/cbp-roller"
domain_path = "./pddl/domain_ai2thor.pddl"
problem_path = "./pddl/problem1.pddl"
output_path = "./pddl/plan_problem1.txt"

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

# Ejecutamos una acción sobre el agente. En este caso la acción GetReachablePositions para que contenga la información de las posiciones que puede tomar el agente en el entorno.
event = controller.step(action="GetReachablePositions")

# Parseo del entorno a fichero pddl
parse_ai2thor_pddl(event, problem_path)

# Ejecución del planificador sobre el dominio y el problema dados
run_plan(planner_path, domain_path, problem_path, output_path)

# Printeo del plan por pantalla
print_plan(output_path)

# Parseo del plan para convertirlo en acciones ejecutables por el agente

# Ejecución de las acciones

# Visualizar estado final

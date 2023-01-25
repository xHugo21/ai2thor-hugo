import ogamus
import json
from ai2thor.controller import Controller
import time
from problem_definition import ProblemDefinition
from parser_ai2thor_pddl import ParserAI2THORPDDL
from parser_pddl_ai2thor import ParserPDDLAI2THOR
from planificador import Planificador
from aux import printAgentStatus, printLastActionStatus, removeGeneratedFolders, createCamera, printObjectStatus, removeResults

dataset = 'Datasets/test_set_ogn_ithor.json'
planner_path = './OGAMUS/Plan/PDDL/Planners/FF/ff'
output_path = f'./pddl/outputs/problem1.txt'

# Eliminamos resultados generados previos
removeResults()

# Pedimos al usuario que seleccione una escena
inputs = ProblemDefinition()
scene_number = inputs.scene_selection()

# Indicamos el problema que se va a generar
problem_path = f'./OGAMUS/Plan/PDDL/facts.pddl'

# Iniciamos la escena para conseguir sus datos necesarios
controller = Controller(scene="FloorPlan" + scene_number)

event = controller.step("Pass")

agent_pos = event.metadata["agent"]["position"]
agent_rot = event.metadata["agent"]["rotation"]
agent_hor = event.metadata["agent"]["cameraHorizon"]

# Pedimos al usuario que seleccione una acción y un objetivo
problem = "pickup"
objective = inputs.object_selection_ogamus()

controller.stop()


# Creamos el diccionario a introducir en el json
dictionary = [{
    "episode": 1,
    "scene": "FloorPlan" + scene_number,
    "goal": "(exists (?o1 - " + objective + ") (and (viewing ?o1) (close_to ?o1)))",
    "agent_position": agent_pos,
    "agent_rotation": agent_rot,
    "initial_orientation": agent_rot['y'],
    "initial_horizon": agent_hor,
    "agent_is_standing": True,
    "agent_in_high_friction_area": False,
    "agent_fov": 79,
    "shortest_path": [
      {
        "x": -1.0,
        "y": 0.901863694190979,
        "z": 2.0
      }
    ],
    "shortest_path_length": 0
}]

# Modificamos el json test_set_ogn_ithor.json con los datos de la escena y el objetivo
json_object = json.dumps(dictionary, indent=4)

with open(dataset, "w") as f:
  f.write(json_object)


# Llamamos a ogamus.main() para analizar la escena y encontrar el objetivo
ogamus.main()

with open(problem_path, "r") as f:
  problem_str = f.read()

index_objective = problem_str.find(objective+"_")
final_objective = problem_str[index_objective:index_objective+len(objective)+2]

# Modificamos el problema devuelto para cambiar su estado meta
goal_state = f'(holding {final_objective})'



start_index = problem_str.find('(:goal') + 12
end_index = problem_str.find('(close_to ?o1)') + 16

problem_str = problem_str[:start_index] + goal_state + problem_str[end_index:]

with open(problem_path, "w") as f:
  f.write(problem_str)


# Llamamos al planificador para que ejecute el problema modificado sobre el dominio de ejecución de acciones
plan = Planificador(planner_path, problem_path, output_path, problem, 0,  print=True, ogamus=True)

# Parseamos el plan para convertirlo en acciones ejecutables por el agente
# parsed = ParserPDDLAI2THOR(plan.get_plan(), controller, iteracion=0, liquid='coffee')

# Visualizamos estado final




# IMPORTS
import json
import ogamus
import shutil
import numpy as np
from ai2thor.controller import Controller
from problem_definition import ProblemDefinition
from parser_ai2thor_pddl import ParserAI2THORPDDL
from parser_pddl_ai2thor import ParserPDDLAI2THOR
from goal_ogamus import GoalOgamus
from planificador import Planificador
from exec_ogamus import ExecOgamus
from aux import printAgentStatus, printLastActionStatus, createCamera, printObjectStatus, removeResultFolders

# CONSTANTES
DATASET = 'Datasets/test_set_ogn_ithor.json'
LOG = "Results/test_set_ogn_ithor_steps200/episode_0/log.txt"

# FLUJO DEL PROGRAMA PRINCIPAL

# Limpiamos los directorios de resultados generados en las ejecuciones
removeResultFolders()

# El usuario escoge el tipo de ejecución que desea: METADATA o OGAMUS
inputs = ProblemDefinition()
method = inputs.method_selection()

# Pedimos al usuario que seleccione una escena
scene_number = inputs.scene_selection()

# Si method = '1' -> METADATA
if method == '1':

  # Inicialización del entorno inicial (Si método = OGAMUS -> sirve para captar posicion inicial del agente)
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
  
  # Bucle que permite realizar más de una acción sobre la escena iniciada
  iteracion = 0
  bucle = 'Y'
  while bucle == 'Y':
      # Pedimos al usuario que escoja el planificador, y el nombre de los ficheros que se van a generar
      planner_path, problem_path, output_path = inputs.paths_selection(method, iteracion)

      # Ejecutamos una acción sobre el agente. En este caso la acción GetReachablePositions para que contenga la información de las posiciones que puede tomar el agente en el entorno.
      event = controller.step(action="GetReachablePositions")

      # Pedimos al usuario que indique el tipo de problema a resolver y el objetivo concreto dentro de ese problema
      problem, objective, liquid = inputs.problem_selection(event)

      # Parseo del entorno a fichero pddl
      ParserAI2THORPDDL(event, problem_path, problem, objective, controller)

      # Ejecución del planificador sobre el dominio y el problema dados. Si último parámetro = True -> se imprime plan por pantalla.
      plan = Planificador(planner_path, problem_path, output_path, problem, print=True, ogamus=False)

      # Parseo del plan para convertirlo en acciones ejecutables por el agente y ejecutarlas
      parsed = ParserPDDLAI2THOR(plan.get_plan(), controller, iteracion, liquid)

      # Visualizar estado final
      printLastActionStatus(controller.last_event)
      if problem == 'move':
          printAgentStatus(controller.last_event)
      else:
          printObjectStatus(controller.last_event, objective)

      # Preguntar si se desea ejecutar otra acción en este mismo entorno. En caso contrario se finaliza el programa
      bucle = input('Desea realizar otra acción sobre este entorno [Y/n]: ')
      iteracion += 1


# Si method = '2' -> OGAMUS
else:

  hfov = 79 / 360. * 2. * np.pi
  vfov = 2. * np.arctan(np.tan(hfov / 2) * 224 / 224)
  vfov = np.rad2deg(vfov)

  controller = Controller(renderDepthImage=1,
                          renderObjectImage=True,
                          visibilityDistance=1,
                          gridSize=0.25,
                          rotateStepDegrees=45,
                          scene="FloorPlan" + str(scene_number),
                          continuousMode=True,
                          snapToGrid=False,
                          # camera properties
                          width=224,
                          height=224,
                          fieldOfView=vfov,
                          agentMode='default'
                          )

  # Creamos una cámara que extrae una foto general de la escena en ./images/scene.png
  createCamera(controller)

  # Obtenemos los datos necesarios de la escena para pasar a OGAMUS y detenemos la ejecución del controlador
  event = controller.step("Pass")
  agent_pos = event.metadata["agent"]["position"]
  agent_rot = event.metadata["agent"]["rotation"]
  agent_hor = event.metadata["agent"]["cameraHorizon"]

  # Pedimos al usuario que seleccione los problemas que quiere resolver
  problem_list, objective_list = inputs.problem_selection_ogamus()

  # Bucle que se ejecuta tantas veces como problemas hayamos introducido para resolver
  iteracion = 0
  for problem in problem_list:
    # Establecemos el planificador y las rutas de los problemas y ficheros de salida
    planner_path, problem_path, output_path = inputs.paths_selection(method, iteracion)

    # Creamos el diccionario a introducir en el json
    dictionary = [{
        "episode": 1,
        "scene": "FloorPlan" + scene_number,
        "goal": "(exists (?o1 - " + objective_list[iteracion] + ") (and (viewing ?o1) (close_to ?o1)))",
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

    with open(DATASET, "w") as f:
      f.write(json_object)

    # Llamamos a ogamus.main() para analizar la escena y encontrar el objetivo
    controller = ogamus.main(controller)

    # Comprobamos el log de la ejecución de OGAMUS para ver si ha encontrado el objetivo.
    # Si ha llegado a la iteración 200 significa que no lo ha encontrado
    with open(LOG, "r") as f:
      log_str = f.read()
      if log_str.find("200:Stop") != -1:
        print("No se ha encontrado el objetivo indicado tras recorrer la escena durante 200 pasos\n")
        print("Ejecute de nuevo el programa y pruebe con un objetivo distinto\n")
        exit()

    execute = ExecOgamus(controller, problem, objective_list[iteracion], iteracion)  

    # Si ha encontrado el objetivo se ejecuta el problema concreto indicado al inicio
    # Modificamos el fichero "./OGAMUS/Plan/PDDL/facts.pddl" para cambiar su estado meta dependiendo del tipo de problema
    # GoalOgamus(problem_path, problem, objective_list[iteracion])

    # Copiamos el archivo al directorio de problemas de pddl para dejarlo guardado si hay más iteraciones
    shutil.copyfile("OGAMUS/Plan/PDDL/facts.pddl", f"pddl/problems/problem{iteracion}.pddl")

    # Llamamos al planificador para que ejecute el problema modificado sobre el dominio
    # plan = Planificador(planner_path, f"pddl/problems/problem{iteracion}.pddl", output_path, problem, print=True, ogamus=True)

    # Llamamos al parser para ejecutar la accion pedida sobre el objetivo
    # parsed = ParserPDDLAI2THOR(plan.get_plan(), controller, iteracion, liquid='coffee', ogamus=True)

    # Actualizamos posicion agente para inicializar la siguiente iteración desde la posición anterior
    event = controller.step("Pass")
    agent_pos = event.metadata["agent"]["position"]
    agent_rot = event.metadata["agent"]["rotation"]
    agent_hor = event.metadata["agent"]["cameraHorizon"]

    # Contamos la iteración realizada
    iteracion += 1

    



# Fichero que implementa funciones auxiliares
# Imports
from PIL import Image as im
import os
import shutil

def printAgentStatus(event):
    '''Muestra posición del agente'''
    print("-----------------------------------------------")
    print(f'sceneName: {event.metadata["sceneName"]}')
    print(f'lastAction: {event.metadata["lastAction"]}')
    print(f'agent cameraHorizon: {event.metadata["agent"]["cameraHorizon"]}')
    print(f'agent isStanding: {event.metadata["agent"]["isStanding"]}')
    print(f'agent position: {event.metadata["agent"]["position"]}')
    print(f'agent rotation: {event.metadata["agent"]["rotation"]}')
    print("-----------------------------------------------\n")

def printObjectStatus(event):
    '''Muestra objetos actuales y parámetros importantes de ellos'''
    print("-----------------------------------------------")
    for obj in event.metadata["objects"]:
        print(f'objectId: {obj["objectId"]} | distance: {obj["distance"]} | visible: {obj["visible"]} | position: {obj["position"]} | receptacle: {obj["receptacle"]} | parentReceptacles: {obj["parentReceptacles"]}')
    print("-----------------------------------------------\n")

def printLastActionStatus(event):
    '''Muestra información acerca de la ejecución de la última acción'''
    print("-----------------------------------------------")
    print(f'lastAction: {event.metadata["lastAction"]}')
    print(f'lastActionSuccess: {event.metadata["lastActionSuccess"]}')
    if event.metadata["errorMessage"]:
        print(f'Error: {event.metadata["errorMessage"]}')
    print("-----------------------------------------------\n")

def extractActionImage(event, name):
    '''Devuelve una imagen de la situación indicada con el nombre pasado por parámetro'''
    data = im.fromarray(event.frame)
    data.save("./images/" + name + ".png")

def createCamera(controller):
    event = controller.step("Done")
    center = event.metadata["sceneBounds"]["center"]
    center["y"] = event.metadata["sceneBounds"]["cornerPoints"][0][1]
    camera_loc = center

    event = controller.step(
        action="AddThirdPartyCamera",
        position=camera_loc,
        rotation=dict(x=90, y=0, z=0),
        fieldOfView=110
    )

    extractCameraImage(event.third_party_camera_frames[0], 'scene')

def extractCameraImage(nparray, name):
    '''Devuelve una imagen de la situación indicada con el nombre pasado por parámetro'''
    data = im.fromarray(nparray)
    data.save("./images/" + name + ".png")

def removeGeneratedFolders():
    '''Limpia los directorios de problemas, salidas del planificador e imágenes'''
    problems_dir = "./pddl/problems/"
    outputs_dir = "./pddl/outputs/"
    images_dir = "./images/"
    shutil.rmtree(problems_dir)
    shutil.rmtree(outputs_dir)
    shutil.rmtree(images_dir)
    os.mkdir(problems_dir)
    os.mkdir(outputs_dir)
    os.mkdir(images_dir)


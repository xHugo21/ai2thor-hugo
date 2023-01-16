# Fichero que implementa funciones auxiliares
# Imports

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

'''
def printEveryPosibleLocation(controller):
    i = 0
    event = controller.step("GetReachablePositions")
    positions = event.metadata["actionReturn"]
    print("-----------------------------------------------")
    for pos in positions:
        print(f'pos{i}: {pos}')
        i += 1
    print("-----------------------------------------------\n")
'''
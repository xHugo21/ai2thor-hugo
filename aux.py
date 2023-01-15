# Fichero que implementa funciones auxiliares

def printAgentStatus(event):
    '''Muestra posición del agente'''
    print("-----------------------------------------------\n")
    print(f'sceneName: {event.metadata["sceneName"]}\n')
    print(f'lastAction: {event.metadata["lastAction"]}\n')
    print(f'agent cameraHorizon: {event.metadata["agent"]["cameraHorizon"]}\n')
    print(f'agent isStanding: {event.metadata["agent"]["isStanding"]}\n')
    print(f'agent position: {event.metadata["agent"]["position"]}\n')
    print(f'agent rotation: {event.metadata["agent"]["rotation"]}\n')
    print("-----------------------------------------------\n")

def printObjectStatus(event):
    '''Muestra objetos actuales y parámetros importantes de ellos'''
    print("-----------------------------------------------\n")
    for obj in event.metadata["objects"]:
        print(f'objectId: {obj["objectId"]} | distance: {obj["distance"]} | visible: {obj["visible"]} | position: {obj["position"]} | receptacle: {obj["receptacle"]} | parentReceptacles: {obj["parentReceptacles"]}\n')
    print("-----------------------------------------------\n")

def printLastActionStatus(event):
    '''Muestra información acerca de la ejecución de la última acción'''
    print("-----------------------------------------------\n")
    print(f'lastAction: {event.metadata["lastAction"]}\n')
    print(f'lastActionSuccess: {event.metadata["lastActionSuccess"]}\n')
    if event.metadata["errorMessage"]:
        print(f'Error: {event.metadata["errorMessage"]}\n')
    print("-----------------------------------------------\n")

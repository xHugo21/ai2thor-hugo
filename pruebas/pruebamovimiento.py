from ai2thor.controller import Controller
import time

controller = Controller(agentMode="default",
                        visibilityDistance=1.5,
                        scene="FloorPlan1",

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

event = controller.step("RotateLeft")
event = controller.step ("MoveAhead")
event = controller.step ("MoveAhead")
event = controller.step ("MoveAhead")
event = controller.step ("MoveAhead")
event = controller.step ("MoveAhead")
event = controller.step ("MoveAhead")
event = controller.step ("MoveAhead")
time.sleep(2)

print("Agent pos:")
print(event.metadata["agent"]["position"])

print("\nObjects:")
for obj in event.metadata["objects"]:
    #print(obj["objectId"])
    if obj["objectId"] == "Toaster|-01.84|+00.90|+00.13":
        goal_object = obj

print("\nIs visible:")
print(obj["visible"])

print("\nLast action success:")
print(event.metadata["lastActionSuccess"])


#print("Agent pos:")
#print(controller.last_event.metadata)
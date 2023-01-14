from ai2thor.controller import Controller
from PIL import Image as im
import time

controller = Controller()
event = controller.step("RotateLeft")

print(controller.last_event.frame)

data = im.fromarray(controller.last_event.frame)

data.save('imagen2.png')

# Fichero para parsear los datos de un plan en acciones ejecutables por AI2THOR
from ai2thor.controller import Controller
from aux import printAgentStatus


class ParserPDDLAI2THOR:
    def __init__(self, raw_plan, controller):
        self.actions = []
        self.executable_actions = []
        self.controller = controller

        self.extract_plan(raw_plan)

        self.parse_actions()
    
    def extract_plan(self, raw_plan):
        '''Método que extrae el plan dividido para poder procesar acción por acción.
        Guarda en self.actions una lista de todas las acciones que deben ser parseadas'''
        start_index = raw_plan.find("0:")
        end_index = raw_plan.find("time")

        plan = raw_plan[start_index:end_index] # Trunca la parte exacta de los pasos del plan

        plan = plan.splitlines() # Divide el string en un array donde cada posición es una línea

        # Eliminar espacios en blanco
        for act in plan:
            if (act.find(":") == -1) or (not act):
                plan.remove(act)
            index = act.find(":")
            act = act[:index].replace(" ", "") + act[index:]
            self.actions.append(act)

        # Eliminar posición extra (vacía)
        self.actions.pop()

    def parse_actions(self):
        for act in self.actions:
            if act.find("ROTATE-LEFT") != -1:
                self.rotate_left()
            elif act.find("ROTATE-RIGHT") != -1:
                self.rotate_right()
            elif (act.find("MOVE-AHEAD-0") != -1) or (act.find("MOVE-AHEAD-90") != -1) or (act.find("MOVE-AHEAD-180") != -1) or (act.find("MOVE-AHEAD-270") != -1):
                self.move_ahead()



    def move_ahead(self):
        self.controller.step("MoveAhead")

    def rotate_left(self):
        self.controller.step("RotateLeft")

    def rotate_right(self):
        self.controller.step("RotateRight")
        
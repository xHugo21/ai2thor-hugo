# Fichero para parsear los datos de un plan en acciones ejecutables por AI2THOR
from ai2thor.controller import Controller
from aux import extractActionImage


class ParserPDDLAI2THOR:
    def __init__(self, raw_plan, controller):
        self.actions = []
        self.executable_actions = []
        self.controller = controller
        self.objects = self.controller.last_event.metadata["objects"] # TODO puede que en futuros casos se deba actualizar ocn cada acción

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
        
        # Elimina la última posición si está vacía
        if self.actions[-1] == ' ':
            self.actions.pop()
        

    def parse_actions(self):
        # Creamos foto situación inicial
        extractActionImage(self.controller.last_event, 'paso0')
        n_image = 1

        for act in self.actions:
            if act.find("ROTATE-LEFT") != -1:
                #self.executable_actions.append('controller.step("RotateLeft")')
                self.controller.step("RotateLeft")

            elif act.find("ROTATE-RIGHT") != -1:
                #self.executable_actions.append('controller.step("RotateRight")')
                self.controller.step("RotateRight")

            elif (act.find("MOVE-AHEAD-0") != -1) or (act.find("MOVE-AHEAD-90") != -1) or (act.find("MOVE-AHEAD-180") != -1) or (act.find("MOVE-AHEAD-270") != -1):
                #self.executable_actions.append('controller.step("MoveAhead")')
                self.controller.step("MoveAhead")

            elif act.find("LOOKUP") != -1:
                #self.executable_actions.append('controller.step(action="LookUp", degrees=30)')
                self.controller.step(action="LookUp", degrees=30)

            elif act.find("LOOKDOWN") != -1:
                #self.executable_actions.append('controller.step(action="LookDown", degrees=30)')
                self.controller.step(action="LookDown", degrees=30)

            elif act.find("PICKUP") != -1:
                start_index = act.find("PICKUP")
                end_index = act.find(" POSE")
                obj_name = act[start_index+7:end_index]
                for obj in self.objects:
                    if obj["name"].upper() == obj_name:
                        #self.executable_actions.append(f'controller.step(action="PickupObject", objectId={obj["objectId"]})')
                        self.controller.step(action="PickupObject", objectId=obj["objectId"])

            elif act.find("DROP") != -1:
                #self.executable_actions.append('controller.step("DropHandObject")')
                self.controller.step("DropHandObject")
            
            # Extraemos una foto del paso ejecutado
            extractActionImage(self.controller.last_event, f'paso{n_image}')
            n_image += 1
        
        #return self.executable_actions


                



        
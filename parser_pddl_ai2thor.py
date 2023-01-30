# Fichero para parsear los datos de un plan en acciones ejecutables por AI2THOR
from ai2thor.controller import Controller
from aux import extractActionImage


class ParserPDDLAI2THOR:
    def __init__(self, raw_plan, controller, iteracion, liquid, ogamus=False):
        '''Método init de la clase ParserPDDLAI2THOR. Guarda los parámetros y ejecuta los métodos necesarios para convertir el plan generado a acciones de ai2thor'''
        self.actions = []
        self.executable_actions = []
        self.controller = controller
        self.objects = self.controller.last_event.metadata["objects"] # TODO puede que en futuros casos se deba actualizar ocn cada acción

        self.extract_plan(raw_plan)

        print("*EJECUTANDO PLAN SOBRE ENTORNO AI2THOR*\n")

        if ogamus == False:
            self.parse_actions(iteracion, liquid)
        else:
            self.parse_actions_ogamus(iteracion)

        print("*PLAN EJECUTADO SATISFACTORIAMENTE*\n")

    
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

        print(self.actions)
        

    def parse_actions(self, iteracion, liquid):
        '''Método que identifica cada acción junto a sus parámetros y la ejecuta. Extrae además una foto en ./images/ del simulador después de ejecutar cada acción'''
        # Creamos foto situación inicial
        extractActionImage(self.controller.last_event, f'iter{iteracion}_0')
        n_image = 1

        for act in self.actions:
            if act.find("ROTATE-LEFT") != -1:
                self.controller.step("RotateLeft")

            elif act.find("ROTATE-RIGHT") != -1:
                self.controller.step("RotateRight")

            elif (act.find("MOVE-AHEAD-0") != -1) or (act.find("MOVE-AHEAD-90") != -1) or (act.find("MOVE-AHEAD-180") != -1) or (act.find("MOVE-AHEAD-270") != -1):
                self.controller.step(action="MoveAhead")

            elif act.find("LOOKUP") != -1:
                self.controller.step(action="LookUp", degrees=30)

            elif act.find("LOOKDOWN") != -1:
                self.controller.step(action="LookDown", degrees=30)
            
            elif act.find("STANDUP") != -1:
                self.controller.step("Stand")

            elif act.find("CROUCH") != -1:
                self.controller.step("Crouch")

            elif act.find("PICKUP") != -1:
                self.object_state_action(act, "PICKUP", 7, "PickupObject")

            elif act.find("OPEN") != -1:
                self.object_state_action(act, "OPEN", 5, "OpenObject")
            
            elif act.find("CLOSE") != -1:
                self.object_state_action(act, "CLOSE", 6, "CloseObject")

            elif act.find("BREAK") != -1:
                self.object_state_action(act, "BREAK", 6, "BreakObject")
            
            elif act.find("COOK") != -1:
                self.object_state_action(act, "COOK", 5, "CookObject")

            elif act.find("SLICE") != -1:
                self.object_state_action(act, "SLICE", 6, "SliceObject")

            elif act.find("TOGGLEON") != -1:
                self.object_state_action(act, "TOGGLEON", 9, "ToggleObjectOn")

            elif act.find("TOGGLEOFF") != -1:
                self.object_state_action(act, "TOGGLEOFF", 10, "ToggleObjectOff")

            elif act.find("DIRTY") != -1:
                self.object_state_action(act, "DIRTY", 6, "DirtyObject")
            
            elif act.find("CLEAN") != -1:
                self.object_state_action(act, "CLEAN", 6, "CleanObject")

            elif act.find("FILL") != -1:
                self.object_state_action(act, "FILL", 5, "FillObjectWithLiquid", liquid)

            elif act.find("EMPTY") != -1:
                self.object_state_action(act, "EMPTY", 6, "EmptyLiquidFromObject")

            elif act.find("USEUP") != -1:
                self.object_state_action(act, "USEUP", 6, "UseUpObject")

            elif act.find("DROP") != -1:
                self.controller.step(action="DropHandObject", forceAction=True)
            
            elif act.find("PUT") != -1:
                start_index = act.find("PUT ")
                end_index1 = act.find(" ", start_index+4)
                end_index2 = act.find(" ", end_index1)
                print(end_index2)
                obj_name = act[start_index+4:end_index2]
                for obj in self.objects:
                    if (obj["name"].upper() == obj_name) and ("PUT" != "FILL"):
                        self.controller.step(action="PutObject", objectId=obj["objectId"], forceAction=True)

            if self.controller.last_event.metadata['errorMessage']:
                print(f'Error: {self.controller.last_event.metadata["errorMessage"]}')
                print("Reinicie el programa e intente con otra acción\n")
                exit()
            
            # Extraemos una foto del paso ejecutado
            extractActionImage(self.controller.last_event, f'iter{iteracion}_{n_image}')
            n_image += 1
        
        #return self.executable_actions
    
    def object_state_action(self, act, action_name_domain, plus_index, action_name_ai2thor, liquid='coffee'):
        '''Método que ejecuta las acciones de cambio de estado de objetos al ser similares entre sí'''
        start_index = act.find(action_name_domain)
        end_index = act.find(" POSE")
        obj_name = act[start_index+plus_index:end_index]
        for obj in self.objects:
            if (obj["name"].upper() == obj_name) and (action_name_domain != "FILL"):
                self.controller.step(action=action_name_ai2thor, objectId=obj["objectId"])
            elif (obj["name"].upper() == obj_name) and (action_name_domain == "FILL"):
                self.controller.step(action=action_name_ai2thor, objectId=obj["objectId"], fillLiquid=liquid)

    def parse_actions_ogamus(self, iteracion):
        '''Método que identifica cada acción junto a sus parámetros y la ejecuta. Extrae además una foto en ./images/ del simulador después de ejecutar cada acción'''
        # Creamos foto situación inicial
        extractActionImage(self.controller.last_event, f'iter{iteracion}_0')
        n_image = 1

        if self.actions[0].find('PICKUP') != -1:
            self.object_state_action_ogamus(self.actions[0], "PICKUP", 7, "PickupObject")
        elif self.actions[0].find("OPEN") != -1:
                self.object_state_action_ogamus(self.actions[0], "OPEN", 5, "OpenObject")
        elif self.actions[0].find("CLOSE") != -1:
            self.object_state_action_ogamus(self.actions[0], "CLOSE", 6, "CloseObject")

        if self.controller.last_event.metadata['errorMessage']:
            print(f'Error: {self.controller.last_event.metadata["errorMessage"]}')
            print("Reinicie el programa e intente con otra acción\n")
            exit()
        
        # Extraemos una foto del paso ejecutado
        extractActionImage(self.controller.last_event, f'iter{iteracion}_{n_image}')
    
    def object_state_action_ogamus(self, act, action_name_domain, plus_index, action_name_ai2thor):
        
        start_index = act.find(action_name_domain)
        end_index = len(act)
        obj_name = act[start_index+plus_index:end_index]
        print(obj_name)
        for obj in self.objects:
            print(obj["objectId"])
        
                



        
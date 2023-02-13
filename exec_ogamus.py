from aux import printObjectStatus, extractActionImage

class ExecOgamus:
    '''Clase encargada de ejecutar la acción indicada en el problema una vez que el objeto ha sido encontrado'''
    def __init__(self, controller, problem, objective, iteracion):
        '''Método constructor'''
        self.controller = controller
        self.problem = problem
        self.objective = objective

        # Se extrae una imágen del estado inicial
        extractActionImage(self.controller.last_event, f'problem{iteracion}_0')

        # Se almacena en self.final_objective el objeto concreto con el que se va a interactuar
        self.get_object_id()

        # Se identifica el problema y se ejecuta su acción correspondiente. Si self.problem == 'get_close_to' -> se ignora porque no tiene que realizar acción
        if self.problem == 'pickup':
            self.controller.step(action="PickupObject", objectId=self.final_objective["objectId"], forceAction=True)

        elif self.problem == 'open':
            self.controller.step(action="OpenObject", objectId=self.final_objective["objectId"], forceAction=True)
            
        elif self.problem == 'close':
            self.controller.step(action="CloseObject", objectId=self.final_objective["objectId"], forceAction=True)

        elif self.problem == "break":
            self.controller.step(action="BreakObject", objectId=self.final_objective["objectId"], forceAction=True)

        elif self.problem == "cook":
            self.controller.step(action="CookObject", objectId=self.final_objective["objectId"], forceAction=True)

        elif self.problem == "slice":
            self.controller.step(action="SliceObject", objectId=self.final_objective["objectId"], forceAction=True)

        elif self.problem == 'drop':
            self.controller.step(action="DropHandObject", forceAction=True)

        elif self.problem == 'put':
            self.controller.step(action="PutObject", objectId=self.final_objective["objectId"], forceAction=True, placeStationary=True)

        # Se imprime por pantalla el estado final del objetivo para comprobar si se ha modificado
        printObjectStatus(self.controller.last_event, self.final_objective)

        # Se extrae una imagen del estado final tras realizar la acción
        self.controller.step("Pass")
        extractActionImage(self.controller.last_event, f'problem{iteracion}_1')
            

    def get_object_id(self):
        '''Método que extrae el objeto de la escena para poder referenciarlo en las acciones'''
        for obj in self.controller.last_event.metadata["objects"]:
            aux = obj["objectId"].upper().find(self.objective.upper())
            if aux != -1:
                if obj["objectId"][aux+len(self.objective)] == "|":
                    print(obj["objectId"])
                    self.final_objective = obj



    
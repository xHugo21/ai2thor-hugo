from aux import printObjectStatus, extractActionImage

class ExecOgamus:
    def __init__(self, controller, problem, objective, iteracion):
        self.controller = controller
        self.problem = problem
        self.objective = objective

        extractActionImage(self.controller.last_event, f'problem{iteracion}_0')

        self.get_object_id()

        if self.problem == 'get_close_to':
            pass

        elif self.problem == 'pickup':
            self.controller.step(action="PickupObject", objectId=self.final_objective["objectId"], forceAction=True)

        elif self.problem == 'open':
            self.controller.step(action="OpenObject", objectId=self.final_objective["objectId"], forceAction=True)
            
        elif self.problem == 'close':
            self.controller.step(action="CloseObject", objectId=self.final_objective["objectId"], forceAction=True)

        elif self.problem == 'drop':
            self.controller.step(action="DropHandObject", forceAction=True)

        elif self.problem == 'put':
            self.controller.step(action="PutObject", objectId=self.final_objective["objectId"], forceAction=True, placeStationary=True)

        printObjectStatus(self.controller.last_event, self.final_objective)

        self.controller.step("Pass")
        extractActionImage(self.controller.last_event, f'problem{iteracion}_1')
            

    def get_object_id(self):
        for obj in self.controller.last_event.metadata["objects"]:
            aux = obj["objectId"].upper().find(self.objective.upper())
            if aux != -1:
                if obj["objectId"][aux+len(self.objective)] == "|":
                    print(obj["objectId"])
                    self.final_objective = obj



    
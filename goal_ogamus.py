from aux import printObjectStatus

class GoalOgamus:
    def __init__(self, problem_path, problem, objective):
        self.problem_path = problem_path
        self.problem = problem
        self.objective = objective

        self.read_pddl()

        if self.problem == 'get_close_to':
            pass

        elif self.problem == 'pickup':
            self.goal_state = f'(exists (?o1 - {self.objective}) (and (viewing ?o1) (holding ?o1) '

        elif self.problem == 'open':
            self.goal_state = f'(exists (?o1 - {self.objective}) (and (viewing ?o1) (open ?o1) '

        elif self.problem == 'close':
            self.goal_state = f'(exists (?o1 - {self.objective}) (and (viewing ?o1) (not (open ?o1)) '

        self.write_pddl()
        
    def read_pddl(self):
        with open(self.problem_path, "r") as f:
            self.problem_str = f.read()
        #index_objective = self.problem_str.find(self.objective+"_")
        #self.final_objective = self.problem_str[index_objective:index_objective+len(self.objective)+2]

    def write_pddl(self):
        start_index = self.problem_str.find('(:goal') + 12
        end_index = self.problem_str.find('(close_to ?o1)')
        self.problem_str = self.problem_str[:start_index] + self.goal_state + self.problem_str[end_index:]

        with open(self.problem_path, "w") as f:
            f.write(self.problem_str)

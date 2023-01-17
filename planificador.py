# Fichero que contiene todas las funciones relacionadas con la ejecución de los planificadores
import os

class Planificador():
    def __init__(self, planner_path, domain_path, problem_path, output_path, search_algorithm=0, print=False):
        self.planner_path = planner_path
        self.domain_path = domain_path
        self.problem_path = problem_path
        self.output_path = output_path

        self.run_plan(search_algorithm)

        if print:
            self.print_plan()

    def run_plan(self, search_algorithm):
        '''Método que ejecuta el planificador sobre los archivos indicados'''    
        try:
            os.system(f'./{self.planner_path} -o {self.domain_path} -f {self.problem_path} -S {search_algorithm} -H 1 > {self.output_path}')
        except FileNotFoundError:
            raise Exception("Error al ejecutar el planificador (Archivo no encontrado)")

    def print_plan(self):
        '''Método que permite imprimir el resultado del plan introducido por parámetro'''
        with open(self.output_path, 'r') as f:
            print(f.read())

    def get_plan (self):
        with open(self.output_path, 'r') as f:
            raw_plan = f.read()
        return raw_plan
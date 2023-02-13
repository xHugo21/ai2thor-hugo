# Fichero que contiene todas las funciones relacionadas con la ejecución de los planificadores
import os

class Planificador():
    def __init__(self, problem_path, output_path, problem, print=False, ogamus=False):
        '''Método init de Planificador. Inicia los parámetros y llama a los métodos necesarios para ejecutar el planificador'''
        if ogamus == False:
            self.domain_path = f'./pddl/domain_{problem}.pddl'
        else:
            self.domain_path = './pddl/domain_input.pddl'
        self.problem_path = problem_path
        self.output_path = output_path
        
        self.run_plan_cbp()

        if print:
            self.print_plan()

    def run_plan_cbp(self):
        '''Método que ejecuta el planificador sobre los archivos indicados'''    
        try:
            os.system(f'./pddl/cbp-roller/cbp-roller -o {self.domain_path} -f {self.problem_path} > {self.output_path}')
        except FileNotFoundError:
            raise Exception("Error al ejecutar el planificador (Archivo no encontrado)")  

    def print_plan(self):
        '''Método que permite imprimir el resultado del plan introducido por parámetro'''
        with open(self.output_path, 'r') as f:
            print(f.read())

    def get_plan (self):
        '''Método que extrae el plan generado a una variable que es devuelta'''
        with open(self.output_path, 'r') as f:
            raw_plan = f.read()
        return raw_plan
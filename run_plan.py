# Fichero que contiene todas las funciones relacionadas con la ejecución de los planificadores
import os

def run_plan(planner_path, domain_path, problem_path, output_path):
    '''Función que ejecuta el planificador sobre los archivos indicados'''    
    try:
        os.system(f'./{planner_path} -o {domain_path} -f {problem_path} -S 1 -O > {output_path}')
    except FileNotFoundError:
        raise Exception("Error al ejecutar el planificador (Archivo no encontrado)")

def print_plan(output_path):
    '''Función que permite imprimir el resultado del plan introducido por parámetro'''
    with open(output_path, 'r') as f:
        print(f.read())

def save_plan (output_path):
    with open(output_path, 'r') as f:
        raw_plan = f.read()
    return raw_plan
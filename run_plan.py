import os


def run_plan(planner_path, domain_path, problem_path, output_path):
    '''Función que ejecuta el planificador sobre los archivos indicados'''    
    try:
        os.system(f'./{planner_path} -o {domain_path} -f {problem_path} -F {output_path} -O -h 3 -H 1 -S 9 -n')
    except FileNotFoundError:
        raise Exception("Error al ejecutar el planificador")

def print_plan(output_path):
    '''Función que permite imprimir el resultado del plan introducido por parámetro'''
    with open(output_path, 'r') as f:
        print(f.read())
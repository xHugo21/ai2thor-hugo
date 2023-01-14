import os

def plan(planner_path, domain_path, problem_path, output_path):
    '''Función que ejecuta el planificador sobre los archivos indicados'''    

    try:
        os.system(f'./{planner_path} -o {domain_path} -f {problem_path} -F {output_path} -O -h 3 -H 1 -S 9 -n')
    except FileNotFoundError:
        raise Exception("Error al ejecutar el planificador")
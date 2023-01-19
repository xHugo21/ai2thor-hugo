# Ejecución de acciones automatizadas sobre AI2THOR mediante planificación automática (PDDL)

## Trabajo Fin de Grado - Hugo García Cuesta 100428954

### Documentacion
- AI2THOR documentation: https://ai2thor.allenai.org/ithor/documentation/

### Ejecución
1. Clonar repositorio en carpeta local
2. Comprobar apartado de requisitos e instalar o modificar componentes necesarios
3. Descargar planificador cbp-roller y modificar ruta de acceso al mismo. La variable debe modificarse en el archivo problem_definition.py dentro de la función paths_selection(). La variable es self.planner_path
4. Ejecutar main.py con Python
    3.1 Seleccionar escena
    3.2 Seleccionar problema/accion
    3.3 Seleccionar objetivo
    3.4 Indicar si se desea ejecutar otra acción

*Se recomienda leer detenidamente los pasos escritos en la terminal durante la ejecución del programa

*Se puede visualizar el estado del planificador y el simulador tras cada acción por las salidas que se devuelven en la terminal o en los archivos en ./images, ./pddl/problems y ./pddl/outputs

*Existen otros parámetros modificables dentro del entorno pero se recomienda no modificarlos

### Requisitos
1. Importar las librerías necesarias: ai2thor, Pillow, numpy, opencv-python. Mi entorno utilizado en requirements.txt
2. Sistema operativo: macOS 10.9 o Ubuntu 14.04+ (recomendado: Ubuntu 22.04)
3. Python: 3.5+ (recomendado: Python 3.10.6)
4. CPU con soporte de instrucciones SSE2
5. GPU: DX9 (shader model 3.0) or DX11 with feature level 9.3 capabilities
6. Para usuarios de Linux necesario X server con módulo GLX habilitado



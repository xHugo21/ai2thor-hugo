# Ejecución de acciones sobre AI2THOR mediante planificación automática (PDDL)

## Trabajo Fin de Grado - Hugo García Cuesta 100428954

### Introducción
Se pretende realizar un programa que permita moverse e interactuar al agente AI2THOR con los objetos indicados

Para conseguirlo se puede ejecutar el programa de dos maneras:
    1. Mediante metadatos extraídos del simulador. De esta manera se conocen las posiciones de los objetos y posiciones permitidas. Sobre estos datos se ejecutan diferentes dominios escritos en PDDL para obtener y ejecutar el plan generado de manera óptima.
    2. Mediante el algoritmo OGAMUS, elaborado por Leonardo Lamanna, Luciano Serafini, Alessandro Saetti, Alfonso Gerevini y Paolo Traverso. Este algoritmo permite obtener información de un entorno recorriendolo gracias a una serie de redes de neuronas preentrenadas. Esta información es devuelta a un fichero PDDL desde el cuál se puede comprobar si se ha llegado a un estado meta y ejecutar acciones del simulador a partir de él.

### Ejecución
1. Clonar repositorio en carpeta local
2. Comprobar apartado de requisitos e instalar o modificar componentes necesarios
3. Crear un entorno virtual de Python 3.9 en conda (conda create -n ogamus python=3.9)
4. Activar el entorno (conda activate ogamus)
5. Instalar pip en el entorno (conda install pip)
6. Instalar PyTorch (versión 1.11.0) (https://pytorch.org/get-started/locally/)
7. Instalar AI2THOR (versión 5.0.0) (pip install ai2thor)
8. Instalar matplotlib (pip install matplotlib)
9. Descargar la red neuronal preentrenada del siguiente enlace y volcar archivos en /Utils/pretrained_models/ (https://drive.google.com/drive/folders/1UjADpBeBOMUKXQt-qSULIP3vM90zr_MR?usp=sharing)
10. Ejecutar main.py dentro del entorno y seguir los pasos que se muestran por pantalla (python main.py)

### Visualización de resultados
El simulador de AI2THOR se ejecuta a una velocidad muy alta y no permite visualizar los pasos de manera correcta. Para solucionar esto se extraen las siguientes imágenes en cada ejecución dentro de la carpeta ./images:
    - scene.png: Una imágen de la escena desde el plano cenital que permite ver los objetos y la posición inicial del agente.
    - iterX_Y: Una imagen de los pasos ejecutados donde X indica la iteración e Y indica el paso.

Se extraen además los archivos de problemas pddl y salidas de planificador en ./pddl/problems y ./pddl/outputs respectivamente.

### Requisitos de AI2THOR
1. Sistema operativo: macOS 10.9 o Ubuntu 14.04+ (recomendado: Ubuntu 22.04)
2. Python: 3.5+ 
3. CPU con soporte de instrucciones SSE2
4. GPU: DX9 (shader model 3.0) or DX11 with feature level 9.3 capabilities
5. Para usuarios de Linux necesario X server con módulo GLX habilitado

### Documentacion
- AI2THOR documentation: https://ai2thor.allenai.org/ithor/documentation/
- LAMANNA, Leonardo, et al. Online grounding of symbolic planning domains in unknown environments. En Proceedings of the International Conference on Principles of Knowledge Representation and Reasoning. 2022. p. 511-521.
- OGAMUS GitHub: https://github.com/LamannaLeonardo/OGAMUS





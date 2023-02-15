# Ejecución de acciones sobre AI2THOR mediante planificación automática (PDDL)

## Trabajo Fin de Grado - Hugo García Cuesta 100428954

### Introducción
Se pretende realizar un programa que permita moverse e interactuar al agente AI2THOR con el entorno

El programa ofrece la opción de ejecutarse de dos maneras:
1. Mediante metadatos extraídos del simulador. De esta manera se conocen las posiciones de los objetos y posiciones permitidas. Sobre estos datos se ejecutan diferentes dominios escritos en PDDL para obtener y ejecutar el plan generado de manera óptima.
2. Mediante el algoritmo OGAMUS, elaborado por Leonardo Lamanna, Luciano Serafini, Alessandro Saetti, Alfonso Gerevini y Paolo Traverso. Este algoritmo permite obtener información de un entorno recorriendolo gracias a una serie de redes de neuronas preentrenadas. En este trabajo se ha modificado ligeramente el código original con el fin de conseguir concatenar acciones simples en entornos desconocidos. Además, se permite introducir un fichero de entrada en formato PDDL que puede procesar acciones simples y complejas y pasarlas al algoritmo para ser resueltas.

### Requisitos de AI2THOR
1. Sistema operativo: macOS 10.9 o Ubuntu 14.04+ (recomendado: Ubuntu 22.04)
2. Python: 3.5+ 
3. CPU con soporte de instrucciones SSE2
4. GPU: DX9 (shader model 3.0) or DX11 with feature level 9.3 capabilities
5. Para usuarios de Linux necesario X server con módulo GLX habilitado

### Ejecución
1. Comprobar apartado de requisitos e instalar o modificar componentes necesarios

2. Clonar repositorio en carpeta local y acceder al directorio
    ```
    git clone https://github.com/xHugo21/ai2thor-hugo.git
    ```
    ```
    cd ai2thor-hugo
    ```
3. Crear un entorno virtual de Python 3.9 en conda
    ```
    conda create -n ogamus python=3.9
    ```
4. Activar el entorno
    ```
    conda activate ogamus
    ```
5. Instalar pip en el entorno
    ```
    conda install pip
    ```
6. Instalar [PyTorch](https://pytorch.org/get-started/locally/) (versión 1.11.0) *Ejecutar solo uno de los siguientes comandos

    ```
    # OSX
    conda install pytorch==1.11.0 torchvision==0.12.0 torchaudio==0.11.0 -c pytorch
    ```

    ```
    # CUDA 10.2
    conda install pytorch==1.11.0 torchvision==0.12.0 torchaudio==0.11.0 cudatoolkit=10.2 -c pytorch

    # CUDA 11.3
    conda install pytorch==1.11.0 torchvision==0.12.0 torchaudio==0.11.0 cudatoolkit=11.3 -c pytorch

    # CPU Only
    conda install pytorch==1.11.0 torchvision==0.12.0 torchaudio==0.11.0 cpuonly -c pytorch
    ```

7. Instalar AI2THOR (versión 5.0.0) 
    ```
    pip install ai2thor==5.0.0
    ```
8. Instalar matplotlib 
    ```
    pip install matplotlib
    ```
9. Descargar la red neuronal preentrenada del siguiente enlace [Red Preentrenada](https://drive.google.com/drive/folders/1UjADpBeBOMUKXQt-qSULIP3vM90zr_MR?usp=sharing) y volcar archivos en /Utils/pretrained_models/

10. Ejecutar main.py y seguir los pasos que se muestran por pantalla
    ```
    python main.py
    ```
11. Adicionalmente se permite la introducción de un fichero PDDL como parámetro para indicar los problemas a resolver. Cabe tener en cuenta que esto solo se permite para el método OGAMUS. Hay un fichero PDDL de ejemplo en ./pddl/inputs/example.pddl
    ```
    python main.py ./pddl/inputs/example.pddl
    ```

### Visualización de resultados
El simulador de AI2THOR se ejecuta a una velocidad muy alta y no permite visualizar los pasos de manera correcta. Para solucionar esto se extraen las siguientes imágenes en cada ejecución dentro de la carpeta ./images:
- scene.png: Una imágen de la escena desde el plano cenital que permite ver los objetos y la posición inicial del agente.
- problemX_Y: Una imagen de los pasos ejecutados donde X indica la iteración e Y indica el paso.

Se extraen además los archivos de problemas pddl y salidas de planificador en ./pddl/problems y ./pddl/outputs respectivamente.

### Documentacion
- AI2THOR documentation: https://ai2thor.allenai.org/ithor/documentation/
- LAMANNA, Leonardo, et al. Online grounding of symbolic planning domains in unknown environments. En Proceedings of the International Conference on Principles of Knowledge Representation and Reasoning. 2022. p. 511-521. [PDF](https://arxiv.org/pdf/2112.10007.pdf)
- OGAMUS GitHub: https://github.com/LamannaLeonardo/OGAMUS





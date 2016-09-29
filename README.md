<h3 align="center">MULTIPLICACIÓN DE MATRICES EN CPU, GPU Y CON MEMORIA COMPARTIDA</h3>
<br>
<br>
<br>
<br>
<br>
<h3 align="center">HIGH PERFORMANCE COMPUTING</h3>
<br>
<br>
<br>
<br>
<br>
<h3 align="center">DOUGLAS HERNANDEZ</h3>
<h3 align="center">SEBASTIAN ARDILA AGUDELO</h3>
<br>
<br>
<br>
<br>
<br>
<h3 align="center">UNIVERSIDAD TECNOLÓGICA DE PEREIRA</h3>
<h3 align="center">INGENIERÍA DE SISTEMAS Y COMPUTACION</h3>
<h3 align="center">2016</h3>
<br>
<HR width=100% align="center">
<br>
<h2 >ENUNCIADO</h2>
* Construir un programa que calcule la multiplicación de matrices (floats) de cualquier tamaño A(mxn) y B(nxl) en  CUDA.

* Evaluar el desempeño del programa construido comparándolo con la versión secuencial del mismo en CPU

* Se debe entregar un repositorio en github por equipo de trabajo que contenga el código de ambas implementaciones, documentación sobre el código donde se explique detalladamente lo que se hace y un documento  en MarkDown donde se especifiquen gráficas de tiempos y gráfica de aceleración (se hace con un promedio de tiempos). Se debe tener un apartado de conclusiones del trabajo realizado.
<br>
<h2 >GENERACIÓN DEL CÓDIGO</h2>
Para el Desarrollo de nuestro código, tomamos en cuenta los talleres hechos previamente, como podemos observar en el archivo **parcial1.c**, el código se divide en varias partes dónde las funciones mas importantes son **matrixMultKernelTiled**,**matrixMultKernel**,**matrixMultCPU2**, las cuales se refieren respectivamente a la función de multiplicación en gpu con memoria compartida, la funcion para gpu, y la función para cpu, estas funciones reciben tres matrices, de las cuales dos tienen valores a multiplicar y la tercera siempre es la matriz lineal donde se guardarán los resultados, los tiempos de ejecucion y aceleración se toman mediante se hace el llamado a las funciones, y las matrices resultantes son analizadas para verificar que las multiplicaciones sean correctas.
<br>
<h2 >PROBLEMAS QUE SE ENCUENTRAN</h2>
Se encuentran originalmente problemas en la multiplicación correcta de matrices en CPU, problemas que se resuelven haciendo un análisis escrito y concienzudo de la multiplicación. La multiplicación de las matrices en GPU se resuelve de manera similar. Actualemente encontramos un error en la multiplicación en GPU con memoria compartida apesar de revisar el libro detalladamente, la multiplicación de las matrices en **GPU tiled** funciona para todos los valores menos para el último, por lo que se procede a hacer el análisis de gráficas de tiempos de ejecución versus aceleración con la multiplicación en CPU y GPU sin tiled
<br>
<HR width=100% align="center">
<br>
<h2>RECOLECCIÓN DE DATOS</h2>
1. En la tabla 1.1, se puede observar los tamaños de las matrices usados para CPU y GPU
![Texto alternativo](/Tamaños_matriz.PNG)
<br>

<h3 align="center">FILTRO DE SOBEL</h3>
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
Construir un programa que aplique el filtro de sobel tanto en cpu como en gpu
<br>
<h2 >GENERACIÓN DEL CÓDIGO</h2>
Para el desarrollo de nuestro código tuvimos que tener en cuenta el desarrollo del código pasado de la convolución de matrices,
también tuvimos en cuenta la transformación de una imagen a escala de grises que habíamos hecho previamente, y estudiamos
los axiomas del operador sobel. Usamos diferente contenido de internet que nos explicaba como debía aplicarse el filtro
matematicamente hablando, al concatenar toda la información pudimos determinar que el operador de sobel se aplicaba haciendo
convolución a una imagen en escala de grises con el filtro de sobel y pasandole por encima posteriormente el cálculo de la 
magnitud y la transformación necesaria.
<br>
<br>
Calculo que se hace teniendo en cuenta que las dos matrices numericas visibles son tomadas como la máscara para la convolucion en cada punto dependiendo del contexto horizontal o vertical, la letra **A** representa la imagen original en escala de grises, mientras que las letras **Gx** y **Gy** son las matrices resultantes de las operaciones en la imagen:
<br>
![gráfica 1.1](/sobel/mult.png)
<br>
<br>
Cálculo del vector magnitud que se hace utilizando el resultado de las matrices previamente calculadas:
<br>
![gráfica 1.2](/sobel/magnitud.png)



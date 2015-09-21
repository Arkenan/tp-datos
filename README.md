# tp-datos

# Armadillo
Las cosas se bajan de http://arma.sourceforge.net/download.html (versión 5.6, es la estable).

Documentación de la librería en http://arma.sourceforge.net/docs.html

# Ejemplos Armadillo
- randoms.m: archivo de octave para generar los csv de entrada para cuadMin.cpp
- cuadmin.cpp: toma un training set de trainingGrad.csv y entrena un vector de parámetros theta con varios métodos:
  - Solve de armadillo.
  - Ecuaciones normales para cuadrados mínimos,
  - Gradient Descent sin feature scaling. Performance mala.
  - Gradient Descent con feature scaling. Precisión similar a los no iterativos.
  
  En particular está pensado para estimar coeficientes de ecuaciones cuadráticas. Cuando termina cada método lo usa para predecir los resultados en un vector de prueba obtenido de crossGrad.csv y con los resultados verdaderos que vienen de crossGradRes.csv.

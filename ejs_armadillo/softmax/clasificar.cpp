#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

// Cálculo de hipótesis ejemplo a ejemplo por filas.
mat h(mat Theta, mat X){
	mat B = X*Theta;
	return diagmat(pow(sum(exp(B.t())),-1))*exp(B);
}

// Transforma de un vector de K categorías a una matriz binaria de K columnas.
mat binarizar(vec cat, int K){
	mat binaria = zeros(cat.n_rows, K);
	for (int i = 0; i < cat.n_rows; i++){
		// Pone un 1 en la columna especificada por la categoría (0 - K-1).
		binaria(i,cat[i]) = 1;
	}
	return binaria;
}

// Devuelve la clase de mayor probabilidad para cada hipótesis.
vec convertirAClase(mat probabilidades){
	uword index;
	vec p(probabilidades.n_rows);
	for (u32 i = 0; i < p.n_elem; i++){
		probabilidades.row(i).max(index);
		p[i] = index;
	}
	return p;
}

// Habría que ver cómo modularizo esto para no repetirlo...
mat predecirSobreSet(mat Theta, rowvec mu, rowvec sigma, mat ejs){
	// Agrego features cuadráticos.
	ejs = join_rows(ejs,pow(ejs,2));

	// Normalizo.
	for (u32 feat = 0; feat < ejs.n_cols ; feat++){
		ejs.col(feat) = (ejs.col(feat) - mu[feat])/sigma[feat];
	}

	// Agrego Bias
	ejs = join_rows(ones(ejs.n_rows),ejs);
	mat hret = h(Theta,ejs);

	return hret;
}

double porcentaje(vec errores){
	u32 n = errores.n_elem;
	double acu = 0.0;
	for (u32 i = 1; i < n ; i++){
		if (errores[i] != 0){
			acu ++;
		}
	}
	return acu / n * 100;
}

int main(){
	// Cargo los datos. Hay que ver si se puede
	// trabajar sobre la misma matriz de datos en lugar de duplicar.
	mat datos;
	cout << "Cargando datos..." << endl;
	datos.load("training.csv");
	cout << "Modificando datos..." << endl;

	// Training Set. Esto, normalización y bias incluido, debería ir a una función.
	mat X(datos.n_rows, 2);
	int m = X.n_rows;
	int n = X.n_cols;
	X.col(0) = datos.col(0);
	// Agrego una columna cuadrática.
	X.col(1) = pow(X.col(0),2);

	// Medias y desviaciones por columna.
    rowvec mu = mean(X);
    rowvec sigma = stddev(X);

	// Normalizo X
	for (int feat = 0; feat < n ; feat++){
		X.col(feat) = (X.col(feat) - mu(feat))/sigma(feat);
	} // por ahí esto se puede hacer mejor de alguna manera.

	// Agrego Bias
	X = join_rows(ones(m,1),X);

	// Resultados del Training set.
	vec y = datos.col(datos.n_cols - 1);
	// Cantidad de categorías. Supongo que van de 0 a K - 1;
	int K = max(y) + 1;
	cout << "Cantidad de categorías K: " << K << endl;
	mat Unos = binarizar(y, K);

	// Matriz de parámetros.
	mat Theta(n+1,K);
	// Ver si conviene inincializar random.
	Theta.fill(1.0);
	cout << "Entrenando..." << endl;

	// Gradient Descent para entrenar.
	double alpha = 0.01; // hiperp: paso.
	double lambda = 0.0001; // hiperp: regularización.
	for (int i = 1; i < 5000; i++){
		// Por ahí esto se puede mejorar con diagmat(1-lambda) o algo así.
		Theta = Theta + (alpha)* X.t() * (Unos - h(Theta,X)) - lambda * Theta;
	}
	cout << "Entrenado. Prediciendo sobre set de entrenamiento para ver convergencia..." << endl;

	Theta.print("Matriz de pesos:");
	mat predTrain = predecirSobreSet(Theta,mu,sigma,datos.col(0));
	vec predClase = convertirAClase(predTrain);
	vec erroresTrain = predClase - datos.col(1);
	cout << porcentaje(erroresTrain) << "% de error" << endl;
	cout << "Prediciendo sobre set de testing." << endl;

	// Empiezan las pruebas

	mat datosCross;
	datosCross.load("adivinar.csv");
	mat predicciones = predecirSobreSet(Theta,mu,sigma,datosCross.col(0));
	vec prediccionClase = convertirAClase(predicciones);
	vec errores = prediccionClase - datosCross.col(1);
	cout << porcentaje(errores) << "% de error" << endl;

	return 0;
}

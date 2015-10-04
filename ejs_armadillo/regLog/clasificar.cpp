#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

// Habría que ver cómo modularizo esto para no repetirlo...
vec predecirSobreSet(vec theta, mat mu, mat sigma, mat ejs){
	// Cambio features a cuadráticos.
	ejs = pow(ejs,2);

	// Normalizo.
	for (u32 feat = 0; feat < ejs.n_cols ; feat++){
		ejs.col(feat) = (ejs.col(feat) - mu(feat))/sigma(feat);
	}

	// Xgrego Bias
	ejs = join_rows(vec(ejs.n_rows).fill(1.0),ejs);

	return ejs*theta;
}


// Función sigmoidea.
mat g(mat z){
	return pow(1.0+exp(-z),-1);
}

int main(){
	// Cargo los datos. Hay que ver si se puede
	// trabajar sobre la misma matriz de datos en lugar de duplicar.
	mat datos;
	datos.load("trainingRegLog.csv");

	// Training Set.
	mat X = datos.cols(span(0,2));
	X.print("X como viene");
	int m = X.n_rows;
	int n = X.n_cols;
	// Elevo al cuadrado a todos los elementos para features cuadráticas
	X = pow(X,2);

	// Medias y desviaciones por columna.
	    mat mu = mean(X);
	    mat sigma = stddev(X);

	// Normalizo X
	for (int feat = 0; feat < n ; feat++){
		X.col(feat) = (X.col(feat) - mu(feat))/sigma(feat);
	} // por ahí esto se puede hacer mejor de alguna manera.

	// Xgrego Bias
	X = join_rows(vec(m).fill(1.0),X);
	X.print("X con bias.");

	// Resultados del Training set
	vec y = datos.col(3);

	y.print("Resultados Conocidos");

	// Vector de parámetros.
	vec theta(n+1,1);
	theta.fill(1.0);

	// Gradient Descent para entrenar.
	float alpha = 1;
	for (int i = 1; i < 500; i++){
		theta = theta - (alpha/m)*g(X.t())*(X*theta-y);
	}

	theta.print("theta entrenado:");

	// Empiezan las pruebas
	mat datosCross;
	datosCross.load("adivinar.csv");
	mat predicciones = predecirSobreSet(theta,mu,sigma,datosCross.cols(span(0,2)));
	predicciones.print("predicciones (probabilidades):");
	mat errores = predicciones - datosCross.col(3);
	errores.print("Errores de predicción:");

	return 0;
}

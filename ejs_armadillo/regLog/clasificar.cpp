#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

// Función sigmoidea.
mat g(mat z){
	return pow(1.0+exp(-z),-1);
}

// Si la probabilidad es mayor a 0.5 dar 1, si no, dar 0.
vec convertirAClase(vec predicciones){
	vec p(predicciones.n_elem);
	for (u32 i = 0; i < p.n_elem; i++){
		if (predicciones[i] >= 0.5){
			p[i] = 1;
		} else {
			p[i] = 0;
		}
	}
	return p;
}

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
	return g(ejs*theta);
}

float porcentaje(vec errores){
	u32 n = errores.n_elem;
	float acu = 0.0;
	for (u32 i = 1; i < n ; i++){
		acu += abs(errores[i]);
	}
	return acu / n * 100;
}

int main(){
	// Cargo los datos. Hay que ver si se puede
	// trabajar sobre la misma matriz de datos en lugar de duplicar.
	mat datos;
	cout << "cargando datos..." << endl;
	datos.load("trainingRegLog.csv");
	cout << "modificand datos..." << endl;
	// Training Set.
	mat X = datos.cols(span(0,2));
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

	// Resultados del Training set
	vec y = datos.col(3);

	// Vector de parámetros.
	vec theta(n+1,1);
	theta.fill(1.0);

	cout << "entrenando..." << endl;
	// Gradient Descent para entrenar.
	float alpha = 1;
	for (int i = 1; i < 500; i++){
		theta = theta - (alpha/m)*X.t()*(g(X*theta)-y);
	}
	theta.print("theta entrenado:");

	cout << "prediciendo..." << endl;
	// Empiezan las pruebas

	mat datosCross;
	datosCross.load("adivinar.csv");
	mat predicciones = predecirSobreSet(theta,mu,sigma,datosCross.cols(span(0,2)));
	mat prediccionClase = convertirAClase(predicciones);
	mat errores = prediccionClase - datosCross.col(3);

	cout << porcentaje(errores) << "% de error" << endl;

	return 0;
}

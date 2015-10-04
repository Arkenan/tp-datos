#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

// Función sigmoidea.
mat g(mat z){
	return pow(1.0+exp(-z),-1);
}

int main(){
	// Cargo los datos.
	mat datos;
	datos.load("trainingRed.csv");

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
	for (int i = 1; i < 150; i++){
		theta = theta - (alpha/m)*g(X.t())*(X*theta-y);
	}

	theta.print("theta entrenado:");

	return 0;
}

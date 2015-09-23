#include <iostream>
#include <armadillo>
#include <cmath>
using namespace std;
using namespace arma;

/* Recibe Theta entrenado y un vector de ejemplos a predecir. */
vec predecirSinScaling(vec theta, vec ejs){
	// Agrego features.
	vec unos(ejs.n_rows);
	unos.fill(1.0);
	vec cuadrados = pow(ejs,2);
	mat X = join_rows(join_rows(unos, ejs),cuadrados);

	return X*theta;
}

/* Recibe Theta entrenado, las medias, las desviaciones standard
 * y un vector de ejemplos a predecir. */
vec predecirConScaling(vec theta, mat mu, mat sigma, vec ejs){
	// Agrego features.
	vec unos(ejs.n_rows);
	unos.fill(1.0);
	vec cuadrados = pow(ejs,2);

	// Matriz intermedia sin unos. A normalizar.
	mat A = join_rows(ejs,cuadrados);
	for (u32 feat = 0; feat < A.n_cols ; feat++){
		A.col(feat) = (A.col(feat) - mu(feat))/sigma(feat);
	}

	// Agrego 1s.
	mat X = join_rows(unos, A);

	return X*theta;
}

int main(){
    // Cargo datos del csv del training set.
    mat datos;
	datos.load("trainingGrad.csv");

    // Cargo set de prueba post entrenamiento.
    vec prueba;
    prueba.load("crossGrad.csv");

    // Cargo respuestas para ver errores.
    vec respuestas;
    respuestas.load("crossGradRes.csv");

    // Cantidad de ejemplos.
    int m = datos.n_rows;
    // Cantidad de features (x y x^2)
    int n = 2;

    // Armo el training set matricial.
    vec y = datos.col(1);

    vec unos(m);
    unos.fill(1.0);
    mat X = join_rows(unos, join_rows(datos.col(0) , pow(datos.col(0),2) ));


    X.print("Training Set (X): ");
    y.print("y:");
    
    // Resolución con "solve" de armadillo.
    vec theta = solve(X,y);
    theta.print("Parámetros con solve:");
    vec errores = predecirSinScaling(theta,prueba) - respuestas;
    errores.print("Errores cometidos en la predicción:");

    // Resolución por cuadrados mínimos.
    theta = inv(X.t()*X) * X.t() * y;
    theta.print("Parámetros con ecs normales:");
    errores = predecirSinScaling(theta,prueba) - respuestas;
    errores.print("Errores cometidos en la predicción:");

    // Gradient descent. Inicio Theta en 1.
    theta.fill(1.0);

    float alpha = 0.0005;
    for (int i = 1; i < 10000; i++){
    	theta = theta - (alpha/m)*X.t()*(X*theta-y);
    }
    theta.print("Parámetros por gradient descent sin feature scaling.\n 10000 iteraciones.");
    errores = predecirSinScaling(theta,prueba) - respuestas;
	errores.print("Errores cometidos en la predicción:");

    /* Gradient descent con feature scaling. */

    // Quito la columna de unos. Si solo se usara este algoritmo los agregaría directamente después.
    mat A = X(span(0,m-1),span(1,2));
    // Medias y desviaciones por columna.
    mat mu = mean(A);
    mat sigma = stddev(A);

    mu.print("Mu:");
    sigma.print("Sigma: ");

    // Normalizo A (la columna de 1s de X no se normaliza).
    for (int feat = 0; feat < n ; feat++){
    	A.col(feat) = (A.col(feat) - mu(feat))/sigma(feat);
    } // por ahí esto se puede hacer mejor de alguna manera.

    // Reemplazo los valores normalizados.
    X(span(0,m-1),span(1,n)) = A;

	X.print("X normalizado:");

	theta.fill(1.0);
    alpha = 1.0;
    for (int i = 1; i < 100000; i++){
    	theta = theta - (alpha/m)*X.t()*(X*theta-y);
    }
    theta.print("Parámetros por gradient descent con feature scaling.\n 10000 iteraciones.");

    errores = predecirConScaling(theta,mu,sigma,prueba) - respuestas;
	errores.print("Errores cometidos en la predicción:");

    return 0;
}

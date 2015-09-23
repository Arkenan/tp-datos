#include <iostream>
#include <armadillo>

using namespace std;
using namespace arma;

// Nodos de la hidden layer.
const int N = 3;

//Forward propagation.
float fp(vec input, vec hidden, mat Theta1, mat Theta2){
	return Theta2*Theta1*input;
}

int main(){
	// Cargo los datos.
	mat datos;
	datos.load("trainingRed.csv");
	// Training Set.
	mat X = datos.cols(span(0,2));
	// Resultados del Training set
	vec y = datos.col(3);

	vec input(3);
	vec hidden(N);
	mat Theta1(N,3);
	mat Theta2(1,N);




	return 0;
}

% Genera puntos en 3D en un cubo de lado 30. Genero 100 puntos.
% nTrain = 800000;
nTrain = 10000;
nPrueba = 1000;
function [X, res] = generar(n)
    X = zeros(n,1);
    res = zeros(n,1);

    for i = [1:n]
        % Generamos puntos aleatorios entre 0 y 30. Por ahora es solo la
        % longitud. Después voy a probar que tal sale meterle ruido con la y.
        X(i) = rand()*30;

        % Está todo dividido en 10 cateorías (de a 3).
        res(i) = floor(X(i)/3);
    endfor

endfunction
% Entrenamiento.
[X,res] = generar(nTrain);
csvwrite('training.csv',[X res]);

% Para predecir.
[X2,res2] = generar(nPrueba);
csvwrite('adivinar.csv',[X2 res2]);

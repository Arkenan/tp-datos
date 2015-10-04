% Genera puntos en 3D en un cubo de lado 30. Genero 100 puntos.
a = 0;

function [X, res] = generar(n)

    X = zeros(n,3);
    res = zeros(n,1);

    for i = [1:n]

        x = rand()*30 - 15;
        y = rand()*30 - 15;
        z = rand()*30 - 15;
        X(i,:) = [x y z];

        % Positivo si está en una esfera de radio 20 centrada en el origen.
        if (x^2 + y^2 + z^2) <= 100
            res(i) = 1;
        endif

    endfor
endfunction

% Entrenamiento.
[X,res] = generar(50);
csvwrite("trainingRegLog.csv",[X res]);

% Para predecir.
[X2,res2] = generar(20);
csvwrite("adivinar.csv",[X2 res2]);

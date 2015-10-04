% Genera puntos en 3D en un cubo de lado 30. Genero 100 puntos.

X = zeros(100,3);
res = zeros(100,1);
function [X,res] = generar()
    for i = [1:100]

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

[X,res] = generar();

csvwrite("trainingRegLog.csv",[X res]);

[X,res] = generar();

csvwrite("adivinar.csv",[X res]);

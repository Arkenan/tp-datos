% Training set.

a = zeros(20,2);
for i = [1:20]
  x = rand*20 - 10;
  a(i,1) = x;
  a(i,2) = x^2 - x + 5;
endfor
a
csvwrite("trainingGrad.csv",a);

% Casos a predecir. 50 x randoms. Respuestas aparte.

b = zeros(50,1);
c = zeros(50,1);
for i = [1:50]
  b(i) = rand*20 - 10;
  c(i) = b(i)^2 - b(i) + 5;
endfor
[b c]
csvwrite("crossGrad.csv",b);
csvwrite("crossGradRes.csv",c);
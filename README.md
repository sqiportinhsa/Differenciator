# Дифференциатор

Программа, способная сгенерировать решение дифференцирования и разложения функции в ряд Тейлора в формате tex и pdf

## Чтение формулы

Считывание происходит из txt файла, название которого можно указать при запуске с флагом -i, в случае отсутствия чтение произойдет из файла с названием example.

Оформление в привычном для чтения виде, примеры оформления.
Тригонометрические и прочие функции:

```
sin(x)
cos(x)
tan(x)
ctg(x)
log(x)
exp(x)
```

Арифметика:
```
x+1
2*x
5-3
x^4
6\x
```
Реализация чтения - рекурсивный спуск. Грамматика:
```
Descent                 ::= Get_add_and_sub, '\0';

Get_add_and_sub         ::= Get_mul_and_div {[+, -]}*

Get_mul_and_div         ::= Get_deg {[*, /]}*

Get_deg                 ::= Get_transc {[^]}* 

Get_transc              ::= [sin/cos/...] '(' Get_add_and_sub ')' | Get_primary_expression

Get_primary_expression  ::= '(' Get_add_sub ')' | Get_argument

Get_argument            ::= Get_value | Get_variable

Get_value               ::= ['0' - '9']+ 

Get_variable            ::= ['a' - 'z', 'A' - 'Z'] 
```

В результате чтения выражения строится его дерево, вершины которого могут иметь тип операций, чисел и переменных.

## Дифференцирование

При дифференцировании строится новое дерево, соответствующее производной. При этом преобразования сохраняются в структуру преобразования, состоящую из двух массивов для сохранения указателей на ноды начального и конечного поддеревьев и счетчика для перемещения по массиву.

## Упрощение

В результате дифференцирования получается дерево, требующее упрощения в силу того, что в нем содержатся избыточные операции. При упрощении происходит

* объединение нескольких констант соединенных арифметическими операциями в одну

* удаление умножаемого на ноль поддерева

* удаление поддерева, делящего ноль

* пропуск нейтральных по операции элементов - сложение и вычитание с нулем, умножение и деление на единицу, возведение в первую степень

* замена функций с нулевым аргументом на их значение в нуле

Преобразования так же сохраняются, только в случае замен поддеревья копируются чтобы избежать их дальнейшего изменения, и при окончании работы функции упрощения память освобождается

## Замены

Для того, чтобы длина выражения не превышала размер строки готового файла, производятся замены частей выражения. Принцип: для каждого поддерева вызывается рекурсивная функция вычисления его веса. Вес поддерева вычисляется как сумма весов его поддеревьев и добавка, зависящая от того, какой функцией является поддерево. Веса соответствуют значениям типов операций из `enum Operators`. Таблица значений:

```
DEG = 5,
ADD = 10,
SUB = 11,
MUL = 12,
DIV = 13,
COS = 30,
SIN = 31,
TAN = 32,
CTG = 33,
LOG = 34,
EXP = 35,
```

Вес переменной является константой, указанной в файле замен и равен 5. Веса нод с числами вычисляются исходя из длины числа как `(int) log10(abs(variable) + 1) + 1`. 

В случае если вес поддерева превышает некоторое константное значение (150), ему присваевается значение переменной -1. (Константы объявлены в файле replacer.cpp). После этого осуществляется проход по самому дереву и массивам замен и помеченные поддеревья приобретают свои номера, при этом для каждого помеченного поддерева происходит поиск таких же, и им присваивается тот же номер. На этапе вывода номер преобразовывается в букву с индексом как 
```c++
char letter = 'a' + number % ('w' - 'a' + 1);

int  letter_num = number / ('w' - 'a' + 1);
```

После произведения замен массив преобразований выводится в файл последовательно с добавлением поясняющих фраз из банка `phrases.h`
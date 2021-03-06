// Question 1
// про виды памяти: https://www.intuit.ru/studies/courses/2193/67/lecture/1976?page=6

void foo () {
    int x = 1;          // автоматическая переменная - время жизни ограничено областью видимости, уничтожается при выходе из функции
}

void foo () {
    static int x = 1;   // статическая - значение сохраняется между вызовами функции
}
//подробнее: https://www.math.spbu.ru/user/nlebedin/theory_static.pdf

// динамическая - производится системный вызов с запросом на выделение определённого объема памяти
//                операционной системой
void foo () {
    
#include <vector>
    // способы выделения и освобождения динамической памяти:
    
    /* один объект */
    auto vec = new std::vector<int>(10, 0); // выделение памяти под вектор из 10 элементов и его инициализация
    // возвращается указатель на выделенный в пямяти объект
    delete vec;
    
    /* массив объектов */
    auto matrix = new std::vector<int>[10]; // массив из 10-ти пустых векторов. Дополнительно выделяется память
    delete[] matrix;                        // под количество объектов в массиве, чтобы оператор
    // delete[] вызвал деструктор для каждого объекта в массиве
    
    /* выделение определённого количества байт */
    char* data1 = new char[100500];
    delete[] data1;
    // или
    char* data2 = reinterpret_cast<char*>(::operator new(100500));
    ::operator delete(data2);
    // или
    char* data3 = reinterpret_cast<char*>(calloc(100500, sizeof(char))); // бывает полезно, если нужна память,
    // заполненная нулями (только под linux)
    free(data3);
}
// Основные проблемы: Утечки и двойное удаление - есть в min_theory.cpp

// Question 2
// Про дилемму не понял :(
// Ссылки: https://www.tutorialspoint.com/cplusplus/cpp_references.htm, https://habr.com/ru/post/479358/ (весьма жестко, но интересно)
// Висячие ссылки:
int& fun () {
    int a = 0;
    return a;
}

int main () {
    int a = fun ();
    std::cout << a; // в сложном случае, может быть мусор, хотя в этом примере эта возможность очень маловероятна
}
// Продление жизни (если я правильно понял вопрос) | еще https://habr.com/ru/post/186790/ - прикольный момент, связанный с наследованием
void fun (const int& x) {
    //... использование каких-то манипуляций с x
}
int main () {
    fun (10);   // корректно.
}
/*
 Выдержки из стандарта.
 ⁴ [...] Temporary objects are destroyed as the last step in evaluating the full-expression that (lexically) contains the point where they were created. [...]
 
 ⁵ There are three contexts in which temporaries are destroyed at a different point than the end of the full-expression. [...]
 
 ⁶ The third context is when a reference is bound to a temporary object. The temporary object to which the reference is bound or the temporary object that is the complete object of a subobject to which the reference is bound persists for the lifetime of the reference [...] The exceptions to this lifetime rule are:
 
 (6.9) — A temporary object bound to a reference parameter in a function call persists until the completion of the full-expression containing the call.
 
 (6.10) — The lifetime of a temporary bound to the returned value in a function return statement is not extended; the temporary is destroyed at the end of the full-expression in the return statement.
 */
// Прикольный пример, подтверждающий, что "все работает только на той же инструкции": https://husl.ru/questions/731506



// Question 3
// Классы: https://www.tutorialspoint.com/cplusplus/cpp_classes_objects.htm

/* Пример определения класса */
class Student {
private: /* приватные члены - из класса и friend'ов*/
    // статическое поле класса (например, для учета количества объектов класса)
    static int num_of_students;
    
    // "обычные" поля класса
    int calculus_score;
    int algorithms_score = 175; // значение по умолчанию
    
public: /* публичные члены - откуда угодно*/
    // статический метод класса
    static int get_num_of_students() {
        return num_of_students;
    } // может быть определён и снаружи класса
    
    // при вызове НЕСТАТИЧЕСКОГО метода объекта ему передаётся дополнительный аргумент
    // this (- ключевое слово, обозначающее указатель на объект, метод которого
    // был вызван). В статических методах обращение к this недоступно
    
protected: /* защищённые члены - из класса и наследников*/
    // "обычный" метод класса
    int get_total_score() {
        return calculus_score + algorithms_score;
    }
}; // Больше про модификаторы доступа: https://en.cppreference.com/w/cpp/language/access

/* Конструкторы, операторы присваивания, деструкторы */
class Dummy {
    
    char* name;
    
public:
    // Конструктор
    Dummy(const char* name) {
        /* выделение памяти и
         * копирование имени
         * */
    }
    
    // Конструктор копирования                                 //
    Dummy(const Dummy& other) {                                //
        /* аналогично */                                       //
    }                                                          //
    //
    // Оператор присваивания                                   //
    Dummy& operator=(const Dummy& other) {                     // > Правило Трёх:
        if (&other == this) { // self-assignment check         //   Если в классе есть нетривиальный конструктор копирования,
            return *this;                                      //   оператор присваивания или деструктор, ТО с большой вероятностью
        }                                                      //   все эти методы должны быть реализованы нетривиально
        /* аналогично */                                       //
        return *this;                                          //
    }                                                          //
    //
    // Деструктор                                              //
    ~Dummy {                                                   //
        /* очищение памяти */                                  //
    }                                                          //
    
    // Конструктор перемещения                                 //
    Dummy(Dummy&& other) : name(other.name) { // <- список     //
        // инициализации //
        // чтобы при вызове деструктора                        //
        // объекта other строка не удалилась                   //
        other.name = nullptr;                                  // > Правило Пяти:
    }                                                          //   С появлением move-семантики в C++11 появилась возможность
    //   объявлять конструктор перемещения и оператор присваивания
    // Оператор присваивания перемещением                      //   перемещением. На эти методы обобщается Правило Трёх.
    Dummy& operator=(Dummy&& other) {                          //
        /* self-move check */                                  //
        name = other.name;                                     //
        other.name = nullptr;                                  //
        return *this;                                          //
    }                                                          //
};

// Делегирующие конструкторы: https://ravesli.com/urok-119-delegiruyushhie-konstruktory/
// 1. Конструктор может быть вызван в списке инициализации
// 2. Важно, что конструктор ЛИБО инициализирует поля,
//    ЛИБО вызывает другой конструктор
// 3. Важно следить, чтобы не возникало циклического вызова
//    конструкторов (конструктор1 вызывает конструктор2,
//                   конструктор2 вызывает конструктор1)

// Правила генерации компилятором конструкторов и операторов присваивания:
//
// Конструктор по умолчанию: те же правила, что и в C++98. Генерируется только если класс
// не содержит определенных пользователем конструкторов.
//
// Деструктор: генерируется только если класс не содержит определённого пользователем
// деструктора; по умолчанию деструкторы объявляются как noexcept.
// Как и в C++98, виртуальный если деструктор базового класса также виртуальный.
//
// Копирующий конструктор: то же поведение, что и в C++98: вызов копирующего
// конструктора для всех нестатических членов. Генерируется только если в классе
// нет определенного пользователем копирующего конструктора. Удаляется, если класс
// определяет операцию перемещения. Генерация этой функции в классе с определенным
// пользователем копирующим оператором присваивания или деструктором считается устаревшей.
//
// Копирующее присваивание: то же поведение, что и в C++98: копирующее присваивание всех
// нестатических членов. Генерируется только если в классе нет определенного пользователем
// копирующего присваивания. Удаляется, если в классе присутствует определенная пользователем
// операция перемещения. Генерация в случае, если в классе есть определенный пользователем
// копирующий конструктор или деструктор, считается устаревшей (deprecated).
//
// Перемещающий конструктор и перемещающее присваивание: перемещают нестатические члены
// (соответственно конструктором или присваиванием). Генерируются только если класс
// не содержит определенных пользователем операций копирования, перемещения, или деструкторов.
//
// Примечание: шаблонные методы не участвуют в этих правилах, то есть не предотвращают
// генерацию специальных функций, даже если их специализации могут иметь подходящую форму.



// Question 6
// Операторы преобразования типов

// Хотим явно проинтерпретировать какую-то часть выражения,
// как тип, которым она не является.

// Способы:
// 1) C-style cast
// (type) (expression);
int a = 5;
double b = (double) a;

// Но спсобов преобразования существует много
// Например можно преобразовать int в float по умному, а можно просто считать,
// что те байты которые занимал int это теперь float. Понадобится может и то, и другое.
// Так вот эта адская машина перебирает все подряд пока какой-нибудь каст не подойдет.
// Начинает со static, потом навешивает на него const, потом reinterpret и тд
// Выдает CE только если даже сам дьявол не может сделать этот каст
// Еще его минусы как минимум раза два слушали от Деда
//
// 2) static_cast
// Называется static, потому что на этапе компилиции
// определяет как и по каким правилам будет cast, и не будет ли ошибки.
// Оператор просматривает все правила преобразования типов и по этим правилам
// преобразует переданный ему объект к тому типу, который мы попросили.
// Если такое преобразование не определено для стандартного типа или мы не определили
// для своего типа, то будет CE
double b = static_cast<double> (a);

// 3) reinterpret_cast
// Говорит компилятору, чтобы те биты, которые мы ему передали, он стал считать другим типом
// НО! Это работает не всегда
// Хороший кейс (на фоне остальных, а так конечно осуждаем любой каст): менять тип указателей
int *a = nullptr;
type *b = *(reinterpret_cast<type> (a)) // указатель типа type на a
// Если указатели не совместимы: этот тип занимал другое количество памяти, по другому размещался,
// если хоть что-то не так, эти типы не абсолютно совместимы, то это ваши проблемы. будет UB.

// 3) const_cast
// Единственный из всех кастов, который может кастовать константы к неконстантам
// Может и наоборот
// Пример использования:
int x = 5;
const int& y = x;
// Тут утекает много воды
int z = const_cast<int&> (y);
// Но мы помним что на самом деле то, что лежит под y
// можно менять и говорим об этом компилятору.
// Но если изначально там лежала константа, то будет UB

// Обратный пример:
int f(int& x);
int f(const int& x);

int a = 5;
// Хочу вызвать f(a), но ту, которая предназначена для const
f(const_cast<const int&> (a));


// Если вызвать static или reinterpret от const к не const или наоборот, то будет CE,
// они так не могут - сказал лектор, но на практике от не const к const кажется могут.


// Неявное приведение типов
// Иногда компилятор может сам преобразовать один тип в другой, когда это требуется
// Виды:
// 1) Числовое расшишерние
// 1.1) Интегральное расширение
// Целочисленный тип расширяется добавлением байт char -> int -> long long и тд
// 1.2) Расширение с плавающей точкой
// float -> double -> long double

// Расширение всегда безопасно и не приводит к потери данных

// 2) Числовые конверсии
// Конвертирование из большего типа в аналогичный меньший или между разными типами данных.
// Не всегда безопасно, возможна потеря данных


// Можно перегрузить C-Style cast для своего класса
// Пример каста к инту:
class C {
    operator int() {
        // Что-то, что вернет int
    }
};
// Это преобразование будет искать static_cast, если его использовать
// Проблема:
// После его определения может происходить неявное приведение типов,
// что не всегда хорошо, поэтому воизбежание этого появилось ключевое слово explicit,
// которое разрешает только явно использовать это преобразование.
explicit operator int() {}

// Question 9
// https://habr.com/ru/post/487920/#id-1-1

// Question 11
// https://habr.com/ru/post/266747/

// Question 13
template<int n>
struct fibonacci {
    static constexpr int value = fibonacci<n-1>::value + fibonacci<n-2>::value;
};
template<>
struct fibonacci<0> {
    static constexpr int value = 0;
};
template<>
struct fibonacci<1> {
    static constexpr int value = 1;
};

int main()
{
    fibonacci<40>::value;
    return 0;
}

// Question 23
// http://thbecker.net/articles/rvalue_references/section_03.html - лучше и не скажешь
// quite good exapmle of different behavior
#include <iostream>
#include <utility>

void f (int& a) {
    std::cout << "lvalue " << a << '\n';
}

void f (const int& a) {
    std::cout << "const lvalue " << a << '\n';
}

void f (int&& a) {
    std::cout << "rvalue " << a << '\n';
}

int main() {
    int x = 0;
    
    f (0);
    f (x);
    f (static_cast <const int&> (x));
    f (static_cast <const int> (x));
    f (std::move (x));
    
    return 0;
}

// Question 24 - смотри №23 и главу 5 Мейерса (эффективный и современный). Если нет ее - пиши Леше Кудринскому

// Question 25
// Кратко: https://habr.com/ru/post/441742/
// Жестко: https://habr.com/ru/post/322132/


// Question 29 (SFINAE и шаблонная магия)
// https://en.cppreference.com/w/cpp/language/sfinae
// "Substitution Failure Is Not An Error"!

namespace Question29 {

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* Простой пример проявления принципа SFINAE */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void f(int x) {
    std::cout << "int: " << x << '\n'; 
}

// для double не ясно, что такое double::type
// сигнатура функции не может быть определена
template<typename T>
typename T::type f(T) {            
    std::cout << "template \n";    
    return static_cast<typename T::type>(0); // каст не приводит к CE, т.к. тело  
}                                            // функции даже не инстанцируется

/*    template<typename T>
 *    int f(T) {
 *        typename T::type temp = 0; // привело бы к CE, т.к. сигнатура функции определена,
 *        return 0;                  // и ошибка найдена на этапе инстанцирования тела функции
 *    }
 */

void g() {
    f(3.1415926); // Несмотря на то, что предпочтительна
                  // шаблонная версия, вызовется специализация
                  // с неявным приведением к int
} // Ожидаемый вывод: "int: 3"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* Пример нетривиальной проверки с помощью SFINAE */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

template<typename T>
class is_default_constructible {

    template<typename...>
    static int check(...) { // -> int
        return 0;
    }// будет выбрано при отсутствии конструктора по-умолчанию

    template<typename Type>
    static decltype(Type(), char()) check(int) { // -> char
       return 0; 
    }// будет выбрано при наличии конструктора по-умолчанию
    
public:
    // std::is_same содержится в type_traits
    static const bool value = std::is_same<char, decltype(check<T>(0))>::value;
};

struct S {
    S() = delete;
};

void test() {
    std::cout << is_default_constructible<int>::value << ' '  <<
                 is_default_constructible<S>::value   << '\n';
} // Ожидаемый вывод: "1 0"

/* Много других проверок можно найти в type_traits */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/*              enable_if и enable_if_t               */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
// https://en.cppreference.com/w/cpp/types/enable_if

/* реализация enable_if */
template<bool B, class T = void>
struct enable_if {};
 
template<class T>
struct enable_if<true, T> { typedef T type; };

/* реализация enable_if_t */
template< bool B, class T = void >
using enable_if_t = typename enable_if<B,T>::type;

// Примеры использования: 
// https://riptutorial.com/ru/cplusplus/example/3777/enable-if

}

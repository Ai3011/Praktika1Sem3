#ifndef DATA_STRUCTURES_H // Защита от множественного включения
#define DATA_STRUCTURES_H

#include <string>
#include <type_traits>
#include <stdexcept> // Для работы с исключениями

using namespace std;

// Шаблон структуры для узла двусвязного списка
template <typename T>
struct NodeD {
    T data; // Данные, хранящиеся в узле
    NodeD* next = nullptr; // Указатель на следующий узел
    NodeD* prev = nullptr; // Указатель на предыдущий узел
};

// Шаблон структуры для двусвязного списка
template <typename T>
struct List {
    int length = 0; // Длина списка
    NodeD<T>* head = new NodeD<T>; // Указатель на "голову" списка
    NodeD<T>* tail = head; // Указатель на "хвост" списка (последний узел)

    // Метод добавления нового узла в конец списка
    void push(T data);
    
    // Перегрузка оператора [] для доступа к элементам списка
    T operator[](int index);

    // Метод удаления последнего узла списка
    T pop();

    // Метод получения элемента по индексу
    T get(int index);

    // Метод поиска индекса элемента
    int find(T data);

    // Метод удаления узла по индексу
    void del(int index);

    // Метод удаления узла по значению
    void remove(T data);

    // Метод вставки узла по индексу
    void insert(T data, int index);

    // Метод очистки списка
    void clear();
};

// Шаблон структуры для массива с логическим состоянием
template <typename T>
struct Array {
    struct Node { // Вложенная структура для элемента массива
        T value; // Значение
        bool state = false; // Логическое состояние: занято или нет
    };

    int arSize; // Размер массива
    Node* head; // Указатель на массив элементов

    // Конструктор массива
    Array(int size);

    // Метод установки значения по индексу
    void set(T data, int index);

    // Перегрузка оператора [] для доступа к элементам массива
    T operator[](int index);

    // Метод удаления значения по индексу
    void del(int index);

    // Метод поиска индекса элемента по значению
    int find(T data);

    // Метод удаления значения по его содержимому
    void remove(T data);

    // Деструктор для освобождения памяти
    ~Array();
};

// Шаблон структуры для узла хэш-таблицы
template <typename T>
struct NodeH {
    string key; // Ключ
    T value; // Значение
    bool state = false; // Состояние: занято или нет
    bool deleted = false; // Состояние: удалено или нет
};

// Шаблон структуры для хэш-таблицы
template <typename T>
struct HashTable {
    int size = 8; // Начальный размер хэш-таблицы
    NodeH<T>* arr = new NodeH<T>[size]; // Массив узлов хэш-таблицы

    // Деструктор для освобождения памяти
    ~HashTable();

    // Хэш-функция для вычисления индекса
    int hashFunc(string key);

    // Метод для добавления элемента в хэш-таблицу
    void Add(string key, T value);

    // Метод для изменения размера хэш-таблицы
    void Resize();

    // Метод для получения значения по ключу
    T Get(string key);

    // Метод для удаления элемента по ключу
    void Remove(string key);
};

#endif // DATA_STRUCTURES_H

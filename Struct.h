#include <string> // Подключаем библиотеку для работы со строками
#include <type_traits> // Подключаем библиотеку для работы с типами

using namespace std; // Используем пространство имен std для удобства

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
    void push(T data) {
        NodeD<T>* new_node = new NodeD<T>{ data, nullptr, tail }; // Создаем новый узел
        tail->next = new_node; // Указываем, что предыдущий узел указывает на новый узел
        tail = new_node; // Обновляем указатель на "хвост"
        length += 1; // Увеличиваем длину списка
    }

    // Перегрузка оператора [] для доступа к элементам списка
    T operator[](int index) {
        return get(index); // Возвращаем элемент по индексу
    }

    // Метод удаления последнего узла списка
    T pop() {
        if (length == 0) throw runtime_error("List is empty"); // Проверка на пустой список

        NodeD<T>* last = tail; // Запоминаем последний узел
        T data = last->data; // Сохраняем данные последнего узла
        tail->prev->next = nullptr; // Обнуляем указатель на следующий узел у предпоследнего узла
        tail = tail->prev; // Обновляем указатель на "хвост"

        length--; // Уменьшаем длину списка
        delete last; // Удаляем последний узел
        return data; // Возвращаем данные последнего узла
    }

    // Метод получения элемента по индексу
    T get(int index) {
        if (index >= length || index < 0) throw runtime_error("Out of range"); // Проверка на выход за границы

        NodeD<T>* tek = head->next; // Начинаем с первого узла (после "головы")
        while (index != 0) {
            tek = tek->next; // Перемещаемся к следующему узлу
            index--; // Уменьшаем индекс
        }
        return tek->data; // Возвращаем данные узла
    }

    // Метод поиска индекса элемента
    int find(T data) {
        for (int index = 0; index < length; index++) {
            if (get(index) == data) { // Если нашли элемент
                return index; // Возвращаем индекс
            }
        }
        return -1; // Если элемент не найден, возвращаем -1
    }

    // Метод удаления узла по индексу
    void del(int index) {
        if (index >= length || index < 0) throw runtime_error("Out of range"); // Проверка на выход за границы
        if (index == length - 1) pop(); // Если последний элемент, вызываем pop

        NodeD<T>* tek = head->next; // Начинаем с первого узла
        while (index != 0) {
            tek = tek->next; // Перемещаемся к узлу по индексу
            index--;
        }

        NodeD<T>* pred = tek->prev; // Запоминаем предыдущий узел
        pred->next = tek->next; // Обновляем указатель следующего узла у предыдущего узла
        pred->next->prev = pred; // Обновляем указатель предыдущего узла у следующего узла

        delete tek; // Удаляем текущий узел
        length--; // Уменьшаем длину списка
    }

    // Метод удаления узла по значению
    void remove(T data) {
        int index = find(data); // Находим индекс

        if (index == -1) throw runtime_error("No value"); // Если не нашли, выбрасываем исключение

        del(index); // Удаляем по индексу
    }

    // Метод вставки узла по индексу
    void insert(T data, int index) {
        if (index > length || index < 0) throw runtime_error("Out of range"); // Проверка на выход за границы

        if (index == length) { // Если вставляем в конец
            push(data); // Вызываем push
        }
        else if (index >= 0) { // Если вставляем не в конец
            NodeD<T>* new_node = new NodeD<T>; // Создаем новый узел
            new_node->data = data; // Заполняем данные
            NodeD<T>* tek = head->next; // Начинаем с первого узла

            while (index != 0) {
                tek = tek->next; // Перемещаемся к узлу по индексу
                index--;
            }

            new_node->prev = tek->prev; // Обновляем указатель на предыдущий узел
            tek->prev->next = new_node; // Указываем, что предыдущий узел указывает на новый
            tek->prev = new_node; // Указываем, что новый узел указывает на текущий узел
            new_node->next = tek; // Указываем, что новый узел указывает на текущий узел
            length++; // Увеличиваем длину списка
        }
    }

    // Метод очистки списка
    void clear() {
        NodeD<T>* prev; // Указатель на предыдущий узел
        NodeD<T>* tek = tail; // Начинаем с "хвоста"
        while (tek != head) { // Проходим пока не дойдем до "головы"
            prev = tek->prev; // Запоминаем предыдущий узел
            delete tek; // Удаляем текущий узел
            tek = prev; // Переходим к предыдущему узлу
        }
        delete tek; // Удаляем "голову"
    }
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
    Array(int size) {
        arSize = size; // Устанавливаем размер массива
        head = new Node[size]; // Создаем массив
    }

    // Метод установки значения по индексу
    void set(T data, int index) {
        if (index >= arSize) throw runtime_error("Out of range"); // Проверка на выход за границы
        head[index].value = data; // Устанавливаем значение
        head[index].state = true; // Устанавливаем состояние как занятое
    }

    // Перегрузка оператора [] для доступа к элементам массива
    T operator[](int index) {
        if (index >= arSize) throw runtime_error("Out of range"); // Проверка на выход за границы
        if (!head[index].state) throw runtime_error("No value"); // Проверка на занятое состояние
        return head[index].value; // Возвращаем значение
    }

    // Метод удаления значения по индексу
    void del(int index) {
        if (index >= arSize) throw runtime_error("Out of range"); // Проверка на выход за границы
        if (!head[index].state) throw runtime_error("No value"); // Проверка на занятое состояние
        head[index].state = false; // Устанавливаем состояние как не занятое
    }

    // Метод поиска индекса элемента по значению
    int find(T data) {
        for (int i = 0; i < arSize; i++) {
            if (!head[i].state) continue; // Пропускаем не занятые элементы

            if (head[i].value == data) { // Если нашли элемент
                return i; // Возвращаем индекс
            }
        }
        return -1; // Если не нашли, возвращаем -1
    }

    // Метод удаления значения по его содержимому
    void remove(T data) {
        int index = find(data); // Находим индекс
        if (index == -1) throw runtime_error("No value"); // Если не нашли, выбрасываем исключение

        head[index].state = false; // Устанавливаем состояние как не занятое
    }

    // Деструктор для освобождения памяти
    ~Array() {
        delete[] head; // Удаляем массив
    }
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
    ~HashTable() {
        delete[] arr; // Удаляем массив узлов
    }

    // Хэш-функция для вычисления индекса
    int hashFunc(string key) {
        int hash_result = 0; // Переменная для хранения хэш-результата
        for (int i = 0; i < key.size(); i++) {
            hash_result = ((size - 1) * hash_result + key[i]) % size; // Обновляем хэш-результат
            hash_result = (hash_result * 2 + 1) % size; // Увеличиваем хэш-результат
        }
        return hash_result; // Возвращаем хэш-результат
    }

    // Метод для добавления элемента в хэш-таблицу
    void Add(string key, T value) {
        int index = hashFunc(key); // Получаем индекс с помощью хэш-функции

        while (index < size) { // Проходим по массиву, начиная с вычисленного индекса
            if (!arr[index].state) { // Если ячейка не занята
                arr[index].key = key; // Устанавливаем ключ
                arr[index].value = value; // Устанавливаем значение
                arr[index].state = true; // Устанавливаем состояние как занятое
                return; // Выходим из метода
            }
            else if (arr[index].key == key) { // Если ключ совпадает
                arr[index].value = value; // Обновляем значение
                return; // Выходим из метода
            }
            index++; // Переходим к следующему индексу
        }

        Resize(); // Увеличиваем размер массива, если не нашли свободное место
        Add(key, value); // Повторно вызываем метод Add для добавления элемента
    }

    // Метод для изменения размера хэш-таблицы
    void Resize() {
        size *= 2; // Увеличиваем размер вдвое
        NodeH<T>* oldArr = arr; // Запоминаем старый массив
        arr = new NodeH<T>[size]; // Создаем новый массив

        // Перемещаем старые элементы в новый массив
        for (int i = 0; i < size / 2; i++) {
            if (oldArr[i].state && !oldArr[i].deleted) { // Если элемент существует и не удален
                Add(oldArr[i].key, oldArr[i].value); // Добавляем элемент в новый массив
            }
        }

        delete[] oldArr; // Удаляем старый массив
    }

    // Метод для получения значения по ключу
    T Get(string key) {
        int index = hashFunc(key); // Получаем индекс с помощью хэш-функции

        while (index < size) { // Проходим по массиву, начиная с вычисленного индекса
            if (arr[index].state && arr[index].key == key) { // Если нашли ключ
                return arr[index].value; // Возвращаем значение
            }
            else if (arr[index].key == key && arr[index].deleted) throw runtime_error("No such key"); // Если ключ удален, выбрасываем исключение
            else if (!arr[index].deleted && !arr[index].state) throw runtime_error("No such key"); // Если ячейка пустая и не удалена, выбрасываем исключение
            index++; // Переходим к следующему индексу
        }

        throw runtime_error("No such key"); // Если не нашли ключ, выбрасываем исключение
    }

    // Метод для удаления элемента по ключу
    void Remove(string key) {
        int index = hashFunc(key); // Получаем индекс с помощью хэш-функции
        while (index < size) { // Проходим по массиву, начиная с вычисленного индекса
            if (arr[index].state && arr[index].key == key) { // Если нашли ключ
                if (is_same<T, List<string>>::value) arr[index].value.clear(); // Если значение - список, очищаем его
                arr[index].state = false; // Устанавливаем состояние как не занятое
                arr[index].deleted = true; // Устанавливаем состояние как удаленное
                return; // Выходим из метода
            }
            else if (arr[index].key == key && arr[index].deleted) throw runtime_error("No such key"); // Если ключ удален, выбрасываем исключение
            else if (!arr[index].deleted && !arr[index].state) throw runtime_error("No such key"); // Если ячейка пустая и не удалена, выбрасываем исключение
            index++; // Переходим к следующему индексу
        }
        throw runtime_error("No such key"); // Если не нашли ключ, выбрасываем исключение
    }
};

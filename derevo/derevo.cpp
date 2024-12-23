#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip> // Для std::setw

// Определяем структуру для хранения информации о группе
struct GroupNumber {
    char type; // Тип (бакалавриат 'B', магистратура 'M', специалитет 'S')
    int number; // Номер группы (4-значный)
};

// Определяем структуру для узла дерева
struct TreeNode {
    GroupNumber key; // Ключ узла
    TreeNode* left;  // Указатель на левое поддерево
    TreeNode* right; // Указатель на правое поддерево
    int balanceFactor; // Коэффициент балансировки
    std::vector<int> lineNumbers; // Список номеров строк, где встречался элемент

    // Конструктор для инициализации узла
    TreeNode(GroupNumber k, int lineNumber) : key(k), left(nullptr), right(nullptr), balanceFactor(0) {
        lineNumbers.push_back(lineNumber);
    }
};
// Определяем класс для AVL-дерева
class AVLTree {
public:
    AVLTree() : root(nullptr) {} // Конструктор, инициализирующий корень дерева как nullptr

    void insert(GroupNumber key, int lineNumber) { // Метод для вставки нового элемента
        root = insert(root, key, lineNumber); // Вставляем элемент в дерево, начиная с корня
    }

    void remove(GroupNumber key) { // Метод для удаления элемента
        root = remove(root, key); // Удаляем элемент из дерева, начиная с корня
    }

    TreeNode* search(GroupNumber key) { // Метод для поиска элемента
        return search(root, key); // Ищем элемент, начиная с корня
    }

    void print() const { // Метод для печати дерева
        int height = getHeight(root);
        print(root, 0, height); // Печатаем дерево, начиная с корня
    }

    void preOrderTraversal(std::ostream& out) { // Метод для прямого обхода дерева
        preOrderTraversal(root, out); // Обходим дерево, начиная с корня
    }

    void clear() { // Метод для очистки дерева
        clear(root); // Освобождаем память, начиная с корня
        root = nullptr; // Устанавливаем корень как nullptr
    }

    // Метод для загрузки данных из файла
    void loadFromFile(const std::string& filename) {
        std::ifstream inFile(filename); // Открываем файл для чтения
        if (!inFile) { // Проверяем, удалось ли открыть файл
            std::cerr << "Ошибка при открытии файла " << filename << std::endl; // Сообщаем об ошибке
            return; // Выходим из метода
        }

        std::string line; // Переменная для хранения строки из файла
        int lineNumber = 0; // Счетчик строк
        while (std::getline(inFile, line)) { // Читаем файл построчно
            lineNumber++; // Увеличиваем номер строки
            if (line.length() != 5 ||
                (line[0] != 'B' && line[0] != 'M' && line[0] != 'S') ||
                !std::isdigit(line[1]) || !std::isdigit(line[2]) || !std::isdigit(line[3]) || !std::isdigit(line[4])) {
                std::cerr << "Ошибка в строке " << lineNumber << ": некорректный формат строки." << std::endl;
                continue; // Пропускаем некорректную строку
            }

            GroupNumber group; // Создаем переменную для хранения группы
            group.type = line[0]; // Извлекаем тип группы из первой буквы строки
            group.number = std::stoi(line.substr(1, 4)); // Извлекаем 4-цифровый номер группы

            // Проверка на значение номера группы
            if (group.number < 1000 || group.number > 9999) {
                std::cerr << "Ошибка в строке " << lineNumber << ": номер группы должен быть 4-значным числом." << std::endl;
                continue; // Пропускаем некорректную строку
            }

            insert(group, lineNumber); // Добавляем элемент в дерево
        }

        inFile.close(); // Закрываем файл
    }

    // Метод для сохранения данных в файл
    void saveToFile(const std::string& filename) {
        std::ofstream outFile(filename); // Открываем файл для записи
        if (!outFile) { // Проверяем, удалось ли открыть файл
            std::cerr << "Ошибка при открытии файла " << filename << std::endl; // Сообщаем об ошибке
            return; // Выходим из метода
        }
        preOrderTraversal(outFile); // Записываем дерево в файл
        outFile.close(); // Закрываем файл
    }

private:
    TreeNode* root; // Указатель на корень дерева

    // Метод для получения коэффициента балансировки узла
    int balanceFactor(TreeNode* node) {
        return node ? (height(node->left) - height(node->right)) : 0; // Разница высот левого и правого поддеревьев
    }

    // Метод для получения высоты узла
    int height(TreeNode* node) {
        if (!node) return 0; // Если узел пуст, высота 0
        return std::max(height(node->left), height(node->right)) + 1; // Высота узла
    }

    // Метод для правого вращения узла
    TreeNode* rotateRight(TreeNode* y) {
        TreeNode* x = y->left; // Сохраняем левое поддерево
        TreeNode* T2 = x->right; // Сохраняем правое поддерево левого узла

        x->right = y; // Поворачиваем узел вправо
        y->left = T2; // Присоединяем правое поддерево к левому узлу

        // Обновляем коэффициенты балансировки
        y->balanceFactor = balanceFactor(y);
        x->balanceFactor = balanceFactor(x);

        return x; // Возвращаем новый корень поддерева
    }

    // Метод для левого вращения узла
    TreeNode* rotateLeft(TreeNode* x) {
        TreeNode* y = x->right; // Сохраняем правое поддерево
        TreeNode* T2 = y->left; // Сохраняем левое поддерево правого узла

        y->left = x; // Поворачиваем узел влево
        x->right = T2; // Присоединяем левое поддерево к правому узлу

        // Обновляем коэффициенты балансировки
        x->balanceFactor = balanceFactor(x);
        y->balanceFactor = balanceFactor(y);

        return y; // Возвращаем новый корень поддерева
    }

    // Метод для вставки узла в дерево
    TreeNode* insert(TreeNode* node, GroupNumber key, int lineNumber) {
        if (!node) return new TreeNode(key, lineNumber); // Если узел пуст, создаем новый узел

        // Сравниваем ключи и вставляем в соответствующее поддерево
        if (key.type < node->key.type || (key.type == node->key.type && key.number < node->key.number)) {
            node->left = insert(node->left, key, lineNumber); // Вставляем в левое поддерево
        }
        else if (key.type > node->key.type || (key.type == node->key.type && key.number > node->key.number)) {
            node->right = insert(node->right, key, lineNumber); // Вставляем в правое поддерево
        }
        else {
            // Если элемент уже существует, добавляем номер строки в список
            if (std::find(node->lineNumbers.begin(), node->lineNumbers.end(), lineNumber) == node->lineNumbers.end()) {
                node->lineNumbers.push_back(lineNumber);
            }
            return node; // Элемент уже существует
        }

        // Обновляем коэффициент балансировки
        node->balanceFactor = balanceFactor(node);

        // Балансировка дерева
        if (node->balanceFactor > 1 && (key.type < node->left->key.type || (key.type == node->left->key.type && key.number < node->left->key.number))) {
            return rotateRight(node); // LL
        }
        if (node->balanceFactor < -1 && (key.type > node->right->key.type || (key.type == node->right->key.type && key.number > node->right->key.number))) {
            return rotateLeft(node); // RR
        }
        if (node->balanceFactor > 1 && (key.type > node->left->key.type || (key.type == node->left->key.type && key.number > node->left->key.number))) {
            node->left = rotateLeft(node->left); // LR
            return rotateRight(node);
        }
        if (node->balanceFactor < -1 && (key.type < node->right->key.type || (key.type == node->right->key.type && key.number < node->right->key.number))) {
            node->right = rotateRight(node->right); // RL
            return rotateLeft(node);
        }

        return node; // Возвращаем узел
    }

    // Метод для удаления узла из дерева
    TreeNode* remove(TreeNode* root, GroupNumber key) {
        if (!root) return root; // Если узел пуст, возвращаем его

        // Сравниваем ключи и удаляем из соответствующего поддерева
        if (key.type < root->key.type || (key.type == root->key.type && key.number < root->key.number)) {
            root->left = remove(root->left, key); // Удаляем из левого поддерева
        }
        else if (key.type > root->key.type || (key.type == root->key.type && key.number > root->key.number)) {
            root->right = remove(root->right, key); // Удаляем из правого поддерева
        }
        else {
            // Узел с одним дочерним элементом или без них
            if (!root->left || !root->right) {
                TreeNode* temp = root->left ? root->left : root->right; // Узел, который заменит удаляемый

                if (!temp) { // Если нет дочерних узлов
                    temp = root; // Удаляемый узел
                    root = nullptr; // Узел удален
                }
                else { // Копируем содержимое
                    *root = *temp;
                }

                delete temp; // Освобождаем память
            }
            else {
                // Узел с двумя дочерними элементами
                TreeNode* temp = getMinValueNode(root->right); // Находим минимальный узел в правом поддереве
                root->key = temp->key; // Копируем значение
                root->right = remove(root->right, temp->key); // Удаляем узел
            }
        }

        // Если дерево стало пустым
        if (!root) return root;

        // Обновление коэффициента балансировки
        root->balanceFactor = balanceFactor(root);

        // Балансировка дерева
        if (root->balanceFactor > 1 && balanceFactor(root->left) >= 0)
            return rotateRight(root); // LL
        if (root->balanceFactor > 1 && balanceFactor(root->left) < 0) {
            root->left = rotateLeft(root->left); // LR
            return rotateRight(root);
        }
        if (root->balanceFactor < -1 && balanceFactor(root->right) <= 0)
            return rotateLeft(root); // RR
        if (root->balanceFactor < -1 && balanceFactor(root->right) > 0) {
            root->right = rotateRight(root->right); // RL
            return rotateLeft(root);
        }

        return root; // Возвращаем узел
    }

    // Метод для получения узла с минимальным значением
    TreeNode* getMinValueNode(TreeNode* node) {
        TreeNode* current = node; // Начинаем с данного узла
        while (current && current->left) current = current->left; // Идем до самого левого узла
        return current; // Возвращаем узел
    }

    // Метод для поиска узла в дереве
    TreeNode* search(TreeNode* node, GroupNumber key) {
        if (node == nullptr || (node->key.type == key.type && node->key.number == key.number))
            return node; // Если узел пуст или ключ совпадает, возвращаем узел

        // Сравниваем ключи и ищем в соответствующем поддереве
        if (key.type < node->key.type || (key.type == node->key.type && key.number < node->key.number))
            return search(node->left, key); // Поиск в левом поддереве

        return search(node->right, key); // Поиск в правом поддереве
    }

    // Метод для получения высоты дерева
    int getHeight(TreeNode* node) const {
        if (!node) return 0;
        return std::max(getHeight(node->left), getHeight(node->right)) + 1;
    }

    // Метод для печати дерева
    void print(TreeNode* node, int indent, int height) const {
        if (node) {
            // Печатаем правое поддерево
            if (node->right) {
                print(node->right, indent + 4, height);
            }
            // Печатаем текущий узел
            if (indent) {
                std::cout << std::setw(indent) << ' '; // Отступ
            }
            std::cout << node->key.type << node->key.number; // Печатаем узел
            // Печатаем номера строк
            if (!node->lineNumbers.empty()) {
                std::cout << " (строки: ";
                for (size_t i = 0; i < node->lineNumbers.size(); ++i) {
                    std::cout << node->lineNumbers[i];
                    if (i < node->lineNumbers.size() - 1) {
                        std::cout << ", ";
                    }
                }
                std::cout << ")";
            }
            std::cout << std::endl; // Переход на новую строку
            // Печатаем левое поддерево
            if (node->left) {
                print(node->left, indent + 4, height);
            }
        }
    }

    // Метод для прямого обхода дерева
    void preOrderTraversal(TreeNode* node, std::ostream& out) {
        if (node) {
            out << node->key.type << node->key.number << " (строки: ";
            for (const auto& line : node->lineNumbers) {
                out << line << " ";
            }
            out << ")" << std::endl; // Запись узла
            preOrderTraversal(node->left, out); // Обход левого поддерева
            preOrderTraversal(node->right, out); // Обход правого поддерева
        }
    }

    // Метод для очистки дерева
    void clear(TreeNode* node) {
        if (node) {
            clear(node->left); // Освобождаем левое поддерево
            clear(node->right); // Освобождаем правое поддерево
            delete node; // Освобождение памяти
        }
    }
};

void showMenu() {
    std::cout << "Выберите действие:\n";
    std::cout << "1. Инициализация (пустое дерево)\n";
    std::cout << "2. Добавление нового элемента\n";
    std::cout << "3. Удаление заданного элемента\n";
    std::cout << "4. Поиск заданного элемента\n";
    std::cout << "5. Печать дерева\n";
    std::cout << "6. Прямой обход дерева\n";
    std::cout << "7. Освобождение памяти (удаление всего дерева)\n";
    std::cout << "0. Выход\n";
}

int main() {
    setlocale(LC_ALL, "Russian");
    AVLTree tree; // Создание объекта дерева
    tree.loadFromFile("input.txt"); // Автоматическая загрузка данных из файла при старте

    int choice; // Переменная для выбора действия

    do {
        showMenu(); // Показать меню
        std::cin >> choice; // Ввод пользователем выбора

        switch (choice) { // В зависимости от выбора выполняем соответствующее действие
        case 1:
            tree.clear(); // Очистка дерева (инициализация пустого дерева)
            std::cout << "Дерево инициализировано.\n"; // Сообщаем об инициализации
            break;

        case 2: { // Добавление нового элемента
            GroupNumber group; // Создаем переменную для группы
            std::cout << "Введите тип (B/M/S) и номер группы (4 цифры): "; // Запрашиваем ввод пользователя
            std::cin >> group.type >> group.number; // Считываем тип и номер группы
            // Проверка на корректность ввода
            if (group.number < 1000 || group.number > 9999 || (group.type != 'B' && group.type != 'M' && group.type != 'S')) {
                std::cout << "Некорректный ввод!" << std::endl; // Сообщаем об ошибке
            }
            else {
                tree.insert(group, 0); // Добавить элемент
                std::cout << "Элемент добавлен.\n";
            }
            break;
        }
        case 3: { // Удаление заданного элемента
            GroupNumber group; // Создаем переменную для группы
            std::cout << "Введите тип (B/M/S) и номер группы (4 цифры) для удаления: "; // Запрашиваем ввод
            std::cin >> group.type >> group.number; // Считываем данные
            // Проверка на корректность ввода
            if (group.number < 1000 || group.number > 9999 || (group.type != 'B' && group.type != 'M' && group.type != 'S')) {
                std::cout << "Некорректный ввод!" << std::endl; // Сообщаем об ошибке
            }
            else {
                tree.remove(group); // Удалить элемент
                std::cout << "Элемент удалён.\n";
            }
            break;
        }
        case 4: { // Поиск заданного элемента
            GroupNumber group; // Создаем переменную для группы
            std::cout << "Введите тип (B/M/S) и номер группы (4 цифры) для поиска: "; // Запрашиваем ввод
            std::cin >> group.type >> group.number; // Считываем данные
            // Проверка на корректность ввода
            if (group.number < 1000 || group.number > 9999 || (group.type != 'B' && group.type != 'M' && group.type != 'S')) {
                std::cout << "Некорректный ввод!" << std::endl; // Сообщаем об ошибке
            }
            else {
                TreeNode* result = tree.search(group); // Поиск элемента
                if (result) {
                    std::cout << "Элемент найден: " << result->key.type << result->key.number << " (строки: ";
                    for (const auto& line : result->lineNumbers) {
                        std::cout << line << " ";
                    }
                    std::cout << ")" << std::endl; // Сообщаем о найденном элементе
                }
                else {
                    std::cout << "Элемент не найден.\n";
                }
            }
            break;
        }
        case 5: // Печать дерева
            std::cout << "Дерево:\n"; // Сообщаем, что будет печататься дерево
            tree.print(); // Печатаем дерево
            break;
        case 6: // Прямой обход дерева
            std::cout << "Прямой обход дерева:\n"; // Сообщаем о прямом обходе
            tree.preOrderTraversal(std::cout); // Прямой обход дерева (посредством preOrderTraversal)
            break;
        case 7: // Освобождение памяти
            tree.clear(); // Освобождение памяти
            std::cout << "Дерево очищено.\n";
            break;
        case 0: // Выход из программы
            tree.saveToFile("output.txt"); // Сохранение данных в файл перед выходом
            std::cout << "Данные сохранены в файл output.txt.\n"; // Сообщаем о сохранении данных
            std::cout << "Выход из программы.\n";
            break;
        default: // Неправильный ввод
            std::cout << "Некорректный выбор, попробуйте снова.\n";
        }

    } while (choice != 0); // Продолжать до выбора выхода 

    tree.clear(); // Освобождение памяти перед выходом
    return 0;
}
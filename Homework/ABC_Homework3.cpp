/* Вариант 27:
 *
 * Пляшущие человечки.
 * На тайном собрании глав преступного мира города
 * Лондона председатель собрания профессор Мориарти
 * постановил: отныне вся переписка между преступниками
 * должна вестись тайнописью. В качестве стандарта
 * были выбраны "пляшущие человечки", шифр, в котором
 * каждой букве латинского алфавита соответствует
 * хитроумный значок. Реализовать многопоточное при-
 * ложение, шифрующее исходный текст (в качестве ключа
 * используется кодовая таблица, устанавливающая одноз-
 * начное соответствие между каждой буквой и каким-нибудь
 * числом). Каждый поток шифрует свои кусочки текста.
 * При решении использовать парадигму портфеля задач.
 *
 * ФИО: Шалаева Марина Андреевна
 * Группа: БПИ191
 */

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <ctime>
#include <thread>
#include <algorithm>

 // Имя входного файла.
std::string input;
// Имя выходного файла.
std::string output;
// Имя выходного файла для букв и их числовых значений.
std::string output_alphabet;

// Переменная для считанного из файла текста (наше портфолио).
std::string text;
// Строковый массив для записи закодированных переменных.
std::string* encoded_text{};
// Словарь (карта) для хранения строчных букв латинского алфавита и их численных значений.
std::map<char, short> alphabet;
// Вспомогательный массив, индексы элементов которого служат значениями букв.
short numbers[90] = { 0 };

// Индекс для работы потоков.
int index = 0;
// Количество потоков.
int numberOfThreads;
// Количество символов, которое кодирует один поток.
int numberOfLetters;


/// <summary>
/// Функция, проверяющая, является ли
/// входной параметр натуральным числом.
/// </summary>
/// <param name="s"> - входная строка</param>
/// <returns>является ли строка числом</returns>
bool isNumber(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(),
        s.end(),
        [](char c) { return !std::isdigit(c); })
        == s.end();
}

/// <summary>
/// Функция, дополняющая название файла до
/// полного пути к нему.
/// </summary>
void addingFullPath() {

    if (input.find("files\\input\\") == std::string::npos)
        input = "files\\input\\" + input;
    if (output.find("files\\output\\") == std::string::npos)
        output = "files\\output\\" + output;
    if (output_alphabet.find("files\\output_alphabet\\") == std::string::npos)
        output_alphabet = "files\\output_alphabet\\" + output_alphabet;

}

/// <summary>
/// Фукция для пересоздания выходного
/// файла, на случай если тот уже создан.
/// </summary>
void createNewOutputFile() {
    std::ofstream out;
    out.open(output, std::ios::out);
    out << "";
    out.close();
}

/// <summary>
/// Функция для заполнения файла
/// данными из словаря (карты).
/// </summary>
void fillAlphabetFiles() {

    std::ofstream out;
    out.open(output_alphabet, std::ios::out);

    out << "--------\n";
    for (auto& item : alphabet) {
        out << " " << item.first << " | " << item.second << "\n--------\n";
    }
    out.close();
}

/// <summary>
/// Функция для заполнения словаря (карты)
/// строчными буквами латинского алфавита
/// (ключи) и случайными числами от 10
/// до 99 (значения).
/// </summary>
void fillAlphabet() {

    for (size_t i = 0; i < 26; i++) {

        srand((unsigned int)time(NULL));

        int index_of_numbers_array = rand() % 90;

        if (numbers[index_of_numbers_array] == 0) {
            alphabet[(char)(i + 97)] = index_of_numbers_array + 10;
            numbers[index_of_numbers_array] = 1;
        }
        else
            i--;
    }
    fillAlphabetFiles();
}

/// <summary>
/// Функция для считывания входных
/// данных из файла.
/// </summary>
void readFromFile() {
    std::ifstream in;
    in.open(input, std::ios::in);

    if (!in.is_open()) {
        std::cout << "Opening of the file failed!\n";
    }
    else {
        char x;
        text = "";
        while ((x = in.get()) != EOF) {
            text += tolower(x);
        }
    }
    in.close();
}


/// <summary>
/// Функция для работы одного потока,
/// кодирующая часть текста, соответствующую
/// переданному индексу.
/// </summary>
/// <param name="i"> - индекс потока</param>
/// <returns>закодированный текст</returns>
void encode(int i) {

    for (int j = i * numberOfLetters; j < i * numberOfLetters + numberOfLetters; ++j) {
        if (j >= text.size())
            break;

        std::map<char, short>::iterator it;
        char letter = text[j];
        it = alphabet.find(letter);

        if (it == alphabet.end())
            encoded_text[j] = (char)(letter);
        else
            encoded_text[j] = std::to_string(it->second);
    }

}

/// <summary>
/// Функция для распределения задач по
/// различным потокам.
/// </summary>
void launchPortfolio() {

    numberOfLetters = ((int)text.size() / numberOfThreads) + 1;
    if (numberOfThreads > text.size()) {
        /* Если введенное количество потоков
         * меньше количества символов в тексте,
         * то приравниваем количество потоков
         * к размеру текста. */
        std::cout << "The number of threads is bigger than number of\n"
            "letters in the text! Program will use "
            << text.size() << "\nthreads instead of "
            << numberOfThreads;
        numberOfLetters = 1;
        numberOfThreads = text.size();
    }
    /* Инициализируем динамический массив,
     * в который мы запишем закодированные
     * буквы, используя соответствующие
     * индексы. */
    encoded_text = new std::string[text.size()];

    // Инициализируем вектор потоков.
    std::vector<std::thread> threads(numberOfThreads);
    /* Цикл, вызывающий функцию для работы каждого
     * потока. В качестве аргумента подается index,
     * увеличивающий свое значение на каждой итерации. */
    for (int i = 0; i != numberOfThreads; ++index, ++i) {
        threads[i] = std::thread(encode, index);
    }
    for (int i = 0; i != numberOfThreads; ++i) {
        threads[i].join();
    }

    std::ofstream out;
    out.open(output, std::ios::app);
    // Запись задокированного текста в файл.
    for (int i = 0; i < text.size(); ++i) {
        out << encoded_text[i];
    }
    out.close();

    // Удаление динамического массива.
    delete[]encoded_text;
}

int main(int argc, char* argv[]) {

    /* Если количество введенных параметров меньше
    * нужного количества, сообщаем об этом пользователю
    * и завершаем работу приложения. */
    if (argc <= 4) {
        std::cout << "An invalid format of input params!\n"
            "Use the name of input file as the first value,\n"
            "the name of output file as the second one, the\n"
            "name of output file for letters and its codes as\n"
            "the third one and number of threads as the forth one.\n";
        exit(0);
    }

    if (!isNumber(argv[4])) {
        std::cout << "An invalid format of input params!\n"
            "Number of threads should be integer "
            "number!";
        exit(0);
    }

    if (atoi(argv[4]) <= 0) {
        std::cout << "An invalid format of input params!\n"
            "Number of threads should be integer "
            "number!";
        exit(0);
    }

    input = argv[1];
    output = argv[2];
    output_alphabet = argv[3];
    numberOfThreads = atoi(argv[4]);

    addingFullPath();
    createNewOutputFile();
    fillAlphabet();
    readFromFile();
    launchPortfolio();

    return 0;
}

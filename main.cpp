#include <iostream>
#include <map>
#include <fstream>
#include "thread"
#include <ctime>

// Метод непосредственно конвертирования строки в шифр по словарю
std::string convert(const std::string& str, const std::map<char, char>& dict) {
    std::string result;
    for (char i : str) {
        result += dict.find(i)->second;
    }
    return result;
}

// Метод для потока (void), куда передаем строку для изменения, затем ее возвращаем
void cipher(const std::string& str, const std::map<char, char>& dict, std::string& res) {
    res = convert(str, dict);
}

int main(int argc, char* argv[]) {

    clock_t begin = clock();

    if (argc != 4) {
        std::cout << "Please, put 3 arguments in the command line!" << "\n";
        return 1;
    }

    // Формирование словаря
    std::map<char, char> dict;
    // Все символы, которые есть исходном файле
    auto* symbols = new char[10000];
    char letter;
    std::ifstream file(argv[1]);
    // Количество символов
    int symbols_number = 0;
    // Посимвольное считывание файла
    for (file >> letter; !file.eof(); file >> letter) {
        symbols[symbols_number] = letter;
        ++symbols_number;
    }
    // Непосредственно формирование словаря
    for (int i = 0; i < symbols_number; ++i) {
        dict.insert({symbols[i], symbols[i + 1]});
        i += 1;
    }
    file.close();
    delete[] symbols;

    // Считывание файла с сообщением
    std::string message;
    std::ifstream text(argv[2]);
    // Количество слов в сообщении
    int wordsNumber = 0;
    // Массив слов
    auto* words = new std::string[10000];
    // Построковое считывание файла
    for (text >> message; !text.eof(); text >> message) {
        std::transform(message.begin(), message.end(), message.begin(), tolower);
        words[wordsNumber] = message;
        ++wordsNumber;
    }
    text.close();

    // Преобразование в шифр
    // Массив зашифрованных слов
    auto* new_words = new std::string[wordsNumber];
    // Массив вызываемых потоков
    auto* threads = new std::thread[wordsNumber];
    // Вызываем поток, куда передаем функцию шифрования с аргументами и строку для изменения,
    // куда записывается результат
    for (int i = 0; i < wordsNumber; ++i) {
        threads[i] = std::thread(cipher, words[i], dict, std::ref(new_words[i]));
    }
    // Вывод результата
    std::ofstream outfile(argv[3]);
    for (int i = 0; i < wordsNumber; ++i) {
        // Ждем, если поток еще не завершил работу
        threads[i].join();
        outfile << new_words[i] << " ";
    }
    clock_t end = clock();
    outfile.close();

    std::cout << double(end - begin) / 1000000;

    // Очистка памяти
    delete[] words;
    delete[] new_words;
    delete[] threads;
}
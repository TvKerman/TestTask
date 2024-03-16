#pragma once

#include <fstream>

#include <string>
#include <vector>
#include <queue>

#include <chrono>
#include <thread>
#include <mutex>

std::mutex globalLock;

// Создает файл с именем input.txt, с количеством строк countRows и 
// lenWord количесвом символов в одной строке. Файл содержит слова.
// Слово состоит из повторяющейся буквы латинского алфавита.
// Слова упорядочены по невозрастанию.
void generateFile(int countRows, int lenWord);

// Объединяет два отсортированных файла input1 и input2 в один файл out.
// Все файлы должны быть открыты заранее. Предполагается что строки 
// файлов input1 и input2 упорядочены по неубыванию. 
void mergeFiles(std::ifstream& input1, 
					std::ifstream& input2, 
					std::ofstream& out);

// Записывает элементы вектора vector в файл file. 
// Элементы вектора разделяются символом перехода на новую строчку.
// Файл должен быть предварительно открыт.
void writeFile(const std::vector<std::string>& vector, std::ofstream& file);

// Возрращает строчку формата 'tmp{index}.txt'
std::string createTempFileName(size_t index);

// Открывает файл tempFile на запись с именем 'tmp{index}.txt'.
// В случае успешного открытия файла, добавляет имя файла в очередь queueTempFiles.
// Иначе - генерирует исключение.
void openNewTempFile(std::ofstream& tempFile, 
                        size_t& indexFile, 
                        std::queue<std::string>& queueTempFiles);

// Открывает файлы с именами inputFileName1, inputFileName2, outputFileName.
// Выполняет слияние файлов с именами inputFileName1 и inputFileName2 в файл с именем outputFileName.
// Добавляет имя outputFileName в очередь mergeQueue. Закрывает все открытые файлы.
// Удаляет файлы с именами inputFileName1 и inputFileName2.
// В случае если не удалось открыть один из файлов, генерирует искючение.
void mergeTempFiles(const std::string inputFileName1,
                        const std::string inputFileName2,
                        const std::string outputFileName,
                        std::queue<std::string>& mergeQueue);
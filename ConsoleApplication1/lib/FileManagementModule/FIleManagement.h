#pragma once

#include <fstream>
#include <filesystem>

#include <string>
#include <vector>
#include <queue>

#include <chrono>
#include <thread>
#include <mutex>

// Открывает поток для чтения данных из файла, расположенного по пути streamName
// Если открыть поток не получилось, сгенерирует исключение.
std::ifstream getStreamToRead(const std::string& streamName);

// Открывает поток для записи данных в файл, расположенного по пути streamName
// Если открыть поток или создать файл не получилось, сгенерирует исключение.
std::ofstream getStreamToWrite(const std::string& streamName);

// Создает папку, куда записываются все временные файлы.
// Если папка существует, ничего не делает.
void createTempFolder();

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

// Удаляет все временные файлы вместе с папкой, в которой они хранились.
void removeAllTempFiles();

// Открывает файл tempFile на запись с именем 'tmp{index}.txt'.
// В случае успешного открытия файла, добавляет имя файла в очередь queueTempFiles.
// Иначе - генерирует исключение.
void openNewTempFile(std::ofstream& tempFile,
                        size_t& indexFile,
                        std::queue<std::string>& queueTempFiles);


// Открывает файлы с именами inputFileName1, inputFileName2, outputFileName.
// Выполняет слияние файлов с именами inputFileName1 и inputFileName2 в файл с именем outputFileName.
// Закрывает все открытые файлы.
// Удаляет файлы с именами inputFileName1 и inputFileName2.
// В случае если не удалось открыть один из файлов, генерирует искючение.
void mergeTempFiles(const std::string inputFileName1,
                        const std::string inputFileName2,
                        const std::string outputFileName);


// Открывает файлы с именами inputFileName1, inputFileName2, outputFileName.
// Выполняет слияние файлов с именами inputFileName1 и inputFileName2 в файл с именем outputFileName.
// Добавляет имя outputFileName в очередь mergeQueue. 
// lock - необходим для предотвращении ошибок при добавлении имён в очередь во время одновременного обращения.
// Закрывает все открытые файлы.
// Удаляет файлы с именами inputFileName1 и inputFileName2.
// В случае если не удалось открыть любой из файлов, изменяет содержимое errorMessage и заканчивает выполнение.
void mergeTempFilesThreadFunction(const std::string inputFileName1,
                        const std::string inputFileName2,
                        const std::string outputFileName,
                        std::queue<std::string>& mergeQueue,
                        std::mutex& lock,
                        std::string& errorMessage);
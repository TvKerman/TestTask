#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <cstdio>
#include <windows.h>
#include <psapi.h>

#define MAX_VECTOR_BUFFER_SIZE 1000000
#define MAX_PERCENTAGE_OF_FREE_RAM_USAGE 1.0
#define WIDTH 7
#define DIV 1024


void generate_file(int countRows, int lenWord) 
{
    std::ofstream file("input.txt");
    for (int i = 0; i < countRows; i++) {
        char symbol = 'z' - (char)(((float)i / countRows) * 26);
        for (int j = 0; j < lenWord; j++) {
            file << symbol;
        }
        file << '\n';
    }
    file.close();
}

void merge(const std::vector<std::string>& source1,
           const size_t startIndex1, const size_t endIndex1,
           const std::vector<std::string>& source2,
           const size_t startIndex2, const size_t endIndex2,
           std::vector<std::string> &destination) 
{
    size_t readIndex1 = startIndex1;
    size_t readIndex2 = startIndex2;
    size_t writeIndex = 0;
    while (readIndex1 < endIndex1 || readIndex2 < endIndex2) {
        if (readIndex2 == endIndex2 || readIndex1 < endIndex1 && 
                    source1[readIndex1] < source2[readIndex2]) {
            destination[writeIndex] = source1[readIndex1];
            readIndex1++;
        }
        else {
            destination[writeIndex] = source2[readIndex2];
            readIndex2++;
        }
        writeIndex++;
    }
}

void printMemorySize() 
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(MEMORYSTATUSEX);

    GlobalMemoryStatusEx(&statex);

    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
    
    std::cout << "Procces memory: " << virtualMemUsedByMe / DIV / DIV << " Mb\n";
    std::cout << "Memory load: " << statex.dwMemoryLoad << "\n";
    std::cout << "Total phys memory: " << statex.ullTotalPhys / DIV / DIV << " Mb\n";
    std::cout << "The physical memory currently available: " << statex.ullAvailPhys / DIV / DIV << " Mb\n";
}

bool isCanAddWord(std::vector<std::string> &wordsBuffer) 
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(MEMORYSTATUSEX);

    GlobalMemoryStatusEx(&statex);

    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    size_t virtualMemUsedByMe = pmc.PrivateUsage;

    float percentage = (float)virtualMemUsedByMe / statex.ullTotalPhys * 100;
    //std::cout << percentage << '\n';
    return wordsBuffer.size() < MAX_VECTOR_BUFFER_SIZE && percentage < MAX_PERCENTAGE_OF_FREE_RAM_USAGE;
}

void mergeSort_(std::vector<std::string> &vector, 
                                size_t leftBorder, 
                                size_t rightBorder, 
                std::vector<std::string> &buffer) 
{
    size_t size = rightBorder - leftBorder;
    if (size <= 1)
        return;
    
    // Определение середины последовательности
    size_t middle = (leftBorder + rightBorder) / 2;

    // Рекурсивные вызовы функций сортировки для каждой половины
    mergeSort_(vector, leftBorder, middle, buffer);
    mergeSort_(vector, middle, rightBorder, buffer);
    
    // Слияние элементов, результат сохраняется в буфер
    merge(vector, leftBorder, middle, vector, middle, rightBorder, buffer);
    
    // Копирование результата в исходный вектор
    std::copy(buffer.begin(), buffer.begin() + size, vector.begin() + leftBorder);
}


void mergeSort(std::vector<std::string> &vector) 
{
    // Создание буфера
    std::vector<std::string> buffer = vector;
    mergeSort_(vector, 0, vector.size(), buffer);
    buffer.clear();
}

void mergeFiles(std::ifstream& input1, std::ifstream& input2, std::ofstream &out) 
{
    std::string word1, word2;
    std::getline(input1, word1);
    std::getline(input2, word2);

    while (!input1.eof() || !input2.eof()) {
        if (input2.eof() || !input1.eof() &&
            word1 < word2) {
            out << word1 << '\n';
            std::getline(input1, word1);
        }
        else {
            out << word2 << '\n';
            std::getline(input2, word2);
        }
    }
}

void writeFile(const std::vector<std::string>& vector, std::ofstream& file) 
{
    for (auto &element: vector) 
    {
        file << element << '\n';
    }
}

void openNewTempFile(std::ofstream &tempFile, size_t &indexFile,std::queue<std::string> &queueTempFiles) 
{
    std::string fileName = "tmp" + std::to_string(indexFile) + ".txt";
    tempFile.open(fileName, 'w');
    if (tempFile.is_open())
    {
        queueTempFiles.push(fileName);
        indexFile++;
    }
    else
    {
        throw std::exception();
    }
}

int main()
{
    std::ifstream inputFile("input.txt");
    std::ofstream outFile("output.txt");

    std::vector<std::string> tmp; 
    std::queue<std::string> namesTmpFiles;
    size_t indexFile = 1;

    if (inputFile.is_open() && outFile.is_open()) 
    {
        std::string word;
        std::ofstream tempFile;
        while (std::getline(inputFile, word)) 
        {
            if (!isCanAddWord(tmp) ) 
            {
                openNewTempFile(tempFile, indexFile, namesTmpFiles);
                if (tempFile.is_open()) 
                {
                    mergeSort(tmp);
                    writeFile(tmp, tempFile);
                    printMemorySize();
                    tempFile.close();
                    tmp.clear();
                }
                else 
                {
                    std::cerr << "Not open temp file!!!";
                    throw std::exception();
                }
            }
            tmp.push_back(word);
        }
        if (!tempFile.is_open() && tmp.size() > 0)
        {
            openNewTempFile(tempFile, indexFile, namesTmpFiles);
        }
        if (tmp.size() > 0 && tempFile.is_open())
        {
            mergeSort(tmp);
            writeFile(tmp, tempFile);
            tempFile.close();
            tmp.clear();
        }


        inputFile.close();
        while (namesTmpFiles.size() > 2) 
        {
            std::string fileName1 = namesTmpFiles.front();
            namesTmpFiles.pop();
            std::string fileName2 = namesTmpFiles.front();
            namesTmpFiles.pop();
            
            std::ifstream file1(fileName1);
            std::ifstream file2(fileName2);
            std::string fileName3 = "tmp" + std::to_string(indexFile) + ".txt";
            indexFile++;
            std::ofstream mergeFile(fileName3);
            
            if (file1.is_open() && file2.is_open() && mergeFile.is_open()) 
            {
                mergeFiles(file1, file2, mergeFile);
                file1.close();
                file2.close();
                mergeFile.close();
                remove(fileName1.c_str());
                remove(fileName2.c_str());
                namesTmpFiles.push(fileName3);
            }
        }

        std::string fileName1 = namesTmpFiles.front();
        namesTmpFiles.pop();
        std::string fileName2 = namesTmpFiles.front();
        namesTmpFiles.pop();

        std::ifstream file1(fileName1);
        std::ifstream file2(fileName2);

        if (file1.is_open() && file2.is_open())
        {
            mergeFiles(file1, file2, outFile);
            file1.close();
            file2.close();
            remove(fileName1.c_str());
            remove(fileName2.c_str());
        }
        outFile.close();
    }
    else 
    {
        generate_file(10000, 2000);
    }

    return 0;
}
﻿#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <queue>

#include <windows.h>
#include <psapi.h>

#include <chrono>
#include <thread>
#include <mutex>

#define MAX_VECTOR_BUFFER_SIZE 1000000
#define MAX_PERCENTAGE_OF_FREE_RAM_USAGE 1.0
#define MAX_NON_PARALLEL_VECTOR_SIZE 1000
#define WIDTH 7
#define DIV 1024

std::mutex globalLock;

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
           std::vector<std::string> &destination,
           const size_t startWrite) 
{
    size_t readIndex1 = startIndex1;
    size_t readIndex2 = startIndex2;
    size_t writeIndex = startWrite;
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

bool isOrdered(std::vector<std::string> &vector) 
{
    for (size_t i = 1; i < vector.size(); i++) 
    {
        if (vector[i - 1] > vector[i]) 
        {
            return false;
        }
    }
    return true;
} 

void mergeSort_(std::vector<std::string> &vector, 
                const size_t start, const size_t end,
                std::vector<std::string> &buffer) 
{
    size_t size = end - start;
    if (size <= 1)
        return;
    
    // Определение середины последовательности
    size_t middle = (start + end) / 2;

    // Рекурсивные вызовы функций сортировки для каждой половины
    mergeSort_(vector, start, middle, buffer);
    mergeSort_(vector, middle, end, buffer);
    
    // Слияние элементов, результат сохраняется в буфер
    merge(vector, start, middle, vector, middle, end, buffer, start);
    
    // Копирование результата в исходный вектор
    std::copy(buffer.begin() + start, buffer.begin() + end, vector.begin() + start);
}


void mergeSort(std::vector<std::string> &vector) 
{
    // Создание буфера
    std::vector<std::string> buffer = vector;
    auto timeStart = std::chrono::steady_clock::now();
    size_t countThreads = (std::thread::hardware_concurrency() >= 4 ? 4: 0);
    if (countThreads == 0 || vector.size() < MAX_NON_PARALLEL_VECTOR_SIZE) 
    {
        std::cout << "Sorting is not parallel\n";
        mergeSort_(vector, 0, vector.size(), buffer);
    }
    else
    {
        std::cout << "Sorting is parallel\n";
        std::cout << "Count threads: " << countThreads << "\n";
        size_t threadSize = vector.size() / countThreads;
        std::vector<std::thread> threads;
        for (size_t i = 0; i < countThreads; i++) 
        {
            size_t start = i * threadSize;
            size_t end = (i != countThreads - 1 ? start + threadSize : vector.size());
            threads.emplace_back(mergeSort_, std::ref(vector), start, end, std::ref(buffer));
        }
        for (auto &thread : threads) 
        {
            thread.join();
        }

        for (size_t i = 0; i < countThreads; i += 2) 
        {
            size_t start1 = i * threadSize;
            size_t end1 = (i != countThreads - 1 ? start1 + threadSize : vector.size());
            size_t start2 = (i + 1) * threadSize;
            size_t end2 = (i + 1 != countThreads - 1 ? start2 + threadSize : vector.size());
            merge(vector, start1, end1, vector, start2, end2, buffer, start1);
            std::copy(buffer.begin() + start1, buffer.begin() + end2, vector.begin() + start1);
        }
        merge(vector, 0, 2 * threadSize, vector, 2 * threadSize, vector.size(), buffer, 0);
        std::copy(buffer.begin(), buffer.begin() + vector.size(), vector.begin());
    }
    auto timeEnd = std::chrono::steady_clock::now();
    std::cout << "Time sort: " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << " ms\n\n";

    buffer.clear();
}

void mergeFiles(std::ifstream& input1, std::ifstream& input2, std::ofstream &out) 
{
    std::string word1, word2;
    std::getline(input1, word1);
    std::getline(input2, word2);

    while (!input1.eof() || !input2.eof()) {
        if (input2.eof() || !input1.eof() &&
            word1 < word2) 
        {
            out << word1 << '\n';
            std::getline(input1, word1);
        }
        else 
        {
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


std::string createTempFileName(size_t index)
{
    return "tmp" + std::to_string(index) + ".txt";
}


void openNewTempFile(std::ofstream &tempFile, size_t &indexFile,std::queue<std::string> &queueTempFiles) 
{
    std::string fileName = createTempFileName(indexFile);
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

void mergeTempFiles(const std::string inputFileName1, 
                    const std::string inputFileName2,
                    const std::string outputFileName,
                    std::queue<std::string> &mergeQueue) 
{
    std::ifstream inputFile1(inputFileName1);
    std::ifstream inputFile2(inputFileName2);
    std::ofstream mergeFile(outputFileName);

    if (inputFile1.is_open() && inputFile2.is_open() && mergeFile.is_open())
    {
        mergeFiles(inputFile1, inputFile2, mergeFile);
        inputFile1.close();
        inputFile2.close();
        mergeFile.close();
        remove(inputFileName1.c_str());
        remove(inputFileName2.c_str());

        globalLock.lock();
        mergeQueue.push(outputFileName);
        globalLock.unlock();
    } 
    else
    {
        std::cerr << "Failed to open files\n";
        throw std::exception();
    }
}

int main()
{
    std::ifstream inputFile("input.txt");

    std::vector<std::string> tmp;
    std::queue<std::string> namesTmpFiles;
    size_t indexFile = 1;

    if (inputFile.is_open())
    {
        std::string word;
        std::ofstream tempFile;
        while (std::getline(inputFile, word))
        {
            if (!isCanAddWord(tmp))
            {
                openNewTempFile(tempFile, indexFile, namesTmpFiles);
                if (tempFile.is_open())
                {
                    mergeSort(tmp);
                    if (!isOrdered(tmp))
                    {
                        std::cerr << "Sort is not correct\n";
                        tempFile.close();
                        tmp.clear();
                        throw std::exception();
                    }
                    writeFile(tmp, tempFile);
                    //printMemorySize();
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
            if (!isOrdered(tmp))
            {
                std::cerr << "Sort is not correct\n";
                tempFile.close();
                tmp.clear();
                throw std::exception();
            }
            tempFile.close();
            tmp.clear();
        }
        inputFile.close();
    }

    //while (namesTmpFiles.size() > 2) 
    //{
    //    auto timeStart = std::chrono::steady_clock::now();
    //
    //    std::string fileName1 = namesTmpFiles.front();
    //    namesTmpFiles.pop();
    //    std::string fileName2 = namesTmpFiles.front();
    //    namesTmpFiles.pop();
    //
    //    mergeTempFiles(fileName1, fileName2, createTempFileName(indexFile), namesTmpFiles);
    //    indexFile++;
    //
    //    auto timeEnd = std::chrono::steady_clock::now();
    //    std::cout << "Time merge two files: " << std::chrono::duration_cast<std::chrono::seconds>(timeEnd - timeStart).count() << " s\n\n";
    //}
    //
    //auto timeStart = std::chrono::steady_clock::now();
    //
    //std::string fileName1 = namesTmpFiles.front();
    //namesTmpFiles.pop();
    //std::string fileName2 = namesTmpFiles.front();
    //namesTmpFiles.pop();
    //
    //mergeTempFiles(fileName1, fileName2, "output.txt", namesTmpFiles);
    //
    //auto timeEnd = std::chrono::steady_clock::now();
    //std::cout << "Time merge two files: " << std::chrono::duration_cast<std::chrono::seconds>(timeEnd - timeStart).count() << " s\n\n";
    auto timeStart = std::chrono::steady_clock::now();
    while (namesTmpFiles.size() > 2)
    {
        std::vector<std::thread> threads;
        size_t countThreads = (std::thread::hardware_concurrency() >= 4 ? 4: std::thread::hardware_concurrency());
        if (countThreads == 0) 
        {
            std::string fileName1 = namesTmpFiles.front();
            namesTmpFiles.pop();
            std::string fileName2 = namesTmpFiles.front();
            namesTmpFiles.pop();
            
            mergeTempFiles(fileName1, fileName2, createTempFileName(indexFile), std::ref(namesTmpFiles));
            indexFile++;
        } 
        else 
        {
            for (size_t i = 0; i < countThreads && namesTmpFiles.size() > 1; i++)
            {
                std::string fileName1 = namesTmpFiles.front();
                namesTmpFiles.pop();
                std::string fileName2 = namesTmpFiles.front();
                namesTmpFiles.pop();

                threads.emplace_back(mergeTempFiles, fileName1, fileName2, createTempFileName(indexFile), std::ref(namesTmpFiles));
                indexFile++;
            }
            for (auto& thread : threads)
            {
                thread.join();
            }
        }
    }
    if (namesTmpFiles.size() == 2)
    {
        std::string fileName1 = namesTmpFiles.front();
        namesTmpFiles.pop();
        std::string fileName2 = namesTmpFiles.front();
        namesTmpFiles.pop();

        mergeTempFiles(fileName1, fileName2, "output.txt", std::ref(namesTmpFiles));
    }
    else 
    {
        std::rename(createTempFileName(indexFile - 1).c_str(), "output.txt");
    }
    auto timeEnd = std::chrono::steady_clock::now();
    std::cout << "Time merge all temp files: " << std::chrono::duration_cast<std::chrono::seconds>(timeEnd - timeStart).count() << " s\n\n";
    return 0;
}
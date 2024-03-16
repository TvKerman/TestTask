#include <iostream>

#include <windows.h>
#include <psapi.h>

#include "lib/Sorting/Sorting.h"
#include "lib/FileManagementModule/FIleManagement.h"

#define MAX_VECTOR_BUFFER_SIZE 1000000
#define MAX_PERCENTAGE_OF_FREE_RAM_USAGE 1.0
#define DIV 1024

//void printMemorySize() 
//{
//    MEMORYSTATUSEX statex;
//    statex.dwLength = sizeof(MEMORYSTATUSEX);
//
//    GlobalMemoryStatusEx(&statex);
//
//    PROCESS_MEMORY_COUNTERS_EX pmc;
//    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
//    SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
//    
//    std::cout << "Procces memory: " << virtualMemUsedByMe / DIV / DIV << " Mb\n";
//    std::cout << "Memory load: " << statex.dwMemoryLoad << "\n";
//    std::cout << "Total phys memory: " << statex.ullTotalPhys / DIV / DIV << " Mb\n";
//    std::cout << "The physical memory currently available: " << statex.ullAvailPhys / DIV / DIV << " Mb\n";
//}

bool isCanAddWord(std::vector<std::string> &wordsBuffer) 
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(MEMORYSTATUSEX);

    GlobalMemoryStatusEx(&statex);

    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    size_t virtualMemUsedByMe = pmc.PrivateUsage;

    float percentage = (float)virtualMemUsedByMe / statex.ullTotalPhys * 100;
    return wordsBuffer.size() < MAX_VECTOR_BUFFER_SIZE && percentage < MAX_PERCENTAGE_OF_FREE_RAM_USAGE;
}

int main()
{
    std::ifstream inputFile("input.txt");

    std::vector<std::string> tempBuffer;
    std::queue<std::string> fileMergeQueue;
    size_t indexFile = 1;

    if (inputFile.is_open())
    {
        std::string word;
        std::ofstream tempFile;
        while (std::getline(inputFile, word))
        {
            if (!isCanAddWord(tempBuffer))
            {
                openNewTempFile(tempFile, indexFile, fileMergeQueue);
                if (tempFile.is_open())
                {
                    mergeSort(tempBuffer);
                    writeFile(tempBuffer, tempFile);
                    tempFile.close();
                    tempBuffer.clear();
                }
                else
                {
                    std::cerr << "Not open temp file!";
                    throw std::exception();
                }
            }
            tempBuffer.push_back(word);
        }
        if (!tempFile.is_open() && tempBuffer.size() > 0)
        {
            openNewTempFile(tempFile, indexFile, fileMergeQueue);
            if (!tempFile.is_open()) 
            {
                std::cerr << "Not open temp file!";
                throw std::exception();
            }
        }
        if (tempBuffer.size() > 0 && tempFile.is_open())
        {
            mergeSort(tempBuffer);
            writeFile(tempBuffer, tempFile);
            tempFile.close();
            tempBuffer.clear();
        }
        inputFile.close();
    }

    while (fileMergeQueue.size() > 2)
    {
        std::vector<std::thread> threads;
        size_t countThreads = (std::thread::hardware_concurrency() >= MAX_COUNT_OF_THREADS_USED ? 
                                                                    MAX_COUNT_OF_THREADS_USED: 
                                                                    std::thread::hardware_concurrency());
        if (countThreads == 0) 
        {
            std::string fileName1 = fileMergeQueue.front();
            fileMergeQueue.pop();
            std::string fileName2 = fileMergeQueue.front();
            fileMergeQueue.pop();
            
            mergeTempFiles(fileName1, fileName2, createTempFileName(indexFile), std::ref(fileMergeQueue));
            indexFile++;
        } 
        else 
        {
            for (size_t i = 0; i < countThreads && fileMergeQueue.size() > 1; i++)
            {
                std::string fileName1 = fileMergeQueue.front();
                fileMergeQueue.pop();
                std::string fileName2 = fileMergeQueue.front();
                fileMergeQueue.pop();

                threads.emplace_back(mergeTempFiles, fileName1, fileName2, createTempFileName(indexFile), std::ref(fileMergeQueue));
                indexFile++;
            }
            for (auto& thread : threads)
            {
                thread.join();
            }
        }
    }
    if (fileMergeQueue.size() == 2)
    {
        std::string fileName1 = fileMergeQueue.front();
        fileMergeQueue.pop();
        std::string fileName2 = fileMergeQueue.front();
        fileMergeQueue.pop();

        mergeTempFiles(fileName1, fileName2, "output.txt", std::ref(fileMergeQueue));
    }
    else 
    {
        std::string fileName = fileMergeQueue.front();
        fileMergeQueue.pop();
        std::rename(fileName.c_str(), "output.txt");
    }
    return 0;
}
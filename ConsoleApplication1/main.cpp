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
    std::cout << "Enter the path to the file:\n";
    std::string filePath;
    std::cin >> filePath;

    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) 
    {
        std::cerr << "Can't open input file\n";
        return 0;
    }

    std::vector<std::string> tempBuffer;
    std::queue<std::string> fileMergeQueue;
    size_t indexFile = 1;

    std::string word;
    std::ofstream tempFile;
    std::cout << "The reading of words from the file has begun\n\n";
    while (std::getline(inputFile, word))
    {
        if (!isCanAddWord(tempBuffer))
        {
            openNewTempFile(tempFile, indexFile, fileMergeQueue);
            if (tempFile.is_open())
            {
                std::cout << indexFile - 1 << ". The data block has been read\n";
                mergeSort(tempBuffer);
                writeFile(tempBuffer, tempFile);
                tempFile.close();
                tempBuffer.clear();
                std::cout << "The data block is sorted and written to a temporary file\n\n";
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
        std::cout << indexFile - 1 << ". The data block has been read\n";
        mergeSort(tempBuffer);
        writeFile(tempBuffer, tempFile);
        tempFile.close();
        tempBuffer.clear();
        std::cout << "The data block is sorted and written to a temporary file\n\n";
    }
    inputFile.close();
    std::cout << "The file reading is finished.\n";

    std::cout << "Merging of temporary files has started.\n\n";
    while (fileMergeQueue.size() > 2)
    {
        std::vector<std::thread> threads;
        std::mutex lock;
        size_t countThreads = (std::thread::hardware_concurrency() >= MAX_COUNT_OF_THREADS_USED ? 
                                                                    MAX_COUNT_OF_THREADS_USED: 
                                                                    std::thread::hardware_concurrency());
        std::cout << "There are no temporary files left: " << fileMergeQueue.size() << "\n";
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

                threads.emplace_back(mergeTempFilesThreadFunction, fileName1, fileName2, 
                                                            createTempFileName(indexFile), 
                                                            std::ref(fileMergeQueue),
                                                            std::ref(lock));
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
        std::cout << "There are no temporary files left: " << fileMergeQueue.size() << "\n\n";
        std::string fileName1 = fileMergeQueue.front();
        fileMergeQueue.pop();
        std::string fileName2 = fileMergeQueue.front();
        fileMergeQueue.pop();

        mergeTempFiles(fileName1, fileName2, "output.txt", std::ref(fileMergeQueue));
    }
    else 
    {
        std::cout << "There are no temporary files left: " << fileMergeQueue.size() << "\n\n";
        std::string fileName = fileMergeQueue.front();
        fileMergeQueue.pop();
        std::rename(fileName.c_str(), "output.txt");
    }
    std::cout << "The sorting of the source file is finished\n";

    return 0;
}
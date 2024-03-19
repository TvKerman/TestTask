#include <iostream>

#include <windows.h>
#include <psapi.h>

#include "lib/Sorting/Sorting.h"
#include "lib/FileManagementModule/FileManagement.h"

#define MAX_VECTOR_BUFFER_SIZE 1000000
#define MAX_PERCENTAGE_OF_FREE_RAM_USAGE 20.0
#define MAX_USAGE_RAM 200 * 1024 * 1024

// Возвращает true, если в векторе wordBuffer меньше чем MAX_VECTOR_BUFFER_SIZE элементов и
// при этом процент используемой основным потоком оперативной памяти 
// меньше чем MAX_PERCENTAGE_OF_FREE_RAM_USAGE, а также примерный размер используемой 
// основным потоком оперативной памяти меньше чем MAX_USAGE_RAM байт.
// Иначе - false.
bool isCanAddWord(std::vector<std::string> &wordsBuffer) 
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(MEMORYSTATUSEX);

    GlobalMemoryStatusEx(&statex);

    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    size_t virtualMemUsedByMe = pmc.PrivateUsage;

    float percentage = (float)virtualMemUsedByMe / statex.ullTotalPhys * 100;
    return wordsBuffer.size() < MAX_VECTOR_BUFFER_SIZE && 
            percentage < MAX_PERCENTAGE_OF_FREE_RAM_USAGE &&
            virtualMemUsedByMe < MAX_USAGE_RAM;
}

// Сортрирует buffer и записывает результат сортировки в file. Файл открывается или создается в 
// функции. Имя файла создается исходя из sequenceNumberTempFile. В случае успешной записи, 
// в fileMergeQueue добавится имя файла, в который произошла запись. 
// Если отрыть или создать файл не удалось, сгенерирует сообщение об ошибке.
void creatingNewTempFileBasedOnBuffer(std::vector<std::string>& buffer, 
                                                    std::ofstream &file,
                                                    size_t &sequenceNumberTempFile,
                                                    std::queue<std::string> &fileMergeQueue)
{
    try
    {
        openNewTempFile(file, sequenceNumberTempFile, fileMergeQueue);
    }
    catch (const std::exception& error)
    {
        throw error;
    }
    std::cout << sequenceNumberTempFile - 1 << ". The data block has been read\n";
    mergeSort(buffer);
    writeFile(buffer, file);
    file.close();
    buffer.clear();
    std::cout << "The data block is sorted and written to a temporary file\n\n";
}

int main(int argc, char* argv[])
{
    std::string filePath(argv[1]);

    try
    {
        std::ifstream inputFile = getStreamToRead(filePath);
        createTempFolder();

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
                creatingNewTempFileBasedOnBuffer(tempBuffer, tempFile, indexFile, fileMergeQueue);
            }
            tempBuffer.push_back(word);
        }
        if (tempBuffer.size() > 0)
        {
            creatingNewTempFileBasedOnBuffer(tempBuffer, tempFile, indexFile, fileMergeQueue);
        }
        inputFile.close();
        std::cout << "The file reading is finished.\n";

        std::cout << "Merging of temporary files has started.\n\n";
        while (fileMergeQueue.size() > 2)
        {
            std::vector<std::thread> threads;
            std::mutex lock;
            size_t countThreads = (std::thread::hardware_concurrency() >= MAX_COUNT_OF_THREADS_USED ?
                                                                            MAX_COUNT_OF_THREADS_USED :
                                                                            std::thread::hardware_concurrency());
            std::string errorMessage = "";
            std::cout << "There are no temporary files left: " << fileMergeQueue.size() << "\n";
            if (countThreads)
            {
                for (size_t i = 0; i < countThreads && fileMergeQueue.size() > 1; i++)
                {
                    std::string fileName1 = fileMergeQueue.front();
                    fileMergeQueue.pop();
                    std::string fileName2 = fileMergeQueue.front();
                    fileMergeQueue.pop();

                    threads.emplace_back(mergeTempFilesThreadFunction,
                                        fileName1, fileName2,
                                        createTempFileName(indexFile),
                                        std::ref(fileMergeQueue),
                                        std::ref(lock),
                                        std::ref(errorMessage));
                    indexFile++;
                }
                for (auto& thread : threads)
                {
                    thread.join();
                }
                if (errorMessage != "")
                {
                    throw std::exception(errorMessage.c_str());
                }
            }
            else
            {
                std::string fileName1 = fileMergeQueue.front();
                fileMergeQueue.pop();
                std::string fileName2 = fileMergeQueue.front();
                fileMergeQueue.pop();

                mergeTempFiles(fileName1, fileName2,
                                        createTempFileName(indexFile),
                                        fileMergeQueue);
                indexFile++;
            }
        }

        std::cout << "There are no temporary files left: " << fileMergeQueue.size() << "\n\n";
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
        std::cout << "The sorting of the source file is finished\n";
    }
    catch (const std::exception& error)
    {
        removeAllTempFiles();
        std::cerr << error.what(); 
    }
    
    return 0;
}
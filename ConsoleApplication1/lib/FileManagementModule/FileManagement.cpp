#include "FIleManagement.h"

void generateFile(int countRows, int lenWord)
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

void mergeFiles(std::ifstream& input1, std::ifstream& input2, std::ofstream& out)
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
    for (auto& element : vector)
    {
        file << element << '\n';
    }
}

std::string createTempFileName(size_t index)
{
    return "Temp/tmp" + std::to_string(index) + ".txt";
}

void openNewTempFile(std::ofstream& tempFile, size_t& indexFile, 
                                    std::queue<std::string>& queueTempFiles)
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
                        std::queue<std::string>& mergeQueue)
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
        mergeQueue.push(outputFileName);
    }
    else
    {
        throw std::exception();
    }
}

void mergeTempFilesThreadFunction(const std::string inputFileName1,
                        const std::string inputFileName2,
                        const std::string outputFileName,
                        std::queue<std::string>& mergeQueue,
                        std::mutex &lock)
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

        lock.lock();
        mergeQueue.push(outputFileName);
        lock.unlock();
    }
    else
    {
        throw std::exception();
    }
}
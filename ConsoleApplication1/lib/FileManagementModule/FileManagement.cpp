#include "FileManagement.h"

std::ifstream getStreamToRead(const std::string& streamName)
{
    std::ifstream stream(streamName);
    if (!stream.is_open())
    {
        throw std::exception("ERROR! The file could not be opened for reading\n");
    }

    return stream;
}

std::ofstream getStreamToWrite(const std::string& streamName) 
{
    std::ofstream stream(streamName);
    if (!stream.is_open())
    {
        throw std::exception("ERROR! The file could not be opened for writing\n");
    }

    return stream;
}

void createTempFolder() 
{
    std::filesystem::create_directory("Temp");
}

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

void removeAllTempFiles() 
{
    std::filesystem::remove_all("Temp");
}

void openNewTempFile(std::ofstream& tempFile, size_t& indexFile, 
                                    std::queue<std::string>& queueTempFiles)
{
    std::string fileName = createTempFileName(indexFile);
    try
    {
        tempFile = getStreamToWrite(fileName);
    }
    catch (const std::exception& error)
    {
        throw error;
    }
    
    queueTempFiles.push(fileName);
    indexFile++;
}

void mergeTempFiles(const std::string inputFileName1,
                    const std::string inputFileName2,
                    const std::string outputFileName)
{
    std::ifstream inputFile1, inputFile2;
    std::ofstream mergeFile;
    try
    {
        inputFile1 = getStreamToRead(inputFileName1);
        inputFile2 = getStreamToRead(inputFileName2);
        mergeFile = getStreamToWrite(outputFileName);
    }
    catch (const std::exception& error)
    {
        inputFile1.close();
        inputFile2.close();
        mergeFile.close();

        throw error;
    }

    mergeFiles(inputFile1, inputFile2, mergeFile);
    inputFile1.close();
    inputFile2.close();
    mergeFile.close();
    remove(inputFileName1.c_str());
    remove(inputFileName2.c_str());
}

void mergeTempFilesThreadFunction(const std::string inputFileName1,
                                    const std::string inputFileName2,
                                    const std::string outputFileName,
                                    std::queue<std::string> &mergeQueue,
                                    std::mutex &lock, 
                                    std::string &errorMessage)
{
    try
    {
        mergeTempFiles(inputFileName1, inputFileName2, outputFileName);
    }
    catch (const std::exception& error)
    {
        errorMessage = error.what();
        return;
    }

    lock.lock();
    mergeQueue.push(outputFileName);
    lock.unlock();
}
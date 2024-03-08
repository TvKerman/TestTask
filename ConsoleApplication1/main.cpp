#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <cstdio>

#define MAX_VECTOR_BUFFER_SIZE 100000

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
        std::ofstream tmpFile;
        while (std::getline(inputFile, word)) 
        {
            if (!tmpFile.is_open())
            {
                std::string fileName = "tmp" + std::to_string(indexFile) + ".txt";
                tmpFile.open(fileName, 'w');
                if (tmpFile.is_open())
                {
                    namesTmpFiles.push(fileName);
                    indexFile++;
                }
                else
                {
                    throw std::exception();
                }
            }

            if (tmp.size() == MAX_VECTOR_BUFFER_SIZE && tmpFile.is_open()) 
            {
                mergeSort(tmp);
                writeFile(tmp, tmpFile);
                tmpFile.close();
                tmp.clear();
            } 
            tmp.push_back(word);
        }
        if (!tmpFile.is_open() && tmp.size() > 0)
        {
            std::string fileName = "tmp" + std::to_string(indexFile) + ".txt";
            tmpFile.open(fileName, 'w');
            if (tmpFile.is_open())
            {
                namesTmpFiles.push(fileName);
                indexFile++;
            }
            else
            {
                throw std::exception();
            }
        }
        if (tmp.size() > 0 && tmpFile.is_open())
        {
            mergeSort(tmp);
            writeFile(tmp, tmpFile);
            tmpFile.close();
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
        generate_file(1000000, 200);
    }

    
    return 0;
}


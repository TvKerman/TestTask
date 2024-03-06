#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void generate_file(int countRows, int lenWord) {
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
           std::vector<std::string> &destination) {
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
                std::vector<std::string> &buffer) {
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


void mergeSort(std::vector<std::string> &vector) {
    // Создание буфера
    std::vector<std::string> buffer = vector;
    mergeSort_(vector, 0, vector.size(), buffer);
    buffer.clear();
}

int main()
{
    std::ifstream inputFile("input.txt");
    std::ofstream outFile("output.txt");

    std::vector<std::string> tmp; 

    if (inputFile.is_open() && outFile.is_open()) 
    {
        std::string line;
        while (std::getline(inputFile, line))
        {
            tmp.push_back(line);
        }

        mergeSort(tmp);
        inputFile.close();

        for (auto element : tmp) 
        {
            outFile << element << '\n';
        }
        outFile.close();
    }
    else 
    {
        generate_file(1000000, 200);
    }

    
    return 0;
}


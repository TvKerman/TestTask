#pragma once
#include <string>
#include <vector>
#include <queue>

#include <chrono>
#include <thread>
#include <mutex>

#define MAX_NON_PARALLEL_VECTOR_SIZE 10000
#define MAX_COUNT_OF_THREADS_USED 4

// Объединяет два отсортированных вектора source1 и source2 в один destination.
// В destination предварительно должен быть выделено достаточно места.
// startIndex1 - указывает на место начала слияния вектора source1
// endIndex1 - указывает на место окончания слияния вектора source1
// startIndex2 - указывает на место начала слияния вектора source2
// endIndex2 - указывает на место окончания слияния вектора source2
// startWrite - указывает на место начало записи элементов в destination.
void merge(const std::vector<std::string>& source1,
                    const size_t startIndex1, const size_t endIndex1,
                    const std::vector<std::string>& source2,
                    const size_t startIndex2, const size_t endIndex2,
                    std::vector<std::string>& destination,
                    const size_t startWrite);

// Сортирует вектор vector от элемента под индексом start до end не включительно.
// Использует итеративную реализацию сортировки слиянием.
// Используемая буфером buffer память должна быть выделена заранее.
// Размер памяти должен быть не меньше чем end элементов.
void mergeSort_(std::vector<std::string>& vector,
                    const size_t start, const size_t end,
                    std::vector<std::string>& buffer);

// Сортировка слиянием вектора vector.
// Если количество элементов в векторе превышает чем MAX_NON_PARALLEL_VECTOR_SIZE,
// функция пробует разбить вектор на несколько частей и использовать отдельные потоки.
// Если возможно использование MAX_COUNT_OF_THREADS_USED потоков, то вектор будет 
// разбит на части размером vector.size() / MAX_NON_PARALLEL_VECTOR_SIZE.
// Иначе потоки использованы не будут.
void mergeSort(std::vector<std::string>& vector);
#pragma once
#include <string>
#include <vector>
#include <queue>

#include <chrono>
#include <thread>
#include <mutex>

#define MAX_NON_PARALLEL_VECTOR_SIZE 1000
#define MAX_COUNT_OF_THREADS_USED 4

// ќбъедин€ет два отсортированных вектора source1 и source2 в один destination.
// ¬ destination предварительно должен быть выделено достаточно места.
// startIndex1 - указывает на место начала сли€ни€ вектора source1
// endIndex1 - указывает на место окончани€ сли€ни€ вектора source1
// startIndex2 - указывает на место начала сли€ни€ вектора source2
// endIndex2 - указывает на место окончани€ сли€ни€ вектора source2
// startWrite - указывает на место начало записи элементов в destination.
void merge(const std::vector<std::string>& source1,
                    const size_t startIndex1, const size_t endIndex1,
                    const std::vector<std::string>& source2,
                    const size_t startIndex2, const size_t endIndex2,
                    std::vector<std::string>& destination,
                    const size_t startWrite);

// –екурсивна€ часть сортировки сли€нием
void mergeSort_(std::vector<std::string>& vector,
                    const size_t start, const size_t end,
                    std::vector<std::string>& buffer);

// —ортировка сли€нием вектора vector
void mergeSort(std::vector<std::string>& vector);
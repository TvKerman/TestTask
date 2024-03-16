#include "Sorting.h"

void merge(const std::vector<std::string>& source1,
                    const size_t startIndex1, const size_t endIndex1,
                    const std::vector<std::string>& source2,
                    const size_t startIndex2, const size_t endIndex2,
                    std::vector<std::string>& destination,
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

void mergeSort_(std::vector<std::string>& vector,
                    const size_t start, const size_t end,
                    std::vector<std::string>& buffer)
{
    size_t size = end - start;
    if (size <= 1)
        return;

    size_t middle = (start + end) / 2;

    mergeSort_(vector, start, middle, buffer);
    mergeSort_(vector, middle, end, buffer);

    merge(vector, start, middle, vector, middle, end, buffer, start);

    std::copy(buffer.begin() + start, buffer.begin() + end, vector.begin() + start);
}

void mergeSort(std::vector<std::string>& vector)
{
    std::vector<std::string> buffer = vector;
    size_t countThreads = (std::thread::hardware_concurrency() >= MAX_COUNT_OF_THREADS_USED ? 
                                                                  MAX_COUNT_OF_THREADS_USED : 
                                                                                          0);
    if (countThreads == 0 || vector.size() < MAX_NON_PARALLEL_VECTOR_SIZE)
    {
        mergeSort_(vector, 0, vector.size(), buffer);
    }
    else
    {
        size_t threadSize = vector.size() / countThreads;
        std::vector<std::thread> threads;
        for (size_t i = 0; i < countThreads; i++)
        {
            size_t start = i * threadSize;
            size_t end = (i != countThreads - 1 ? start + threadSize : vector.size());
            threads.emplace_back(mergeSort_, std::ref(vector), start, end, std::ref(buffer));
        }
        for (auto& thread : threads)
        {
            thread.join();
        }

        // Надо переписать
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

    buffer.clear();
}
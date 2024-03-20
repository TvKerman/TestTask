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
    for (size_t subvectorSize = 1; subvectorSize <= size; subvectorSize *= 2)
    {
        for (size_t i = start; i + subvectorSize < end; i += 2 * subvectorSize)
        {
            size_t from = i;
            size_t middle = i + subvectorSize;
            size_t to = std::min(i + 2 * subvectorSize, end);

            merge(vector, from, middle, vector, middle, to, buffer, from);
        }
        std::copy(buffer.begin() + start, buffer.begin() + end, vector.begin() + start);
    }
}

void mergeSort(std::vector<std::string>& vector)
{
    std::vector<std::string> buffer = vector;
    size_t countThreads = (std::thread::hardware_concurrency() >= MAX_COUNT_OF_THREADS_USED ? 
                                                                  MAX_COUNT_OF_THREADS_USED : 
                                                                                          0);
    if (countThreads != 0 && vector.size() > MAX_NON_PARALLEL_VECTOR_SIZE)
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

        for (size_t subvectorSize = threadSize; subvectorSize < vector.size() / 2 + 1; subvectorSize *= 2, countThreads /= 2) 
        {
            for (size_t i = 0; i < countThreads; i += 2)
            {
                size_t start1 = i * subvectorSize;
                size_t end1 = (i != countThreads - 1 ? start1 + subvectorSize : vector.size());
                size_t start2 = (i + 1) * subvectorSize;
                size_t end2 = (i + 1 != countThreads - 1 ? start2 + subvectorSize : vector.size());
                merge(vector, start1, end1, vector, start2, end2, buffer, start1);
                std::copy(buffer.begin() + start1, buffer.begin() + end2, vector.begin() + start1);
            }
        }
    }
    else
    {
        mergeSort_(vector, 0, vector.size(), buffer);
    }

    buffer.clear();
}
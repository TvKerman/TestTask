#pragma once
#include <string>
#include <vector>
#include <queue>

#include <chrono>
#include <thread>
#include <mutex>

#define MAX_NON_PARALLEL_VECTOR_SIZE 10000
#define MAX_COUNT_OF_THREADS_USED 4

// ���������� ��� ��������������� ������� source1 � source2 � ���� destination.
// � destination �������������� ������ ���� �������� ���������� �����.
// startIndex1 - ��������� �� ����� ������ ������� ������� source1
// endIndex1 - ��������� �� ����� ��������� ������� ������� source1
// startIndex2 - ��������� �� ����� ������ ������� ������� source2
// endIndex2 - ��������� �� ����� ��������� ������� ������� source2
// startWrite - ��������� �� ����� ������ ������ ��������� � destination.
void merge(const std::vector<std::string>& source1,
                    const size_t startIndex1, const size_t endIndex1,
                    const std::vector<std::string>& source2,
                    const size_t startIndex2, const size_t endIndex2,
                    std::vector<std::string>& destination,
                    const size_t startWrite);

// ��������� ������ vector �� �������� ��� �������� start �� end �� ������������.
// ���������� ����������� ���������� ���������� ��������.
// ������������ ������� buffer ������ ������ ���� �������� �������.
// ������ ������ ������ ���� �� ������ ��� end ���������.
void mergeSort_(std::vector<std::string>& vector,
                    const size_t start, const size_t end,
                    std::vector<std::string>& buffer);

// ���������� �������� ������� vector.
// ���� ���������� ��������� � ������� ��������� ��� MAX_NON_PARALLEL_VECTOR_SIZE,
// ������� ������� ������� ������ �� ��������� ������ � ������������ ��������� ������.
// ���� �������� ������������� MAX_COUNT_OF_THREADS_USED �������, �� ������ ����� 
// ������ �� ����� �������� vector.size() / MAX_NON_PARALLEL_VECTOR_SIZE.
// ����� ������ ������������ �� �����.
void mergeSort(std::vector<std::string>& vector);
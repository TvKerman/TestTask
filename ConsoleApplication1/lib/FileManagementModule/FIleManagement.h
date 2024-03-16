#pragma once

#include <fstream>

#include <string>
#include <vector>
#include <queue>

#include <chrono>
#include <thread>
#include <mutex>

std::mutex globalLock;

// ������� ���� � ������ input.txt, � ����������� ����� countRows � 
// lenWord ���������� �������� � ����� ������. ���� �������� �����.
// ����� ������� �� ������������� ����� ���������� ��������.
// ����� ����������� �� �������������.
void generateFile(int countRows, int lenWord);

// ���������� ��� ��������������� ����� input1 � input2 � ���� ���� out.
// ��� ����� ������ ���� ������� �������. �������������� ��� ������ 
// ������ input1 � input2 ����������� �� ����������. 
void mergeFiles(std::ifstream& input1, 
					std::ifstream& input2, 
					std::ofstream& out);

// ���������� �������� ������� vector � ���� file. 
// �������� ������� ����������� �������� �������� �� ����� �������.
// ���� ������ ���� �������������� ������.
void writeFile(const std::vector<std::string>& vector, std::ofstream& file);

// ���������� ������� ������� 'tmp{index}.txt'
std::string createTempFileName(size_t index);

// ��������� ���� tempFile �� ������ � ������ 'tmp{index}.txt'.
// � ������ ��������� �������� �����, ��������� ��� ����� � ������� queueTempFiles.
// ����� - ���������� ����������.
void openNewTempFile(std::ofstream& tempFile, 
                        size_t& indexFile, 
                        std::queue<std::string>& queueTempFiles);

// ��������� ����� � ������� inputFileName1, inputFileName2, outputFileName.
// ��������� ������� ������ � ������� inputFileName1 � inputFileName2 � ���� � ������ outputFileName.
// ��������� ��� outputFileName � ������� mergeQueue. ��������� ��� �������� �����.
// ������� ����� � ������� inputFileName1 � inputFileName2.
// � ������ ���� �� ������� ������� ���� �� ������, ���������� ���������.
void mergeTempFiles(const std::string inputFileName1,
                        const std::string inputFileName2,
                        const std::string outputFileName,
                        std::queue<std::string>& mergeQueue);
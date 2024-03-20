#pragma once

#include <fstream>
#include <filesystem>

#include <string>
#include <vector>
#include <queue>

#include <chrono>
#include <thread>
#include <mutex>

// ��������� ����� ��� ������ ������ �� �����, �������������� �� ���� streamName
// ���� ������� ����� �� ����������, ����������� ����������.
std::ifstream getStreamToRead(const std::string& streamName);

// ��������� ����� ��� ������ ������ � ����, �������������� �� ���� streamName
// ���� ������� ����� ��� ������� ���� �� ����������, ����������� ����������.
std::ofstream getStreamToWrite(const std::string& streamName);

// ������� �����, ���� ������������ ��� ��������� �����.
// ���� ����� ����������, ������ �� ������.
void createTempFolder();

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

// ������� ��� ��������� ����� ������ � ������, � ������� ��� ���������.
void removeAllTempFiles();

// ��������� ���� tempFile �� ������ � ������ 'tmp{index}.txt'.
// � ������ ��������� �������� �����, ��������� ��� ����� � ������� queueTempFiles.
// ����� - ���������� ����������.
void openNewTempFile(std::ofstream& tempFile,
                        size_t& indexFile,
                        std::queue<std::string>& queueTempFiles);


// ��������� ����� � ������� inputFileName1, inputFileName2, outputFileName.
// ��������� ������� ������ � ������� inputFileName1 � inputFileName2 � ���� � ������ outputFileName.
// ��������� ��� �������� �����.
// ������� ����� � ������� inputFileName1 � inputFileName2.
// � ������ ���� �� ������� ������� ���� �� ������, ���������� ���������.
void mergeTempFiles(const std::string inputFileName1,
                        const std::string inputFileName2,
                        const std::string outputFileName);


// ��������� ����� � ������� inputFileName1, inputFileName2, outputFileName.
// ��������� ������� ������ � ������� inputFileName1 � inputFileName2 � ���� � ������ outputFileName.
// ��������� ��� outputFileName � ������� mergeQueue. 
// lock - ��������� ��� �������������� ������ ��� ���������� ��� � ������� �� ����� �������������� ���������.
// ��������� ��� �������� �����.
// ������� ����� � ������� inputFileName1 � inputFileName2.
// � ������ ���� �� ������� ������� ����� �� ������, �������� ���������� errorMessage � ����������� ����������.
void mergeTempFilesThreadFunction(const std::string inputFileName1,
                        const std::string inputFileName2,
                        const std::string outputFileName,
                        std::queue<std::string>& mergeQueue,
                        std::mutex& lock,
                        std::string& errorMessage);
#include "pch.h"
#include "CppUnitTest.h"

#include "../ConsoleApplication1/lib/FileManagementModule/FileManagement.cpp"

#include <ctime>
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define START_DATA_SIZE_1 10
#define END_DATA_SIZE_1 1000
#define START_DATA_SIZE_2 100000
#define END_DATA_SIZE_2 1000000
#define START_WORD_SIZE 1
#define END_WORD_SIZE 1000
#define START_CODE_SYMBOL 'a'
#define END_CODE_SYMBOL 'z'

namespace FileManagementTests
{
	TEST_CLASS(MergeFilesTests)
	{
	private:
		std::string generateRandomWord(size_t startSize, size_t endSize)
		{
			size_t wordSize = std::rand() % (endSize - startSize) + startSize;
			std::string word = "";
			for (size_t j = 0; j < wordSize; j++)
			{
				word += (char)(std::rand() % (END_CODE_SYMBOL - START_CODE_SYMBOL) + START_CODE_SYMBOL);
			}

			return word;
		}

		std::vector<std::string> generateRandomTestDataForMergeSort(size_t startSize, size_t endSize)
		{
			size_t dataSize = std::rand() % (endSize - startSize) + startSize;
			std::vector<std::string> testData(dataSize);
			for (size_t i = 0; i < dataSize; i++)
			{
				testData[i] = generateRandomWord(START_WORD_SIZE, END_WORD_SIZE);
			}

			return testData;
		}

		std::vector<std::string> generateTestDataFromSingleWord()
		{
			size_t dataSize = 1;
			std::vector<std::string> testData(dataSize);
			testData[0] = generateRandomWord(START_WORD_SIZE, END_WORD_SIZE);

			return testData;
		}

		bool isContentsFileEqualExpected(std::ifstream &file, 
										 const std::vector<std::string> &expected)
		{
			std::string currentWord;
			size_t index = 0;
			while (std::getline(file, currentWord)) 
			{
				if (index >= expected.size() || expected[index] != currentWord) 
				{
					return false;
				}
				index++;
			}
			return true;
		}

		void generateTestDataFile(std::string fileName, const std::vector<std::string> &testData) 
		{
			std::ofstream testFile(fileName);
			if (!testFile.is_open())
			{
				throw std::exception();
			}
			writeFile(testData, testFile);
			testFile.close();
		}

		bool test(const std::vector<std::string>& data1, 
				  const std::vector<std::string>& data2,
				  const std::vector<std::string>& expected) 
		{
			generateTestDataFile("testData1.txt", data1);
			generateTestDataFile("testData2.txt", data2);

			std::ifstream testFile1("testData1.txt");
			std::ifstream testFile2("testData2.txt");
			std::ofstream mergeResult("mergeResult.txt");
			if (!testFile1.is_open() ||
				!testFile2.is_open() ||
				!mergeResult.is_open())
			{
				throw std::exception();
			}
			mergeFiles(testFile1, testFile2, mergeResult);
			testFile1.close();
			testFile2.close();
			mergeResult.close();
			remove("testData1.txt");
			remove("testData2.txt");

			std::ifstream result("mergeResult.txt");
			if (!result.is_open())
			{
				throw std::exception();
			}
			bool testResult = isContentsFileEqualExpected(result, expected);
			result.close();
			remove("mergeResult.txt");

			return testResult;
		}

	public:

		TEST_METHOD(RandomDataSmallSize)
		{
			std::srand(time(0));
			std::vector<std::string> expected = generateRandomTestDataForMergeSort(START_DATA_SIZE_1, END_DATA_SIZE_1);
			std::vector<std::string> testData1, testData2;
			for (auto element: expected) 
			{
				if (std::rand() % 2) 
				{
					testData1.push_back(element);
				}
				else 
				{
					testData2.push_back(element);
				}
			}
			std::sort(expected.begin(), expected.end());
			std::sort(testData1.begin(), testData1.end());
			std::sort(testData2.begin(), testData2.end());

			Assert::IsTrue(test(testData1, testData2, expected));
		}

		TEST_METHOD(RandomDataBigSize)
		{
			std::srand(time(0));
			std::vector<std::string> expected = generateRandomTestDataForMergeSort(START_DATA_SIZE_2, END_DATA_SIZE_2);
			std::vector<std::string> testData1, testData2;
			for (auto element : expected)
			{
				if (std::rand() % 2)
				{
					testData1.push_back(element);
				}
				else
				{
					testData2.push_back(element);
				}
			}
			std::sort(expected.begin(), expected.end());
			std::sort(testData1.begin(), testData1.end());
			std::sort(testData2.begin(), testData2.end());

			Assert::IsTrue(test(testData1, testData2, expected));
		}

		TEST_METHOD(SingleWordInData)
		{
			std::srand(time(0));
			std::vector<std::string> expected(2);
			for (size_t i = 0; i < expected.size(); i++) 
			{
				expected[i] = generateRandomWord(START_WORD_SIZE, END_WORD_SIZE);
			}
			std::vector<std::string> testData1, testData2;
			for (auto element : expected)
			{
				if (std::rand() % 2)
				{
					testData1.push_back(element);
				}
				else
				{
					testData2.push_back(element);
				}
			}
			std::sort(expected.begin(), expected.end());
			std::sort(testData1.begin(), testData1.end());
			std::sort(testData2.begin(), testData2.end());

			Assert::IsTrue(test(testData1, testData2, expected));
		}
	};
}
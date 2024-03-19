#include "pch.h"
#include "CppUnitTest.h"

#include "../ConsoleApplication1/lib/Sorting/Sorting.cpp"

#include <ctime>
#include <algorithm>
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SortingTests
{
	#define START_DATA_SIZE_1 10
	#define END_DATA_SIZE_1 1000
	#define START_DATA_SIZE_2 100000
	#define END_DATA_SIZE_2 1000000
	#define START_WORD_SIZE 1
	#define END_WORD_SIZE 1000
	#define START_CODE_SYMBOL 'a'
	#define END_CODE_SYMBOL 'z'

	TEST_CLASS(MergeSortTests)
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

		bool isEqual(const std::vector<std::string>& vector1, 
					 const std::vector<std::string>& vector2) 
		{
			if (vector1.size() != vector2.size()) 
			{
				return false;
			}
			for (size_t i = 0; i < vector1.size(); i++) 
			{
				if (vector1[i] != vector2[i]) 
				{
					return false;
				}
			}
			return true;
		}

	public:

		TEST_METHOD(RandomDataSmallSize)
		{
			std::srand(time(0));
			std::vector<std::string> testData = generateRandomTestDataForMergeSort(START_DATA_SIZE_1, END_DATA_SIZE_1);
			std::vector<std::string> expexted = testData;
			std::sort(expexted.begin(), expexted.end());

			mergeSort(testData);
			Assert::IsTrue(isEqual(expexted, testData));
		}

		TEST_METHOD(RandomDataBigSize)
		{
			std::srand(time(0));
			std::vector<std::string> testData = generateRandomTestDataForMergeSort(START_DATA_SIZE_2, END_DATA_SIZE_2);
			std::vector<std::string> expexted = testData;
			std::sort(expexted.begin(), expexted.end());

			mergeSort(testData);
			Assert::IsTrue(isEqual(expexted, testData));
		}

		TEST_METHOD(SingleWordInData)
		{
			std::srand(time(0));
			std::vector<std::string> testData = generateTestDataFromSingleWord();
			std::vector<std::string> expexted = testData;
			std::sort(expexted.begin(), expexted.end());

			mergeSort(testData);
			Assert::IsTrue(isEqual(expexted, testData));
		}
	};
}

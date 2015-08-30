
#pragma once


#include <fstream>
#include <map>
#include <vector>



class SubtitleCorrector
{
public:
	SubtitleCorrector(std::string fileName, std::string outFileName);

	~SubtitleCorrector();

	bool CheckErrors();

	void ShiftSub();


	void SetCorrections(std::vector<std::string> times, std::vector<int> shifts);

private:
	std::ifstream inFile;
	std::ofstream outFile;


	std::string subName, corrName;


	void writeOutLine(std::string line);

	void ShiftTime(int& Hour, int& Min, int& Sec, int& Ms);

	unsigned long long SumMs(int Hour, int Min, int Sec, int Ms);

	std::string ShiftLine(std::string line);

	unsigned long long GetMillis(std::string time);

	std::map<unsigned long long, int> corrections;

	unsigned long long GetCorrectedTime(unsigned long long time);
	

};



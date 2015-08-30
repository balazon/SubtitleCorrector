#include "SubtitleCorrector.h"
#include <sstream>
#include <cstdio>
#include <iostream>
#include <cstdlib>

#include <regex>


SubtitleCorrector::SubtitleCorrector(std::string fileName, std::string outFileName) : inFile{ fileName }, outFile{ outFileName }, subName{ fileName }, corrName{ outFileName }
{

}


SubtitleCorrector::~SubtitleCorrector()
{
}

bool SubtitleCorrector::CheckErrors()
{
	bool ret = false;
	if (!inFile.is_open())
	{
		std::cout << "ERROR: can't read " << subName << "\n";
		ret = true;
	}

	if (!outFile.is_open())
	{
		std::cout << "ERROR: can't write " << corrName << "\n";
		ret = true;
	}

	return ret;
}


void SubtitleCorrector::ShiftSub()
{
	std::string line;

	std::regex pat{ "[0-9]{2}", std::regex_constants::extended };
	std::string test{ "33" };
	
	if (std::regex_match(test, pat))
	{
		//std::cout << "test matches\n";
	}
	else
	{
		std::cout << "test does not match - it is likely that the program will not work \n (you might need a different / newer compiler (GCC 4.9.x or VS should work) \n";
	}

	while (std::getline(inFile, line))
	{
		//std::cout << "line (" << line.size() << " chars): " << line << "\n";

		try
		{
			//00:00:05,110 --> 00:00:07,710
			if (std::regex_match(line, std::regex{ "^[0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3} --> [0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3}$", std::regex_constants::extended }))
			{
				std::string newLine = ShiftLine(line);
				writeOutLine(newLine);
			}
			else
			{
				writeOutLine(line);
			}
		}
		catch (std::regex_error& e)
		{
			std::cout << "regex error: code" << e.code() << "\n";
			writeOutLine(line);
		}

	};
}

void SubtitleCorrector::writeOutLine(std::string line)
{
	outFile << line << std::endl;
}


void SubtitleCorrector::ShiftTime(int& Hour, int& Min, int& Sec, int& Ms)
{
	unsigned long long sum = SumMs(Hour, Min, Sec, Ms);
	sum = GetCorrectedTime(sum);

	Ms = sum % 1000;
	sum /= 1000;
	Sec = sum % 60;
	sum /= 60;
	Min = sum % 60;
	sum /= 60;
	Hour = sum;
}

unsigned long long SubtitleCorrector::SumMs(int Hour, int Min, int Sec, int Ms)
{
	return ((Hour * 60 + Min) * 60 + Sec) * 1000 + Ms;
}

std::string SubtitleCorrector::ShiftLine(std::string line)
{
	int fromHour, fromMin, fromSec, fromMs, toHour, toMin, toSec, toMs;

	int scanRes = sscanf(line.c_str(), "%d:%d:%d,%d --> %d:%d:%d,%d", &fromHour, &fromMin, &fromSec, &fromMs, &toHour, &toMin, &toSec, &toMs);
	if (scanRes != 8)
	{
		//fail
		std::cout << "ERROR: failed to read line : " << line << std::endl;
		return line;
	}

	std::istringstream iss;


	char buffer[50];


	ShiftTime(fromHour, fromMin, fromSec, fromMs);
	ShiftTime(toHour, toMin, toSec, toMs);

	sprintf(buffer, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d", fromHour, fromMin, fromSec, fromMs, toHour, toMin, toSec, toMs);
	std::string newLine{ buffer };

	return newLine;

}

void SubtitleCorrector::SetCorrections(std::vector<std::string> times, std::vector<int> shifts)
{
	for (int i = 0; i < times.size(); i++)
	{
		corrections[GetMillis(times[i])] = shifts[i];
	}
}

unsigned long long SubtitleCorrector::GetMillis(std::string time)
{
	int Hour, Min, Sec, Ms;
	int scanRes = sscanf(time.c_str(), "%d:%d:%d,%d", &Hour, &Min, &Sec, &Ms);
	if (scanRes != 4)
	{
		std::cout << "ERROR reading time: " << time << "\n";
		return 0;
	}
	return SumMs(Hour, Min, Sec, Ms);
}

unsigned long long SubtitleCorrector::GetCorrectedTime(unsigned long long time)
{
	auto lower = corrections.lower_bound(time);
	auto upper = corrections.upper_bound(time);
	
	long long corr = 0;

	

	if (upper == corrections.end())
	{
		corr = corrections.rbegin()->second;
	}
	else if (lower == corrections.begin())
	{
		corr = corrections.begin()->second;
	}
	else
	{
		lower--;
		double weight = (double)(time - lower->first) / (upper->first - lower->first);

		corr = lower->second * (1.0 - weight) + weight * upper->second;
	}	


	if (corr < 0 && time < -corr)
	{
		return 0;
	}
	else
	{
		return time + corr;
	}

}
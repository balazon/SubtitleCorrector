#include "SubtitleCorrector.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

int main(int argc, char **argv)
{

	std::string fileIn{ "sub.srt" };
	std::string fileOut{ "sub2.srt" };


	

	if (argc < 5 || argc % 2 != 1)
	{
		std::cout << "Usage:\n";
		std::cout << " SubtitleCorrector.exe [input filename] [output filename] [time1] [correction1] [time2] [correction2] ...\n";
		std::cout << "Example:\n";
		std::cout << " SubtitleCorrector.exe sub.srt sub2.srt 00:00:03,990 5000 00:40:14,630 -7000\n";
		std::cout << "(2000 ms is added to every line in sub.srt, result will be in sub2.srt)\n";
		return 0;
	}
	fileIn = std::string{ argv[1] };
	fileOut = std::string{ argv[2] };
	//ms = strtol(argv[3], nullptr, 10);
	
	std::cout << "Subtitle Corrector sub: " << fileIn << ", correctedSub: " << fileOut << "\n";
	SubtitleCorrector sc{ fileIn, fileOut };

	std::vector<std::string> times;
	std::vector<int> shifts;

	for (int i = 3; i < argc; i += 2)
	{
		times.push_back(std::string{ argv[i] });
		shifts.push_back(strtol(argv[i + 1], nullptr, 10));
	}
	
	sc.SetCorrections(times, shifts);

	if (sc.CheckErrors())
	{
		return 0;
	}

	std::cout << "parsing and shifting..\n";

	sc.ShiftSub();

	std::cout << "Done.\n";
}

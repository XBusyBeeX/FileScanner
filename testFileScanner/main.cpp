/****************************************************************************
**				TEST - multithread file scanner.
** this is an application for testing a library of multithreaded file scanning.
**
** how to run:
**		<appname> [command1] [command2]
**	commands:
**		-i [filename] - input from file
**		-o [filename] - output to file
**		-k	[count]	  - count threads
**
** Date: 13.8.2021
** Creator: Konstantin Prigoda
****************************************************************************/

#include <iostream>
#include <thread>
#include <atomic>

#include "FilesEnumerator.h"
#include "FilesScanner.h"
#include "../FileScannerLib/FileMapping.h"

#include "ArgsHandlers.h"
#include <fstream>


using namespace std;
#pragma comment (lib, "FileScannerLib.lib")

int main(int argc, char* argv[])
{

	ArgsHandlers a(argc, argv);

	int count_threads = 1;
	int thr_num = 0;
	std::string outFileName;
	std::ofstream outFile;
	FilesEnumerator *fenum = new FilesEnumerator;
	FilesScanner *fscan = nullptr;
	bool isRunning = false;
	int count_files = 0;

	long tm;


	a.insert(
		"-k",
		"count threads",
		[&count_threads](const std::string& arg)
		{
			count_threads = std::stoi(arg);
			if (count_threads < 1)
				count_threads = 1;
		}
	);

	a.insert(
		"-i",
		"input filename",
		[&fenum](const std::string& arg)
		{
			std::string line;
			std::ifstream file;

			file.open(arg);

			if (file.is_open())
			{
				while (std::getline(file, line))
				{
					fenum->append(line);
				}
			}

		}
	);

	a.insert(
		"-o",
		"output filename",
		[&outFileName](const std::string& arg)
		{
			outFileName = arg;
		}
	);

	a.exec();

	outFile.open(outFileName);

	if (!outFile.is_open())
	{
		return 1;
	}

	outFile.clear();
	outFile << "PATH" << "," << "CRC" << std::endl;

	
	tm = clock();

	fscan = new FilesScanner[count_threads];

	
	fenum->start();

	
		
	for (int i = 0; i < count_threads; i++)
	{
		fscan[i].start();
	}
	

	do 
	{
		isRunning = fenum->running();

		for (int i = 0; i < count_threads; i++)
		{
			if (!isRunning)
				fscan[i].stop();

			if (
				(isRunning = (fscan[i].running() || fscan[i].waiting()))
				)
				break;
		}

		{
			FilesEnumeratorResult result = fenum->get();

			if (result.size)
				fscan[thr_num].append(result);
		}

		{
			FilesScannerResult result = fscan->get();

			if (result.crc)
			{
				outFile << "\"" << result.path << "\"," << std::to_string(result.crc) << std::endl;
				count_files++;
			}
		}

		if (++thr_num == count_threads)
			thr_num = 0;

	} while (isRunning);

	tm = clock() - tm;

	std::cout << "time: " << tm << "ms." << std::endl;
	std::cout << "count files: " << count_files << std::endl;


	return 0;
}
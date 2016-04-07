// ReaderWriterOutput.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <list>
#include <SDL.h>
#include <fstream>
#include <string>


int nr = 0; //num of readers

//Data access semaphore
SDL_sem* rw = NULL;
SDL_sem* mutexR = NULL;

int Reader(void * data);
int Writer(void * data);


int _tmain(int argc, _TCHAR* argv[])
{
	std::list<SDL_Thread*> readers;
	std::list<SDL_Thread*> writers;

	//Initialize semaphore
	rw = SDL_CreateSemaphore(1);
	mutexR = SDL_CreateSemaphore(1);

	for (int i = 0; i < 10; i++)
	{
		readers.push_back(SDL_CreateThread(Reader, "Readers", NULL));
		writers.push_back(SDL_CreateThread(Writer, "Writers", &i));
	}

	std::cin.get();
	return 0;
}

int Readers(void * data)
{
	while (true)
	{

		SDL_SemWait(mutexR);//Lock in mutexR
		nr = nr + 1;
		
		if (nr == 1)
		{
			SDL_SemWait(rw);//lock in rw 
		}
		SDL_SemPost(mutexR);//releases lock

		
		std::cout << "Reading the Textfile" << std::endl;

		std::ifstream myfile("threadFile.txt");//ifstream is used for writng and reading
		std::string line;

		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				std::cout << line << '\n';
			}
			myfile.close();
		}

		SDL_SemWait(mutexR);//lock mutexR
		nr = nr - 1;
		if (nr == 0)
		{
			SDL_SemPost(rw);//if last thread, release the lock
		}
		SDL_SemPost(mutexR);//release mutexR

		//Wait randomly to give writers a chance to acquire lock
		SDL_Delay(16 + rand() % 640);//lazyfoo master of disaster
	}
	return 0;
}
int Writers(void * data)
{
	int i = *(int *)data;
	while (true)
	{
		SDL_SemWait(rw);//lock rw
		std::cout << "Writing to the Textfile" << std::endl;

		std::ofstream myfile;
		myfile.open("threadFile.txt");//Open text file
		myfile << "Achievement Number: " << i << std::endl; //Output to text file
		myfile.close();//Close text file

		SDL_SemPost(rw);//releases lock

		//Wait randomly to give writers a chance to acquire lock
		SDL_Delay(16 + rand() % 640);
	}
	return 0;
}


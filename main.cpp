#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <mpi.h>

#include <string>
#include <cstring>
#include "cubehash.h"

using std::string;
using std::to_string;
using std::cout;
using std::cin;
using std::endl;

int main(int argc, char* argv[])
{
	int myRank, numProcs, finished = 0;
	MPI_Status status, finish;
	MPI_Request request;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	
	long long passwords;
	int hashStrLen, startTime;
	string hash;
	char *cunknownPass;
	CubeHash unknownPass, knownPass;

	hashStrLen = unknownPass.hashStrLength();
	cunknownPass = new char[hashStrLen];

	if (myRank == 0)
	{
		string pass;
		CubeHash passHash;
		cout << "Using " << unknownPass.hashBitLength() << "-bit Cubehash algorithm" << endl;
		cout << "Processes count = " << numProcs << endl;

		//if (argc != 3)
		//{
		//	cout << "Enter your hash: ";
		//	cin >> hash;
		//	cout << "Enter number of digits: ";
		//	cin >> passwords;
		//}
		//else
		//{
		//	passwords = atoi(argv[2]);
		//	hash = string(argv[1]);
		//}
		//if (hash.length() != hashStrLen)
		//{
		//	cout << "Hash length must be " << hashStrLen << " characters!" << endl;
		//	MPI_Finalize();
		//	system("pause");
		//	return 0;
		//}

		cout << "Enter your pass: ";
		cin >> pass;
		if (strspn(pass.c_str(), "0123456789") != pass.size())
		{
			cout << "Only digits accepted" << endl;
			MPI_Abort(MPI_COMM_WORLD, 1);
			return 1;
		}
		passHash.setMessage(pass);
		passHash.show();
		hash = passHash.getHash();
		passwords = pass.length();

		strcpy(cunknownPass, hash.c_str());
		startTime = clock();
		passwords = (long long)pow(10, passwords);

		cout << "Brute-force search..." << endl;
		for (int i = 1; i < numProcs; i++)
		{
			MPI_Send(cunknownPass, hashStrLen + 1, MPI_CHAR, i, 1, MPI_COMM_WORLD);
			MPI_Send(&passwords, 1, MPI_LONG_LONG, i, 2, MPI_COMM_WORLD);
			MPI_Send(&startTime, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
		}
	}
	else
	{
		MPI_Recv(cunknownPass, hashStrLen + 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&passwords, 1, MPI_LONG_LONG, 0, 2, MPI_COMM_WORLD, &status);
		MPI_Recv(&startTime, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
		unknownPass.setHash(string(cunknownPass));
		cout << "Process " << myRank << " : " << (myRank - 1) * passwords / (numProcs - 1) << " - " << myRank * passwords / (numProcs - 1) << endl;
		for (int i = (myRank - 1) * passwords / (numProcs - 1); i < myRank * passwords / (numProcs - 1); i++)
		{
			knownPass.setMessage(to_string(i));
			if (unknownPass == knownPass)
			{ 
				cout << endl << "Your password: " << i << endl;
				cout << "Time elapsed: " << (clock() - startTime) / CLOCKS_PER_SEC << " seconds." << endl;
				for (int i = 1; i < numProcs; i++)
				{
					MPI_Isend(&finished, 1, MPI_INT, i, 4, MPI_COMM_WORLD, &request);
				}
			}
			MPI_Iprobe(MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, &finished, &finish);
			if (finished) 
				break;
		}
	}

	MPI_Finalize();
	cout << "Process " << myRank << " of " << numProcs << " finished" << endl;
	//system("pause");

	return 0;

}
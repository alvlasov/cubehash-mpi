#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <mpi.h>

#include <string>
#include <cstring>
#include "cubehash.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;

int main(int argc, char* argv[])
{
	int myRank;
	int numProcs;
	char host[256];
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs); // number of processes involved in run 
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank); // my process id: 0 <= myRank < numProcs 
	gethostname(host, sizeof(host) / sizeof(host[0])); // machine we are running on 

	string message;
	if (argc != 2)
	{
		cout << "Enter message: ";
		cin >> message;
	}
	else
	{
		message = string(argv[1]);
	}

	CubeHash h1(message);
	h1.show();

	CubeHash h2("lol");
	h2.show();

	cout << (h2 == h1) << endl;
	CubeHash h3;
	h3.setHash("faf54cc53a7eb37a07c8bcf62dd28f14");
	h3.show();
	cout << (h2 == h3) << endl;

	h3.setMessage("lol");
	h3.show();
	cout << (h2 == h3) << endl;

	cout << "Process " << myRank << " of " << numProcs << " is running on '" << host << "'." << endl;

	MPI_Finalize();
	system("pause");
	return 0;

}
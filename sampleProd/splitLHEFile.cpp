using namespace std;
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>


int makeOne(const char* inFileName, const char* outFileName, int eventsToSkip , int eventsToCopy ){
	std::cout << "inFileName = " << inFileName << std::endl;
	std::cout << "eventsToSkip = " << eventsToSkip << std::endl;
	std::cout << "eventsToCopy = " << eventsToCopy << std::endl;
	
	
	
	std::ifstream inFile(inFileName, std::ios::in);
	std::ofstream outFile(outFileName, std::ios::out);
	
	std::string line;
	bool skipEvent = false;
	int eventIt = 0;
	
	int nFilled = 0;
	
	while(!inFile.eof())
	{
		getline(inFile, line);
		
	if( line.find("<event") != std::string::npos )	
		//if( line == "<event>" )
		{
			++eventIt;
			if( eventIt <= eventsToSkip )
				skipEvent = true;
			if( eventIt > eventsToSkip+eventsToCopy )
				skipEvent = true;
			if(!skipEvent) nFilled++;
		}
		
		
		if(skipEvent == false)
			outFile << line << std::endl;
		
//		cout << line << " "<< skipEvent<<" "<< (line == "<event>" ) <<" "<< ( eventIt <= eventsToSkip ) <<" "<<( eventIt > eventsToSkip+eventsToCopy ) <<" "<< ( line == "</event>" ) <<endl;
		
		if( line.find("</event") != std::string::npos )
		//if( line == "</event>" )
			skipEvent = false;
	}
	return nFilled;
}


int main(int argc, char** argv)
{
	if(argc != 4)
	{
		std::cout << ">>>splitLheFile.cpp::Usage:   " << argv[0] << "   inFile.lhe   prefix  eventsPerFile " << std::endl;
		return -1;
	}
	
	char* inFileName = argv[1]; 
	char* prefix    = argv[2]; 
	int eventsPerFile     = atoi(argv[3]);
	
	std::cout << "inFileName = " << inFileName << std::endl;
	std::cout << "prefix = " << prefix << std::endl;
	std::cout << "eventsPerFile = " << eventsPerFile << std::endl;
	
	int  iJ = 0;
	while(true){
		string str = prefix;
		str += "_" + to_string(iJ) + ".lhe";
		
		if(!makeOne(inFileName,str.c_str(),iJ*eventsPerFile,eventsPerFile)){
			remove(str.c_str());
			break;
		} 
		iJ++;
	}
	
	return 0;
}

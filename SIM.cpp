/*
Project : 2
"On my honor, I have neither given nor received unauthorized aid on this assignment"

	Author : Nishit Sadhwani
	UFID   : 5393-5598  
*/
//Clean Code after Removing Debug Comments
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <bitset>
#include <stdlib.h>

using namespace std;

vector<string> Original;
vector<string> Compressed;
vector<string> Decompressed;
vector<string> CompressedInput;
vector<string> Dictionary;
vector<string> SortOriginal;
vector<string> CountSorted;	// Vector to count Frequency and Sort them
vector<string> Reference;
vector<string> IntermediateDecompression;
int GlobalBuffer =0; // Used for Decompression and to detect padding bits

//vector<string> IndexSorted;

// Function to take input
void TakeInput();

// Function to sort according to the frequency
void FrequencySort();

// Function to sort according to index
void IndexSort();

// Check which BitCode arrtived first and sort accordingly 
void CheckFirstArrival(string& , string&);

// Making the final dictionary
void MakeDictionary();

// Map Original Enteries with Dictonary data
void MapOrginalToDictionary();

// After Making Reference , Make Compressed file in the given format
void CompressFile();

// Reinitializing the temp array , for useful padding , marked 'x'
void ReInitialize(char *);

// Output to compressed file , cout.txt
void WriteCompressed();

// Step by Step Compression
void ExecuteCompression();
//---------------------------------------Decompression Functions-----------------------
// Step by Step Decompression
void ExecuteDecompression();

// Take Compressed Input
void TakeCompressedInput();

// Separate Compression and Dictionary into Reference and Dictionary
void SeparateCompressionAndDictionary();

// To Parse Reference , bit by bit
void ParseReference();

// To ReInitialize Char array - Decompression
void ReInitializeDecompression();


// To Parse and Match
void GoMatch(char * , int );
void DirectMatch(char * , int );
void NoMatch(char * , int );
void TwoBitConsecutiveMatch(char * , int );
void TwoBitAnywhereMatch(char * , int );

// Print Intermediate
void PrintIntermediate();

// To decompress Finally
void FinalDecompress(string , int);

// Map Dictionary, Used by Final Decompress to get the dictionary data
string MapDictionary(string);

// Writing data to dout file
void WriteDecompressed();

int main(int argc , char *argv[]){
	if( argc != 2){
		cout<<"\n Incorrect Usage";
		cout<<"\n Use ./SIM 1 - For Compression";
		cout<<"\n Use ./SIM 2 - For Decompression";
	}
	int argument = stoi(argv[1],nullptr);
	switch(argument){
		case 1: //cout<<"\n Compression \n";
				ExecuteCompression();
		break;
		case 2: //cout<<"\n Decompression \n";
				ExecuteDecompression();
		break;
		default: cout<<"\n Incorrect Usage \n";
	}
}
void TakeInput(){
	ifstream InputStream;
	//ofstream OutputStream;
	string BitCodes;
	InputStream.open("original.txt");
	// Check if the file opened or not // Can Use Assert Command
	while(InputStream >> BitCodes){
		Original.push_back(BitCodes);
		SortOriginal.push_back(BitCodes);
	}
	InputStream.close();
}

void FrequencySort(){
	// Testing Sort
	sort(SortOriginal.begin(),SortOriginal.end());
	// Algo to sort according to frequency 
	string temp;
	for(int i=0; i<SortOriginal.size();i++){
		temp = SortOriginal[i];
		int count = 1;
		for(int j=i+1;j<SortOriginal.size();j++){
			if(SortOriginal[j] == temp){
				SortOriginal.erase(SortOriginal.begin()+j);
				//cout<<"Here";
				count++;
				j=j-1;
			}
		}
		CountSorted.push_back(to_string(count)+"->" + temp );
	}
	sort(CountSorted.rbegin(),CountSorted.rend()); // To sort in ascending order
}

void IndexSort(){
	for(int i=0;i<CountSorted.size()-1;i++){
		if(CountSorted[i].substr(0,3)== CountSorted[i+1].substr(0,3))
			CheckFirstArrival(CountSorted[i],CountSorted[i+1]);
	}
}

void CheckFirstArrival(string &A, string &B){
	int arrivalA = -1;
	int arrivalB = -1;
	for (int i = 0; i < Original.size(); ++i)
	{
		if(A.substr(3) == Original[i]){
			arrivalA = i;
			break;
		}
	}
	for (int i = 0; i < Original.size(); ++i)
	{
		if(B.substr(3) == Original[i]){
			arrivalB = i;
			break;
		}
	}
	if (arrivalB < arrivalA)
	{
		string temp;
		temp = B;
		B = A;
		A= temp;
	}
}
void MakeDictionary(){
	//Pusing 8 bits into dictionary with the mapping codes
	for(int i=0;i<8;i++){
		string temp = bitset<3>(i).to_string();
		Dictionary.push_back( temp +" "+CountSorted[i].substr(3));
	}
}
void MapOrginalToDictionary(){
	int flag = 0;// Not Pushed
	for(int i = 0;i!=Original.size();i++){
		for(int j = 0; j != Dictionary.size() ; j++){
			if(Original[i] == Dictionary[j].substr(4)){
				//Reference.push_back(Dictionary[j].substr(0,3) + " " + Original[i]);
				Reference.push_back( "00 "+ Dictionary[j].substr(0,3));
				flag=1;
				break;
			}
			else
				flag=0;
		}
		if(flag==0)
		{
			int ConsecutiveMismatch = 0;
			int AnyWhereMismatch = 0;
			int ConsecutiveStartingBit = -1;
			int AnyWhereFirstBit = -1;
			int AnyWhereSecondBit = -1;
			int MismatchCount = 0;

			// 2-bit Consecutive Mismatch
			for(int j= 0 ; j != Dictionary.size() ; j++){
				MismatchCount=0;
				string temp1 = Original[i];	// Cannot use iterators with this, so converted loops to int
				string temp2 = Dictionary[j].substr(4);
				// Can use set difference as well , Try later
				// Need to put an assert condition to check if both of them are equal
				for(int k=0 ; k<temp1.length() - 1 ; k++){
					if(temp1[k] != temp2[k]){
						//cout<<temp1[k]<<"!="<<temp2[k];
						if(MismatchCount==0){
							ConsecutiveStartingBit = k;
							if(temp1[k+1] != temp2[k+1])
							{
								//cout<<"Only once";
								ConsecutiveMismatch=1;
							}
						}
						MismatchCount++;
					}
				}
				if(MismatchCount==2 && ConsecutiveMismatch ==1)
				{
					//cout<<"Here";
					//cout<<"\n MismatchCount" << MismatchCount;
					string TempStartBit = bitset<5>(ConsecutiveStartingBit).to_string(); // To convert starting bit to 5 bit number
					Reference.push_back( "01 "+ TempStartBit + " " + Dictionary[j].substr(0,3));
					ConsecutiveMismatch = 1;
					break;
				}
				else
					ConsecutiveMismatch=0;
			}
			// 2-bit Anywhere Mismatch
			if(ConsecutiveMismatch == 0){
				for(int j= 0 ; j != Dictionary.size() ; j++){
				MismatchCount=0;
				string temp1 = Original[i];	// Cannot use iterators with this, so converted loops to int
				string temp2 = Dictionary[j].substr(4);
				// Can use set difference as well , Try later
				// Need to put an assert condition to check if both of them are equal
				for(int k=0 ; k<temp1.length() - 1 ; k++){
					if(temp1[k] != temp2[k]){
						if(MismatchCount==0){
							AnyWhereFirstBit = k;
							}
						if(MismatchCount==1){
							AnyWhereSecondBit = k;
							AnyWhereMismatch=1;
						}
						MismatchCount++;
						}
						
					}
					if(MismatchCount==2 && AnyWhereMismatch == 1)
					{
						string TempStartBitOne = bitset<5>(AnyWhereFirstBit).to_string(); // To convert starting bit to 5 bit number
						string TempStartBitTwo = bitset<5>(AnyWhereSecondBit).to_string();
						Reference.push_back( "10 "+ TempStartBitOne + " " + TempStartBitTwo +" "+Dictionary[j].substr(0,3));
						break;
					}
					else
						AnyWhereMismatch=0;
				}
				
			}
			// No Match
			if(AnyWhereMismatch == 0 && ConsecutiveMismatch == 0)
				Reference.push_back("11 "+ Original[i]);
		}
	}
		
}
void CompressFile(){
	// Can Remove the Spacing from Reference // In above function
	char temp[32];
	int buffer = 32;
	int k=0;
	ReInitialize(temp);
	for(int i=0;i<Reference.size();i++){
			string tempRef = Reference[i];
			for(int j=0;j<tempRef.length();j++){
				if(k<32){
					if(tempRef[j] != ' ' && tempRef[j] != '\n'){
					temp[k]=tempRef[j];
					k++;
					buffer--;
					if(k==32){
					Compressed.push_back(temp);
					ReInitialize(temp);
					k=0;
					buffer=32;
					}
					
				}

			}
				
			}
	}
	//Padding with 1's
	if(buffer!=32){
				for(int i=31;i>31-buffer;i--)
					temp[i]='1';
				Compressed.push_back(temp);
	}
	//Now Pushing Dictionary Values
	Compressed.push_back("xxxx");
	for(int i=0; i<Dictionary.size(); i++){
		Compressed.push_back(Dictionary[i].substr(4));
	}
}
void ReInitialize(char *a){
	for(int i=0;i<32;i++){
		a[i]='x';
	}
	a[32]='\0';
}

void WriteCompressed(){
	ofstream OutputStream;
	OutputStream.open("cout.txt");

	for (auto i = Compressed.begin() ; i!=Compressed.end();i++)
	{
		OutputStream<<*i<<"\n";
	}
	OutputStream.close();
}

void ExecuteCompression(){

				TakeInput();
				FrequencySort(); // Sorts According to count
				IndexSort();	// Sorts According to the first arrival , if count matches
	
				// Have to do it n times // Not just once, can give bugs
				for(int i=0;i<CountSorted.size();i++)
					IndexSort();
	

				MakeDictionary();
				MapOrginalToDictionary();
				CompressFile();
				WriteCompressed();
}
//------------------------------------------Decompression-------------------------------
void TakeCompressedInput(){
	ifstream InputStream;
	string BitCodes;
	InputStream.open("cout.txt");
	// Check if the file opened or not // Can Use Assert Command
	//int index=0;
	while(InputStream >> BitCodes){
		CompressedInput.push_back(BitCodes);
	}
	InputStream.close();
}
void SeparateCompressionAndDictionary(){
	int flag=0;
	for(int i=0;i<CompressedInput.size();i++){
		string temp = CompressedInput[i];
		if(flag==0 && temp != "xxxx"){
			Reference.push_back(CompressedInput[i]);
		}
		if(flag==1 && temp != "xxxx"){
			Dictionary.push_back(CompressedInput[i]);
		}
		if(temp == "xxxx"){
			flag=1;
		}
	}
}
void ReInitializeDecompression(char *a, int size){
	for(int i=0;i<size;i++){
		a[i]='x';
	}
	a[size]='\0';// Very Useful, To Prevent Garbage
}
void ParseReference(){
	//Put Everything into a single char array // Naive approach
	// Size of array = Reference size * 32
	int ReferenceSize = Reference.size();
	int CharArraySize = ReferenceSize*32; // Might Create Problem if more than 100 lines , coz int size
	//Assert that size is not 0 , can use unsigned int as well
	GlobalBuffer=CharArraySize;
	char temp[CharArraySize];
	ReInitializeDecompression(temp, CharArraySize);
	int k=0;
	for(int i=0;i<Reference.size();i++){
			string tempRef = Reference[i];
			for(int j=0;j<tempRef.length();j++){
					if(tempRef[j] != ' ' && tempRef[j] != '\n'){
						if(k<CharArraySize){
							temp[k]=tempRef[j];
							k++;
						}
						else
							cout<<"\n I went out of bound";
				}
			}	
	}
	//------------Now we have everything in a single array and parsing will be easy-----------------
	//00- buffer size 5
	//01- buffer size 10
	//10- buffer size 15
	//11- buffer size 34
	GoMatch(temp,0);
}
void GoMatch(char * TempArray , int curr){
	int currentposition=curr;
	char a = TempArray[currentposition];
	char b = TempArray[currentposition+1];
	if(a=='0'){
		if(b=='0'){
			DirectMatch(TempArray,currentposition);
		}
		if(b=='1'){
			TwoBitConsecutiveMatch(TempArray,currentposition);
		}
	}
	if(a=='1'){
		if(b=='0'){
			TwoBitAnywhereMatch(TempArray,currentposition);
		}
		if(b=='1'){
			NoMatch(TempArray,currentposition);
		}
	}
}
// Direct Dictionary Match
void DirectMatch(char * temp,int currentposition){
	int buffer = 5;
	GlobalBuffer = GlobalBuffer - buffer;
	// Might put an if/else condition to check if global buffer goes out of bounds
	string bufferString = "xx xxx";
	string DictionaryMapping = "xxx";
	int k=0;
	for(int i=0;i<6;i++){
		if(i == 2){
			bufferString[i]=' ';
		}
		else
		{
			bufferString[i]= temp[currentposition+k];
			k++;
		}
	}
	if(GlobalBuffer>-1){
		IntermediateDecompression.push_back(bufferString);
		FinalDecompress(bufferString,1); // String + Mode
		GoMatch(temp,currentposition+buffer);
		//cout<<IntermediateDecompression[0];
	}
}
// No Match 
void NoMatch(char * temp,int currentposition){
	int buffer = 34;
	GlobalBuffer = GlobalBuffer - buffer;
	// Might put an if/else condition to check if global buffer goes out of bounds
	string bufferString = "xx xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	//string DictionaryMapping = "xxx";
	int k=0;
	for(int i=0;i<35;i++){
		if(i == 2){
			bufferString[i]=' ';
		}
		else
		{
			bufferString[i]= temp[currentposition+k];
			k++;
		}
	}
	if(GlobalBuffer>-1){
		IntermediateDecompression.push_back(bufferString);
		FinalDecompress(bufferString,4); // String + Mode
		GoMatch(temp,currentposition+buffer);
	}
}
// Consecutive Match
void TwoBitConsecutiveMatch(char * temp,int currentposition){
	int buffer = 10;
	GlobalBuffer = GlobalBuffer - buffer;
	// Might put an if/else condition to check if global buffer goes out of bounds
	string bufferString = "xx xxxxx xxx";
	string DictionaryMapping = "xxx";
	int k=0;
	for(int i=0;i<12;i++){
		if(i == 2 || i==8){
			bufferString[i]=' ';
		}
		else
		{
			bufferString[i]= temp[currentposition+k];
			k++;
		}
	}
	if(GlobalBuffer>-1){
		IntermediateDecompression.push_back(bufferString);
		FinalDecompress(bufferString,2); // String + Mode
		GoMatch(temp,currentposition+buffer);
		//cout<<IntermediateDecompression[0];
	}
}
// 2-bit Anywhere match
void TwoBitAnywhereMatch(char * temp,int currentposition){
	int buffer = 15;
	GlobalBuffer = GlobalBuffer - buffer;
	// Might put an if/else condition to check if global buffer goes out of bounds
	string bufferString = "xx xxxxx xxxxx xxx";
	string DictionaryMapping = "xxx";
	int k=0;
	for(int i=0;i<18;i++){
		if(i == 2 || i == 8 || i==14){
			bufferString[i]=' ';
		}
		else
		{
			bufferString[i]= temp[currentposition+k];
			k++;
		}
	}
	if(GlobalBuffer>-1){
		IntermediateDecompression.push_back(bufferString);
		FinalDecompress(bufferString,3);
		GoMatch(temp,currentposition+buffer);
	}
}
void PrintIntermediate(){
	for (auto i = Decompressed.begin() ; i!=Decompressed.end();i++)
	{
		cout<<*i<<"\n";
	}
	/*for (auto i = IntermediateDecompression.begin() ; i!=IntermediateDecompression.end();i++)
	{
		cout<<*i<<"\n";
	}*/
}
void FinalDecompress(string data , int mode){
	string DictionaryData;
	string DictionaryMatch="xxx";
	char StartBit[6] = {'x','x','x','x','x','\0'};
	char FirstBit[6] = {'x','x','x','x','x','\0'};
	char SecondBit[6] = {'x','x','x','x','x','\0'};
	int StartBitLocation;
	int FirstBitLocation;
	int SecondBitLocation;

	switch(mode){
		case 1 : 
				 DictionaryMatch[0]= data[3];
				 DictionaryMatch[1]= data[4];
				 DictionaryMatch[2]= data[5];
				 DictionaryData = MapDictionary(DictionaryMatch);
				 Decompressed.push_back(DictionaryData);
		break;
		case 2 : 
				 DictionaryMatch[0]= data[9];
				 DictionaryMatch[1]= data[10];
				 DictionaryMatch[2]= data[11];
				 DictionaryData = MapDictionary(DictionaryMatch);
				 //Start Bit Map
				 StartBit[0] = data[3];
				 StartBit[1] = data[4];
				 StartBit[2] = data[5];
				 StartBit[3] = data[6];
				 StartBit[4] = data[7];
				 StartBitLocation = strtol(StartBit,nullptr,2);
				 if(DictionaryData[StartBitLocation] == '0'){
				 	DictionaryData[StartBitLocation] = '1';
				 }
				 else{
				 	DictionaryData[StartBitLocation] = '0';
				 }
				 if(DictionaryData[StartBitLocation+1] == '0'){
				 	DictionaryData[StartBitLocation+1] = '1';
				 }
				 else{
				 	DictionaryData[StartBitLocation+1] = '0';
				 }
				 Decompressed.push_back(DictionaryData);
		break;
		case 3 : //cout<<"\n Two Bit Anywhere Match";
				 DictionaryMatch[0]= data[15];
				 DictionaryMatch[1]= data[16];
				 DictionaryMatch[2]= data[17];
				 DictionaryData = MapDictionary(DictionaryMatch);
				 //Changing First Bit
				 FirstBit[0] = data[3];
				 FirstBit[1] = data[4];
				 FirstBit[2] = data[5];
				 FirstBit[3] = data[6];
				 FirstBit[4] = data[7];
				 FirstBitLocation = strtol(FirstBit,nullptr,2);
				 if(DictionaryData[FirstBitLocation] == '0'){
				 	DictionaryData[FirstBitLocation] = '1';
				 }
				 else{
				 	DictionaryData[FirstBitLocation] = '0';
				 }
				 //Changing Second Bit
				 SecondBit[0] = data[9];
				 SecondBit[1] = data[10];
				 SecondBit[2] = data[11];
				 SecondBit[3] = data[12];
				 SecondBit[4] = data[13];
				 SecondBitLocation = strtol(SecondBit,nullptr,2);
				 if(DictionaryData[SecondBitLocation] == '0'){
				 	DictionaryData[SecondBitLocation] = '1';
				 }
				 else{
				 	DictionaryData[SecondBitLocation] = '0';
				 }
				 Decompressed.push_back(DictionaryData);
		break;
		case 4 : 
				 Decompressed.push_back(data.substr(3));
		break;
		default : cout<<"\n Some Error in Final Decompress";
	}
}
string MapDictionary(string data){
	if(data == "000"){
		return Dictionary[0];
	}
	if(data == "001"){
		return Dictionary[1];
	}
	if(data == "010"){
		return Dictionary[2];
	}
	if(data == "011"){
		return Dictionary[3];
	}
	if(data == "100"){
		return Dictionary[4];
	}
	if(data == "101"){
		return Dictionary[5];
	}
	if(data == "110"){
		return Dictionary[6];
	}
	if(data == "111"){
		return Dictionary[7];
	}
}
void WriteDecompressed(){
	ofstream OutputStream;
	OutputStream.open("dout.txt");

	for (auto i = Decompressed.begin() ; i!=Decompressed.end();i++)
	{
		OutputStream<<*i<<"\n";
	}
	OutputStream.close();
}
void ExecuteDecompression(){
	TakeCompressedInput();
	SeparateCompressionAndDictionary();
	ParseReference();
	WriteDecompressed();

}

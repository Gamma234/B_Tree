#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <ctime>
#include <math.h>
#include <iomanip>

#include "B_Tree.h"

using namespace std ;

struct Data {
	int value ;
	Data( int num ) : value( num ) {} // constructor
} ;

bool operator>=( Data data1, Data data2 ) {
	
	return data1.value >= data2.value ;
	
} // operator>=() 

bool operator==( Data data1, Data data2 ) {
	
	return data1.value == data2.value ;
	
} // operator==() 

bool operator!=( Data data1, Data data2 ) {
		
	return data1.value != data2.value ;
	
} // operator!=() 

ostream& operator<<( ostream& os, Data data ) {
	
	os << data.value ;
	
} // operator<<() 

bool operator<=( Data data, int num ) {
		
	return data.value <= num ;
	
} // operator<=() 

bool operator>=( Data data, int num ) {
		
	return data.value >= num ;
	
} // operator>=() 

bool operator==( Data data, int num  ) {
		
	return data.value == num ;
	
} // operator==() 

int main() {
	
	BST::BTree<Data,int> tree( 30, true ) ; 
	
	string temp = "" ;
	string inputStr = "test_data//input10000a.txt", delStr = "test_data//input100a.txt" ;
	
	fstream inFile( inputStr.c_str(), ios::in ) ;
	
	getline( inFile, temp ) ;
	cout << temp << endl ;
	
	int num = 0 ;
	
	while ( inFile >> num ) {
		tree.Insert( Data( num ) ) ;		
	} // while	
	
	cout << "-----Input-----" << endl ;
	//tree.OutputTree() ;
	tree.OutputFile( "NewResult.txt" ) ;
	cout << "---------------" << endl ;
	
	inFile.close() ;
	
	vector<Data> * dataSet = tree.Search( 4523 ) ;
	
	for ( int i = 0 ; i < dataSet->size() ; i++ ) {
		cout << (*dataSet)[i].value << endl ;
	} // for 	
	
	//return 0 ;
	// -------------------------------------
	
	fstream delFile( delStr.c_str(), ios::in ) ;
	
	getline( delFile, temp ) ;
	cout << temp << endl ;
	
	while ( delFile >> num ) {
		tree.Delete( Data( num ) ) ;		
	} // while	
	
	cout << "-----Delete-----" << endl ;
	//tree.OutputTree() ;
	tree.OutputFile( "NewDelResult.txt" ) ;
	cout << "---------------" << endl ;
	
	delFile.close() ;
	
} // main()



























#ifndef B_TREE_H_INCLUDED
#define B_TREE_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <fstream> 
#include <math.h>

using namespace std ;

namespace BST {

    /*
	operator overload :  
	    bool operator>=( Type data1, Type data2 ) { return data1.value >= data2.value ; }
    	bool operator==( Type data1, Type data2 ) { return data1.value == data2.value ; }
    	bool operator!=( Type data1, Type data2 ) ; // for deletion 
		ostream& operator<<( ostream& os, Type data ) { os << data.num1 << "  " << data.num2 << "  " << data.value ; }
		
		bool operator>=( Data data, SearchKeyType key ) ;
		bool operator<=( Data data, SearchKeyType key ) ;
		bool operator==( Data data, SearchKeyType key ) ;
	*/
	
	template<class Type, class SearchKeyType>
	class BTree {
		
		private :
			
			int gSize ; // content num 
			bool stb ; // small to big 
			
			int gNum ; // for output index 
			
			struct Node {
				Node * parent ;
				int totalSize ;
				int size ;
				vector<Type> * content ;  // content = new vector<Type>[totalSize+1] ;
				Node ** children ;        // children = new Node*[totalSize+2] ;
				Node( int gSize ) ; // constructor
				Node( int gSize, Type firstNum, Node * theParent ) ; // constructor
				Node( int gSize, Node * node, int middle ) ; // constructor
				bool InsertIntoLeaf( Type num, int index ) ; // true -> successful 
				bool SiblingDetect( bool &leftStyle, Node * &sibling, int childLoc ) ; // true : exist sibling size > 1
			} * head ; // struct Node
			
			int GoToTheWay( vector<Type> * content, int size, Type num ) ; // -1 repeat at [0], -2 repeat at [1], ...
			
			void InsertIntoTree( Node * node, Type num ) ;
			bool LeafOrNot( Node * node ) { return node->children[0] == NULL ; }
			void Split( Node * node ) ;		
			
			void DeleteInTheTree( Node * node, Type num ) ;
			int FindTheDeletedItem( int &realLoc, vector<Type> * content, int size, Type num ) ; // -1 repeat at [0], -2 repeat at [1], ...
			Node* FindSuccessor( Node * node ) { return ( LeafOrNot( node ) ? node : FindSuccessor( node->children[0] ) ) ; }
			void FixTree( Node * node ) ;
		
			int InorderOutput( Node * node, int level ) ;		
			void DeleteTree( Node * &node ) ;
			void ReleaseNode( Node * &node ) ;	
			int HeightOfTree( Node * node ) ;
			int InorderOutputFile( fstream &outFile, Node * node, int level ) ;

			int SearchTheWay( vector<Type> * content, int size, SearchKeyType key ) ; // -1 repeat at [0], -2 repeat at [1], ...
			vector<Type>* SearchItem( Node * node, SearchKeyType key ) ; 
		
		public :
			
			BTree( int order, bool smallToBig ) ; // constructor // order = children num 
			~BTree() { DeleteTree( head ) ; } // destructor 
			void Insert( Type num ) ;
			void Delete( Type num ) ;
			void OutputTree() ;
			void OutputFile( string fileName ) ;
			
			vector<Type>* Search( SearchKeyType key ) ; // return an address of vector<Type> (every item with the key) 
		
	}; // class BTree

} ; // namespace BST 

template<class Type, class SearchKeyType>
BST::BTree<Type, SearchKeyType>::BTree( int order, bool smallToBig ) : head( NULL ), gSize( order - 1 ), stb( smallToBig ), gNum( 1 ) { // constructor // order = children num 
	
	if ( order < 3 ) {
		cout << "----------------------" << endl ;
		cout << "[ order too small ]" << endl ;
		cout << "[ change order to 3 ]" << endl ;
		cout << "----------------------" << endl ;
		gSize = 2 ;
	} // if
	else {
		gSize = order - 1 ;
	} // else
	
} // BST::BTree::BTree() 

template<class Type, class SearchKeyType>
void BST::BTree<Type, SearchKeyType>::Insert( Type num ) {
	InsertIntoTree( head, num ) ;
} // BST::BTree::Insert() 

template<class Type, class SearchKeyType>
void BST::BTree<Type, SearchKeyType>::Delete( Type num ) {
	DeleteInTheTree( head, num ) ;
} // BST::BTree::Delete()

template<class Type, class SearchKeyType>
void BST::BTree<Type, SearchKeyType>::OutputTree() {
	cout << "num = " << InorderOutput( head, 1 ) << endl ;
} // BST::BTree::OutputTree()

template<class Type, class SearchKeyType>
vector<Type>* BST::BTree<Type, SearchKeyType>::Search( SearchKeyType key ) { // return an address of vector<Type> (every item with the key) 
	return SearchItem( head, key ) ; 
} // BST::BTree<Type, SearchKeyType>::Search()  

template<class Type, class SearchKeyType>
BST::BTree<Type, SearchKeyType>::Node::Node( int gSize ) : parent( NULL ), totalSize( gSize ), size( 0 ) { // constructor
	
	content = new vector<Type>[totalSize+1] ; // +1 is for full condition 
	children = new Node*[totalSize+2] ; // in +2, one +1 is for full condition 
	
	for( int i = 0 ; i < totalSize + 2 ; i++ ) { // initialize children to NULL
		children[i] = NULL ;
	} // for
			
} // BST::BTree::Node::Node() 

template<class Type, class SearchKeyType>
BST::BTree<Type, SearchKeyType>::Node::Node( int gSize, Type firstNum, Node * theParent ) : 
						 parent( theParent ), totalSize( gSize ), size( 0 ) { // constructor
	
	content = new vector<Type>[totalSize+1] ; // +1 is for full condition 
	children = new Node*[totalSize+2] ; // in +2, one +1 is for full condition 
	
	content[size].push_back( firstNum ) ;
	size++ ;
	
	for( int i = 0 ; i < totalSize + 2 ; i++ ) { // initialize children to NULL
		children[i] = NULL ;
	} // for
	
} // BST::BTree::Node::Node()

template<class Type, class SearchKeyType>
int BST::BTree<Type, SearchKeyType>::GoToTheWay( vector<Type> * content, int size, Type num ) { // -1 repeat at [0], -2 repeat at [1], ...
	
	int wayIndex = 0 ;
	
	while ( wayIndex < size && ( stb ? num >= content[wayIndex][0] : content[wayIndex][0] >= num ) ) {
		if ( num == content[wayIndex][0] ) return ( wayIndex + 1 ) * (-1) ;
		else wayIndex++ ;
	} // while
	
	return wayIndex ;
	
} // BST::BTree::GoToTheWay()

template<class Type, class SearchKeyType>
void BST::BTree<Type, SearchKeyType>::InsertIntoTree( Node * node, Type num ) {
	
	if ( head == NULL ) { // root
		head = new Node( gSize, num, NULL ) ;
	} // if
	else {
		
		int con = GoToTheWay( node->content, node->size, num ) ;
		
		if ( con < 0 ) { // Repeat 
			node->content[(con+1)*(-1)].push_back( num ) ;
			return ;
		} // if
		
		if ( LeafOrNot( node ) ) {		
			if ( ! node->InsertIntoLeaf( num, con ) ) {
				Split( node ) ;
			} // if
		} // if
		else {
			InsertIntoTree( node->children[con], num ) ;
		} // else
		
	} // else
	
} // BST::BTree::InsertIntoTree()

template<class Type, class SearchKeyType>
bool BST::BTree<Type, SearchKeyType>::Node::InsertIntoLeaf( Type num, int index ) { // true -> successful
	
	if ( index != size ) {		
		for ( int move = size - index, temp = size ; move > 0 ; move--, temp-- ) {
			content[temp] = content[temp-1] ;
		} // for
	} // if
	
	content[index].clear() ;
	content[index].push_back( num ) ;
	size++ ; 
	
	if ( size > totalSize ) return false ;
	else return true ;
	
} // BST::BTree::Node::InsertIntoLeaf()

template<class Type, class SearchKeyType>
BST::BTree<Type, SearchKeyType>::Node::Node( int gSize, Node * node, int middle ) :
						parent( node->parent ), totalSize( gSize ), size( 0 ) { // constructor
	
	content = new vector<Type>[totalSize+1] ; // +1 is for full condition 
	children = new Node*[totalSize+2] ; // in +2, one +1 is for full condition 
	
	for( int i = 0 ; i < totalSize + 2 ; i++ ) { // initialize children to NULL
		children[i] = NULL ;
	} // for
	
	int i = middle + 1 ;
	
	for ( ; i < node->size ; i++ ) {
		content[size] = node->content[i] ;
		children[size] = node->children[i] ;
		if ( children[size] != NULL ) children[size]->parent = this ; // set parent 
		size++ ;
	} // for
	
	children[size] = node->children[i] ; // last children 
	if ( children[size] != NULL ) children[size]->parent = this ; // set parent 
	
} // BST::BTree::Node::Node()  

template<class Type, class SearchKeyType>
void BST::BTree<Type, SearchKeyType>::Split( Node * node ) {
	
	Node * parentNode = NULL ;
	
	if ( node == head ) {
		parentNode = new Node( gSize ) ;
		head = parentNode ;
		node->parent = parentNode ; // set parent 
		parentNode->children[0] = node ;
	} // if
	else parentNode = node->parent ;
	
	int middle = node->size / 2 ;
	vector<Type> contentTemp = node->content[middle] ; 
	
	/* create right node */ 
	
	Node * newNode = new Node( gSize, node, middle ) ; // create a new node 
	
	for ( int i = middle + 1, sizeTemp = node->size ; i <= sizeTemp ; i++ ) {
		node->children[i] = NULL ;
		node->content[node->size-1].clear() ;
		node->size-- ;
	} // for
	
	/* put temp into parent */
	
	int loc = GoToTheWay( parentNode->content, parentNode->size, contentTemp[0] ) ; // find the inserted location
	
	for ( int move = parentNode->size - loc, temp = parentNode->size ; move > 0 ; move--, temp-- ) { // move content and children
		parentNode->content[temp] = parentNode->content[temp-1] ; // move content
		parentNode->children[temp+1] = parentNode->children[temp] ; // move children
	} // for
	
	parentNode->content[loc] = contentTemp ;
	parentNode->children[loc+1] = newNode ;
	parentNode->size++ ;
	
	if ( parentNode->size > parentNode->totalSize ) Split( parentNode ) ;	
	
} // BST::BTree::Split()  

template<class Type, class SearchKeyType>
int BST::BTree<Type, SearchKeyType>::InorderOutput( Node * node, int level ) {
	
	if ( node == NULL ) {	
		return 0 ;
	} // if
	
	int num = 0 ;
	
	for ( int i = 0 ; i <= node->size ; i++ ) {
		num = num + InorderOutput( node->children[i], level + 1 ) ;
		if ( i < node->size ) {
			for ( int j = 0 ; j < level ; j++ ) {
				cout << "  " ;
			} // for
			
			cout << "<" << node->content[i].size() << "> " ;
			
			for ( int j = 0 ; j < node->content[i].size() ; j++ ) {
				cout << node->content[i][j] << " " ;
				num++ ;
			} // for
			
			cout << " [" << level << "] " << "(" << i << ") " << node << " {" << node->parent << "} " ;
			for ( int i = 0 ; i <= node->size ; i++ ) cout << node->children[i] << " " ;
			
			cout << "NS[" << node->size << "]" ;
			
			cout << endl ;
			
		} // if
	} // for
	
	return num ;
	
} // BST::BTree::InorderOutput()

template<class Type, class SearchKeyType>
void BST::BTree<Type, SearchKeyType>::DeleteInTheTree( Node * node, Type num ) {
	
	if ( node == NULL ) {
		cout << "[" << num << " doesn't exist !]" << endl ;
		return ;
	} // if
	
	int realLoc = 0 ; // the real data in repeat 
	int con = FindTheDeletedItem( realLoc, node->content, node->size, num ) ; // -1 repeat at [0], -2 repeat at [1], ...
			
	if ( con < 0 ) { // Repeat
	
		int repeatIndex = (con+1)*(-1) ;
		
		node->content[repeatIndex].erase( node->content[repeatIndex].begin() + realLoc ) ;	
		
		if ( node->content[repeatIndex].size() != 0 ) return ; // not a empty content loc 	
		
		if ( ! LeafOrNot( node ) ) {
			Node * successor = FindSuccessor( node->children[repeatIndex+1] ) ;
			node->content[repeatIndex] = successor->content[0] ;
			node = successor ; 
			repeatIndex = 0 ;
		} // if 
		
		node->size-- ;
		
		for ( ; repeatIndex < node->size ; repeatIndex++ ) {
			node->content[repeatIndex] = node->content[repeatIndex+1] ;
		} // for 
		
		if ( node->size == 0 ) FixTree( node ) ;		
		
	} // if
	else {
		DeleteInTheTree( node->children[con], num ) ;
	} // else
	
} // BST::BTree::DeleteInTheTree() 

template<class Type, class SearchKeyType>
int BST::BTree<Type, SearchKeyType>::FindTheDeletedItem( int &realLoc, vector<Type> * content, int size, Type num ) { // -1 repeat at [0], -2 repeat at [1], ...
	
	// realLoc : the real data in repeat 
	
	int wayIndex = 0 ;
	
	while ( wayIndex < size && ( stb ? num >= content[wayIndex][0] : content[wayIndex][0] >= num ) ) {
		for ( int i = 0 ; i < content[wayIndex].size() ; i++ ) {
			if ( ! ( num != content[wayIndex][i] ) ) {
				realLoc = i ;
				return ( wayIndex + 1 ) * (-1) ;
			} // if
		} // for 
		wayIndex++ ;
	} // while
	
	return wayIndex ;
	
} // BST::BTree::FindTheDeletedItem()

template<class Type, class SearchKeyType>
void BST::BTree<Type, SearchKeyType>::FixTree( Node * node ) {
	
	// cout << "Fix Tree !" << endl ; // test 
	
	if ( node == head ) {		
		node = node->children[0] ;		
		ReleaseNode( head ) ;			
		head = node ;
		if ( head != NULL ) head->parent = NULL ;		
	} // if
	else {
		
		Node * nodeParent = node->parent ;
		bool leftStyle = true ;
		Node * sibling = NULL ;
		
		int loc = GoToTheWay( nodeParent->content, nodeParent->size, node->content[0][0] ) ;
		if ( loc < 0 ) loc = loc * (-1) ;
		
		if ( nodeParent->SiblingDetect( leftStyle, sibling, loc ) ) { // true : exist sibling size > 1 
			
			if ( leftStyle ) { // sibling locates at left side 
				
				node->content[0] = nodeParent->content[loc-1] ;
				node->size++ ;
				nodeParent->content[loc-1] = sibling->content[sibling->size-1] ;			
				
				if ( ! LeafOrNot( node ) ) {
					node->children[1] = node->children[0] ;	
					node->children[0] = sibling->children[sibling->size] ;
					if ( node->children[0] != NULL ) node->children[0]->parent = node ; // set parent 
					sibling->children[sibling->size] = NULL ;				
				} // if							
				
			} // if
			else { // rightStyle // sibling locates at right side 
			
				node->content[0] = nodeParent->content[loc] ;
				node->size++ ; 
				nodeParent->content[loc] = sibling->content[0] ;
				
				if ( ! LeafOrNot( node ) ) {
					node->children[1] = sibling->children[0] ;
					if ( node->children[1] != NULL ) node->children[1]->parent = node ; // set parent 
					sibling->children[0] = NULL ;
				} // if
				
				int temp = 0 ;
				
				for ( int move = sibling->size - 1 ; move > 0 ; move--, temp++ ) { // move content and children
					sibling->content[temp] = sibling->content[temp+1] ; // move content
					sibling->children[temp] = sibling->children[temp+1] ; // move children
				} // for 
				
				sibling->children[temp] = sibling->children[temp+1] ; // move last children
				sibling->children[temp+1] = NULL ;
				
			} // else
			
			sibling->size-- ; // decrease sibling's size
			
		} // if
		else { // false : doesn't exist sibling size > 1 
			
			if ( leftStyle ) { // sibling locates at left side 
			
				sibling->content[1] = nodeParent->content[loc-1] ;
				
				for ( int move = nodeParent->size - loc, temp = loc ; move > 0 ; move--, temp++ ) { // move content and children
					nodeParent->content[temp-1] = nodeParent->content[temp] ; // move content
					nodeParent->children[temp] = nodeParent->children[temp+1] ; // move children
				} // for
				
				nodeParent->children[nodeParent->size] = NULL ;
				
				if ( ! LeafOrNot( node ) ) {
					sibling->children[2] = node->children[0] ;
					sibling->children[2]->parent = sibling ; // set parent 
				} // if
				
			} // if
			else { // rightStyle // sibling locates at right side 
			
				for ( int move = sibling->size, j = 1 ; move > 0 ; move--, j++ ) { // move content and children
					sibling->content[j] = sibling->content[j-1] ; // move content
					sibling->children[j+1] = sibling->children[j] ; // move children
				} // for
								
				sibling->children[1] = sibling->children[0] ; // move first child 
				
				sibling->content[0] = nodeParent->content[loc] ;
				
				int temp = loc + 1 ;
				
				for ( int move = nodeParent->size - loc - 1 ; move > 0 ; move--, temp++ ) { // move content and children
					nodeParent->content[temp-1] = nodeParent->content[temp] ; // move content
					nodeParent->children[temp-1] = nodeParent->children[temp] ; // move children
				} // for
				
				nodeParent->children[temp-1] = nodeParent->children[temp] ; // move last child 
				nodeParent->children[temp] = NULL ;
				
				if ( ! LeafOrNot( node ) ) {
					sibling->children[0] = node->children[0] ;
					sibling->children[0]->parent = sibling ; 
				} // if
				
			} // else
			
			ReleaseNode( node ) ;
			
			nodeParent->size-- ;
			sibling->size++ ;
			
			if ( nodeParent->size == 0 ) FixTree( nodeParent ) ; 
			
		} // else
		
	} // else
	
} // BST::BTree::FixTree()

template<class Type, class SearchKeyType>
bool BST::BTree<Type, SearchKeyType>::Node::SiblingDetect( bool &leftStyle, Node * &sibling, int childLoc ) { // true : exist sibling size > 1
	
	/* check sibling */
	
	int siblingIndex = childLoc - 1 ;
	
	if ( siblingIndex >= 0 && children[siblingIndex]->size > 1 ) {
		sibling = children[siblingIndex] ;
		return true ;
	} // if
	
	siblingIndex = childLoc + 1 ;
	
	if ( siblingIndex <= size && children[siblingIndex]->size > 1 ) {
		leftStyle = false ;
		sibling = children[siblingIndex] ;
		return true ;
	} // if
	
	/* merge condition */
	
	siblingIndex = childLoc - 1 ;
	
	if ( siblingIndex >= 0 ) ;
	else {
		siblingIndex = childLoc + 1 ;
		leftStyle = false ;		
	} // else
	
	sibling = children[siblingIndex] ;
	return false ;
	
} // BST::BTree::Node::SiblingDetect()

template<class Type, class SearchKeyType>
void BST::BTree<Type, SearchKeyType>::DeleteTree( Node * &node ) {
	
	if ( node == NULL ) return ;
	
	for ( int i = 0 ; i <= node->size ; i++ ) {
		DeleteTree( node->children[i] ) ;
	} // for
	
	ReleaseNode( node ) ;
	
} // BST::BTree::DeleteTree()

template<class Type, class SearchKeyType>
void BST::BTree<Type, SearchKeyType>::ReleaseNode( Node * &node ) {
	
	if ( node == NULL ) return ;
	
	delete[] node->content ;
	node->content = NULL ;
	delete[] node->children ;
	node->children = NULL ;
	delete node ;
	node = NULL ;
	
} // BST::BTree::ReleaseNode()

template<class Type, class SearchKeyType>
int BST::BTree<Type, SearchKeyType>::HeightOfTree( Node * node ) {
	
	if ( ! node ) return 0 ;
	
	int maxHeight = -1, temp = 0 ;
	
	for ( int i = 0 ; i <= node->size ; i++ ) {
		
		temp = HeightOfTree( node->children[i] ) ;
		
		if ( maxHeight < 0 ) maxHeight = temp ;
		else {
			if ( temp > maxHeight ) maxHeight = temp ;
		} // else
		
	} // for
	
	return 1 + maxHeight ;
	
} // BST::BTree::HeightOfTree()

template<class Type, class SearchKeyType>
void BST::BTree<Type, SearchKeyType>::OutputFile( string fileName ) {
	
	if ( head == NULL ) {	
		cout << "No Node !" << endl ;
		return ;
	} // if
	
	fstream outFile( fileName.c_str(), ios::out ) ;
	
	outFile << "-----Output-----" << endl ;
	
	int num = InorderOutputFile( outFile, head, 1 ) ;
	
	// outFile << "num = " << num << endl ;
	
	outFile << "----------------" << endl ;
	
	cout << "Output File : " << fileName << endl ;
	
	gNum = 1 ;
	
	outFile.close() ;
	
} // BST::BTree::OutputFile()  

template<class Type, class SearchKeyType>
int BST::BTree<Type, SearchKeyType>::InorderOutputFile( fstream &outFile, Node * node, int level ) {
	
	if ( node == NULL ) {	
		return 0 ;
	} // if
	
	int num = 0 ;
	
	for ( int i = 0 ; i <= node->size ; i++ ) {
		num = num + InorderOutputFile( outFile, node->children[i], level + 1 ) ;
		if ( i < node->size ) {
			/*
			for ( int j = 0 ; j < level ; j++ ) {
				outFile << "  " ;
			} // for
			
			outFile << "<" << node->content[i].size() << "> " ;
			*/
			for ( int j = 0 ; j < node->content[i].size() ; j++ ) {
				outFile << "[" << gNum++ << "] " << node->content[i][j] << endl ;
				// outFile << node->content[i][j] << ( j + 1 != node->content[i].size() ? ", " : "  " ) ;
				num++ ;
			} // for
			
			// outFile << " [" << level << "] " << "(" << i << ") " ;
			
			/*
			outFile << node << " {" << node->parent << "} " ;
			for ( int i = 0 ; i <= node->size ; i++ ) outFile << node->children[i] << " " ;
			*/
			//outFile << "NS[" << node->size << "]" ;
			
			//outFile << endl ;
			
		} // if
	} // for
	
	return num ;
	
} // BST::BTree::InorderOutputFile()  


template<class Type, class SearchKeyType>
int BST::BTree<Type, SearchKeyType>::SearchTheWay( vector<Type> * content, int size, SearchKeyType key ) { // -1 repeat at [0], -2 repeat at [1], ...
	
	int wayIndex = 0 ;
	
	while ( wayIndex < size && ( stb ? content[wayIndex][0] <= key : content[wayIndex][0] >= key ) ) {
		if ( content[wayIndex][0] == key ) return ( wayIndex + 1 ) * (-1) ;
		else wayIndex++ ;
	} // while
	
	return wayIndex ;
	
} // BST::BTree::SearchTheWay()

template<class Type, class SearchKeyType>
vector<Type>* BST::BTree<Type, SearchKeyType>::SearchItem( Node * node, SearchKeyType key ) { // return an address of vector<Type> (every item with the key) 
	
	if ( node == NULL ) { // root 
		cout << "The data does not exist !" << endl ; 
		return NULL ; 		
	} // if
	else {
		
		int con = SearchTheWay( node->content, node->size, key ) ;
		
		if ( con < 0 ) { // Repeat 
			return &node->content[(con+1)*(-1)] ;
		} // if
		else {
			return SearchItem( node->children[con], key ) ;	
		} // else 		
		
	} // else
	
} // BST::BTree::SearchItem() 

#endif // GAMMA_H_INCLUDED











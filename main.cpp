////////////////////////////////////////////////////////////////////////////////
//	C++ program to find the shortest path between locations read from a file  //
//	Created by Michael R Spelling, February 2009.                             //
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <conio.h>
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////
//	Class creates an adjacency list from a file and contains functions to traverse list //
//////////////////////////////////////////////////////////////////////////////////////////
class adjacency_list
{
	struct location	//	Structure to hold information on each location in adjacency list
	{
		double distance;
		string city;
		location *next;
	};
	struct route	//	Structure to store route between cities
	{
		double distance;
		string source, destination;
	};
	vector<location> database;	//	Database is the adjacency list, holding all its information
	vector<route> path;	//	Path stores the routes between places
	vector<route> queue;	//	Queue holds places yet to be explored
	string place1, place2;	// Strings hold start and end places

	bool shortest_path();
	void print();
public:
	adjacency_list();
	~adjacency_list();
	void get_paths();
};

////////////////////////////////////////////
//	Constructor builds the adjacency list //
////////////////////////////////////////////
adjacency_list::adjacency_list()
{
	double length;	
	unsigned int count;
	string length_str, filename;
	location temp, *current;	

	cout << "Enter name of file containing location information:\n>> ";
	cin >> filename;
	ifstream file(filename.c_str());	
	if(!file){
		cout << "\nError: Can't open the file named " << filename << ".\n\n"; getch();
		exit(1);};

	while(!file.eof())
	{		
		file >> place1; file >> place2; file >> length_str;
		length = strtod(length_str.c_str(), NULL);		
		
		for(count = 0; count < database.size(); count++)	//	Go through database, checking for place1
		{			
			if(place1 == database[count].city)	//	If place1 in database, go to end of list and add place2
			{
				current = &database[count];
				while(current->next != NULL)
					current = current->next;
				current->next = new location;
				current = current->next;

				current->city = place2;
				current->distance = length;
				current->next = NULL;
				break;
			}			
		}		
		if(count == database.size())	//	If place1 not found in database, add to database and link place2 to it
		{
			temp.city = place1;
			temp.distance = 0;
			temp.next = new location;
			temp.next->city = place2;
			temp.next->distance = length;
			temp.next->next = NULL;
			database.push_back(temp);
		}		
		for(count = 0; count < database.size(); count++)	//	Go through database, checking for place2
		{			
			if(place2 == database[count].city)	//	If place2 in database, go to end of list and add place1
			{
				current = &database[count];
				while(current->next != NULL)
					current = current->next;
				current->next = new location;
				current = current->next;

				current->city = place1;
				current->distance = length;
				current->next = NULL;
				break;
			}
		}		
		if(count == database.size())	//	If place2 not found in database, add to database and link place1 to it
		{
			temp.city = place2;
			temp.distance = 0;
			temp.next = new location;
			temp.next->city = place1;
			temp.next->distance = length;
			temp.next->next = NULL;
			database.push_back(temp);
		}
	}
	file.close();
}

////////////////////////////////////////////////////
//	Destructor removes adjacency list from memory //
////////////////////////////////////////////////////
adjacency_list::~adjacency_list()
{
	int count;
	location *current, *previous;

	for(count = 0; count < database.size(); count++)	//	Remove linked list connected to database
	{
		current = database[count].next;
		while(current != NULL){
			previous = current;
			current = current->next;
			delete previous;}
	}
	database.clear();	//	Remove items in database
}

/////////////////////////////////////////////////////////////////////
//	Function gets destinations from file and prints routes to them //
/////////////////////////////////////////////////////////////////////
void adjacency_list::get_paths()
{
	string filename;
	route temp_path;	

	cout << "Enter name of file storing destinations to be found:\n>> ";
	cin >> filename;
	cout << "\n";

	ifstream file(filename.c_str());
	if(!file){
		cout << "Error: Can't open the file named " << filename << ".\n\n"; getch();
		exit(1);}

	while(!file.eof())
	{		
		file >> place1; file >> place2;	//	Add end destination to queue
		temp_path.distance = 0;
		temp_path.destination = place2;
		temp_path.source = place2;
		queue.push_back(temp_path);
		
		if(!shortest_path()) print();	//	Find the path between cities and print if found
		else cout << "\nPath could not be found\n";

		path.clear();	//	Clear vectors ready for next path to be found
		queue.clear();
	}
	file.close();
	return;
}

///////////////////////////////////////////////////////
//	Function finds shortest path between 2 locations //
///////////////////////////////////////////////////////
bool adjacency_list::shortest_path()
{
	unsigned int count;
	location *current;
	route temp_path, *small;
	
	while(queue.size() > 0)	//	Main loop to repeat while there are items in queue
	{		
		small = &queue[0];	//	Find smallest destination in queue
		for(count = 0; count < queue.size(); count++){
			if(queue[count].distance < small->distance)
				small = &queue[count];}		
		for(count = 0; queue[count].destination != small->destination; count++);

		temp_path.destination = queue[count].destination;	//	Remove smallest destination in queue and add to path
		temp_path.distance = queue[count].distance;
		temp_path.source = queue[count].source;
		path.push_back(temp_path);
		queue.erase(&queue[count]);
		
		if(path[path.size() - 1].destination == place1)	//	Return 0 if item just added was the end destination
			return 0;
		
		for(count = 0; count < database.size() && path[path.size() - 1].destination != database[count].city; count++);	//	Find location of item just added in the database

		if(count == database.size())	//	If item not in database return 1 for error
			return 1;
														
		current = &database[count];		
		while(current->next != NULL)	//	Go through all connected vertices
		{
			current = current->next;

			for(count = 0; count < path.size() && path[count].destination != current->city; count++);	//	Check if vertex already in path
			if(count == path.size())	//	If not in path check whether already in queue
			{
				for(count = 0; count < queue.size() && queue[count].destination != current->city; count++);
				if(count == queue.size())	//	If not then add to queue
				{
					temp_path.destination = current->city;
					temp_path.distance = current->distance + path[path.size() - 1].distance;
					temp_path.source = path[path.size() - 1].destination;
					queue.push_back(temp_path);
				}				
				else if(queue[count].distance > current->distance + path[path.size() - 1].distance)	//	Else add to queue only if it has a smaller distance to it than that already in queue
				{
					temp_path.destination = current->city;
					temp_path.distance = current->distance + path[path.size() - 1].distance;
					temp_path.source = path[path.size() - 1].destination;
					queue.push_back(temp_path);
					queue.erase(&queue[count]);
				}
			}
		}
	}
	return 1;	//	Return 1 if destination not found
}

//////////////////////////////////////////////
// Function prints the route between places //
//////////////////////////////////////////////
void adjacency_list::print()
{
	int count;
	string city;	

	count = path.size() - 1;		
	cout << "\nPath: " << path[count].destination;
	
	while(path[count].destination != place2)	//	Trace path back from end source to the destination,  printing each place
	{	
		city = path[count].source;
		for(count = 0; path[count].destination != city; count++);
		cout  << " -> " << path[count].destination;
	}
	cout << "\nDistance: " << path[path.size() - 1].distance << "\n";

	return;
}

///////////////////
// Main function //
///////////////////
int main()
{
	adjacency_list cities;	//	Create adjacency list object called cities		

	cities.get_paths();	//	Find and print all paths from a file in cities

	cout << "\n\n"; getch();

	return 0;
}
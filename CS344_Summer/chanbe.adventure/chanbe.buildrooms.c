/**

Written by Ben Chan for CS 344

**/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_CONNECTIONS 6
#define TOT_ROOMS 7

/**
	Struct for room type
		Detail:	Struct used to construct the room. Contains information 
				concerning room identity, and connections to other rooms.
		Vars: 	char* name, char* type, int numOutboundConnections, struct outboundConnections[]
**/
struct room { 
	char* name;
	char* type;
	int numOutboundConnections;
	struct room* outboundConnections[MAX_CONNECTIONS];
};

/**-------------------------------------------------------------------
						GETTERS/SETTERS
-------------------------------------------------------------------**/

/**
	Function getName()
		Params:	struct room
		Return:	char* name
**/
char* getName(struct room* room){
	return room->name;
}


/**
	Function setName()
		Params: struct room, char* name
**/
void setName(struct room* room, char* name){
	//Allocate new memory
	room->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(room->name, name);
	//Add null terminator
	room->name[strlen(name)] = '\0';
}

/**
	Function getType()
		Params: struct room
		Return: char* type (start, middle, or end room)
**/
char* getType(struct room* room){
	return room->type;
}

/**
	Function setType()
		Params: struct room, char* type
**/
void setType(struct room* room, char* type){
	//Allocate new memory
	room->type = (char*)malloc(sizeof(char) * (strlen(type) + 1));
	strcpy(room->type, type);
	//Add null terminator
	room->type[strlen(type)] = '\0';
}

/**
	Function getNumOut()
		Detail:	Returns the number of outbound connections to other rooms
		Params:	struct room
		Return:	int numOutboundConnections
**/
int getNumOut(struct room* room){
	return room->numOutboundConnections;
}

/**
	Function setNumOut()
		Detail:	Sets the number of outbound rooms coming out of this room.
		Params:	struct room, int num
**/
void setNumOut(struct room* room, int num){
	room->numOutboundConnections = num;
}

/**
	Function getOutbound()
		Detail:	Returns the 'n'th room on the outboundConnections[] array of the given room.
		Params: struct room, int n
		Return: struct room
**/
struct room* getOutbound(struct room* room, int n){
	return room->outboundConnections[n];
}

/**
	Function getRandomRoom()
		Detail:	Returns a random room from the graph. Does not validate connections.
		Params: graph containing room structs
		Return:	random room from graph
**/
struct room* GetRandomRoom(struct room** rooms){
	int rand_num = rand() % TOT_ROOMS;
	return rooms[rand_num];
}

/**-------------------------------------------------------------------
						END OF GETTERS/SETTERS
-------------------------------------------------------------------**/

/**
	Function isGraphFull()
		Detail:	Returns true if all rooms in graph have a minimum of 
				3 connections. False otherwise.
		Params: graph containing room structs
		Return: boolean value (1 or 0)
**/
int IsGraphFull(struct room** rooms){
	int i;
	for (i = 0; i < TOT_ROOMS; i++){
		if(getNumOut(rooms[i]) < 3){
			return 0;
		}
	}
	return 1;
}

/**
	Function getRandomRoom()
		Detail:	Returns true if a connection can be added from Room x 
				(< 6 outbound connections), false otherwise
		Params: struct room
		Return: boolean value (1 or 0)
**/
int CanAddConnectionFrom(struct room* x){
	if (getNumOut(x) < MAX_CONNECTIONS){
	  return 1;
	}
	return 0;
}

/**
	Function connectRoom()
		Detail: Connects two rooms together
		Params: struct room x, struct room y
**/
void ConnectRoom(struct room* x, struct room* y){
	//printf("Connecting Rooms %s and %s\n", getName(x), getName(y));
	int n_x, n_y;
	//Create the connection by linking pointers
	n_x = getNumOut(x);
	n_y = getNumOut(y);
	x->outboundConnections[n_x] = y;
	y->outboundConnections[n_y] = x;
	
	//Increase number of connections in each struct
	//printf(" - x numOut: %d -> ",x->numOutboundConnections);
	x->numOutboundConnections++;
	//printf("%d\n", x->numOutboundConnections);
	
	//printf(" - y numOut: %d -> ",y->numOutboundConnections);
	y->numOutboundConnections++;
	//printf("%d\n", y->numOutboundConnections);
}


/**
	Function AddRandomConnection()
		Detail: Adds a random, valid outbound connection from a Room to another Room
		Params: graph containing room structs
**/
void AddRandomConnection(struct room** rooms){
  struct room* x;	//Pointer to a connectable random room
  struct room* y;	//Pointer to a different, connectable random room

  while(1){
    x = GetRandomRoom(rooms);
    if (CanAddConnectionFrom(x) == 1)
		break;
	}
	do{
		y = GetRandomRoom(rooms);
	}
	while(CanAddConnectionFrom(y) == 0 || IsSameRoom(x, y) == 1 || ConnectionAlreadyExists(x, y) == 1);
	ConnectRoom(x, y);
}

/**
	Function ConnectionAlreadyExists()
		Detail: Returns true if a connection from Room x to Room y already exists, false otherwise
		Params: struct room x, struct room y
		Return: boolean value (1 or 0)
**/
int ConnectionAlreadyExists(struct room* x, struct room* y){	
	int connections = 0;
	// for loop to see if y exists in x's outbound rooms
	int i = 0;
	for (i = 0; i < getNumOut(x); i++){
		if (IsSameRoom(x->outboundConnections[i], y)){
			// X is connected to Y
			return 1;
		}
	}
	return 0;
}

/**
	Function IsSameRoom()
		Detail: Returns true if Rooms x and y are the same Room, false otherwise
		Params: struct room x, struct room y
		Return: boolean value (1 or 0)
**/
int IsSameRoom(struct room* x, struct room* y){
	if (x == y) return 1;
	return 0;
}

/**
	Function generateNames()
		Detail: Randomly assigns room names from a list of 10 pre-generated names
		Params: graph containing room structs
**/
void generateNames(struct room** rooms){
	int i, rand_num;
	// Create array of 10 potential room names
	char names[10][16] = {"Aldaraan", "Bespin", "Coruscant", "Dathomir", "Endor", "Felucia", "Geonosis", "Mandalore", "Mustafar", "Kamino"};
	int taken[10] = {0,0,0,0,0,0,0,0,0,0};
	//printf(" - loop to gen names\n");
	for(i = 0; i < TOT_ROOMS; i++){
		//printf(" - - Room %d\n", i);
		while (1) {
			//printf(" - - - Generating rand_num\n");
			rand_num = rand() % 10;
			//printf(" - - - rand_num: %d\n", rand_num);
			if (taken[rand_num] == 0){
				//printf(" - - - - taken[rand_num] == 0\n");
				setName(rooms[i], names[rand_num]);
				//printf(" - - - - Name successfully set to %s\n", getName(rooms[i]));
				taken[rand_num] = 1;
				//printf(" - - - - taken[rand_num] => 1. Loop break\n");
				break;
			}
			//printf(" - - - Name not set, looping... \n");
		}
	}
}

/**
	Function generateTypes()
		Detail: Randomly assigns start and end rooms. Assigns the rest as middle rooms.
		Params: graph containing room structs
**/
void generateTypes(struct room** rooms){
	int i, rand_num;
	
	// Set START_ROOM
	rand_num = rand() % 7;
	//printf(" - set room[%d] = START_ROOM\n", rand_num);
	setType(rooms[rand_num], "START_ROOM");
	
	// Set END_ROOM
	while(1){
		//printf(" - Loop to find unset room for END_ROOM\n");
		rand_num = rand() % 7;
		//printf(" - rand_num = %d\n", rand_num);
		if(strcmp(getType(rooms[rand_num]), "START_ROOM") != 0){
			//printf(" - - set room[%d] = END_ROOM\n", rand_num);
			setType(rooms[rand_num], "END_ROOM");
			//printf(" - - break loop\n");
			break;
		}
	}
	
	// Set MID_ROOM
	for (i = 0; i < TOT_ROOMS; i++){
		// Loop to find unset rooms for MID_ROOM
		if((strcmp(getType(rooms[i]), "START_ROOM") != 0) && (strcmp(getType(rooms[i]), "END_ROOM") != 0 )){
			setType(rooms[i], "MID_ROOM");
		}
	}
}


/**
	Function initializeRooms()
		Detail: Initializes values in the room struct. Allocates memory for arrays.
		Params: graph containing room structs
**/
void initializeRooms(struct room** rooms){
	int i;
	for (i = 0; i < TOT_ROOMS; i++){
		rooms[i] = (struct room*)malloc(sizeof(struct room));
		setName(rooms[i], "");
		setType(rooms[i], "");
		setNumOut(rooms[i], 0);
	}
}

/**
	Function IsSameRoom()
		Detail: Deinitializes rooms in graph
		Params: graph containing room structs
**/
void deinitializeRooms (struct room** rooms){
	int i;
	for (i = 0; i < TOT_ROOMS; i++){
		free (rooms[i]);
	}
}

/**
	Function printRoom()
		Detail: Prints room and it's variables for debugging/testing
		Params: struct room
**/
void printRoom(struct room* x){
	printf(" - PRINT ROOM\n");
	printf(" - - Name: %s\n", getName(x));
	printf(" - - Type: %s\n", getType(x));
	printf(" - - nOut: %d\n", getNumOut(x));
}

//Prints outbound rooms for testing purposes
/**
	Function printOutbound()
		Detail: Prints all outbounds rooms from a given room, for debugging/testing
		Params: struct room
**/
void printOutbound(struct room* x){
	int i;
	printf(" - PRINT OUTBOUNDS\n");
	for (i = 0; i < getNumOut(x); i++){
		printf(" - - CONNECTION %d: %s\n", (i+1), getName(getOutbound(x, i)));
	}
}

// Generates rooms/connections, and exports rooms to files
/**
	Function exportRooms()
		Detail:	Generates and exports a text file containing rooms and all relevant variables.
				This function serves as the parent function of this code, and is executed by the main() function.
**/
void exportRooms(){
	//Variables
	int i;
	int j;
	int pid = getpid();
	int buffer = 64 * sizeof(char);
	int file_descriptor;
	struct stat st = {0};
	char *dir_name = (char*)malloc(buffer);
	char *file_connection = (char*)malloc(buffer);
	char *file_footer = (char*)malloc(buffer);
	char *file_header = (char*)malloc(buffer);
	char *file_path = (char*)malloc(buffer);
	char *curr_file = (char*)malloc(buffer);
	ssize_t nread, nwritten;
	
	// Allocate space for 7 room structs
	struct room* rooms[TOT_ROOMS];
	initializeRooms(rooms);
	// Create 7 rooms w/ generated (pre-listed) names
	generateNames(rooms);
	generateTypes(rooms);
	// Create all connections in graph
	// Runs until graph is full (every room has 3 - 6 connections)
	while (IsGraphFull(rooms) == 0)
	{
	  AddRandomConnection(rooms);
	}
	// Generate file directory
	sprintf(dir_name, "%s%d", "chanbe.rooms.", pid);
	
	if (stat(dir_name, &st) == -1){
		mkdir(dir_name, 0700);
	}
	// Open file to write
	for(i = 0; i < TOT_ROOMS; i++){
		// Set curr_file to be the generated file's name
		sprintf(curr_file, "%s_ROOM", getName(rooms[i]));
		// Set file_path to be the location of the file (../dir/file)
		sprintf(file_path, "%s/%s", dir_name, curr_file);
		// Open the file
		file_descriptor = open(file_path, O_RDWR | O_CREAT | O_TRUNC, 0700);
		if(file_descriptor == -1){
			printf("Error, could not generate room files at \"%s\"\n", file_path);
			perror("In main ()");
			exit(1);
		}
		
		//Write header to the file
		sprintf(file_header, "ROOM NAME: %s\n", getName(rooms[i]));
		write(file_descriptor, file_header, (strlen(file_header)) * sizeof(char));
		
		//Writing connections
		for(j = 0; j < getNumOut(rooms[i]); j++){
			sprintf(file_connection, "CONNECTION %d: %s\n", (j + 1), getName(getOutbound(rooms[i], j)));
			write (file_descriptor, file_connection, (strlen(file_connection)) * sizeof(char));
		}
		
		//Write footer to the file
		sprintf(file_footer, "ROOM TYPE: %s\n", getType(rooms[i]));
		write(file_descriptor, file_footer, (strlen(file_footer)) * sizeof(char));
	}
	free (dir_name);
	free (file_connection);
	free (file_footer);
	free (file_header);
	free (file_path);
	free (curr_file);
	//deinitializeRooms(rooms);
}

/**
	Function main()
		Detail: The main function. Creates a seed for rand() generation, then executes exportRooms().
		Params:  void
		Return:  0 on exit
**/
int main (void) {
	//Create seed for random variables
	srand((unsigned) time(0));
	//Run program
	exportRooms();
	return 0;
}

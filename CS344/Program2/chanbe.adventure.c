#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define MAX_CONNECTIONS 6
#define TOT_ROOMS 7

struct room { 
	char* name;
	char* type;
	int numOutboundConnections;
	char* outboundConnections[MAX_CONNECTIONS];
};

struct game {
	int turnCount;
	char* start;
	char* end;
	char* path;
	struct room* currRoom;
};

// Gets the name of the room
char* getName(struct room* room){
	return room->name;
}

// Sets the name of the room
void setName(struct room* room, char* name){
	//Allocate new memory
	room->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(room->name, name);
	//Add null terminator
	room->name[strlen(name)] = '\0';
}

// Gets room type
char* getType(struct room* room){
	return room->type;
}

// Sets room type
void setType(struct room* room, char* type){
	//Allocate new memory
	room->type = (char*)malloc(sizeof(char) * (strlen(type) + 1));
	strcpy(room->type, type);
	//Add null terminator
	room->type[strlen(type)] = '\0';
}

// Gets the numOutboundConnections for a room
int getNumOut(struct room* room){
	return room->numOutboundConnections;
}

// Sets the numOutboundConnections for a room
void setNumOut(struct room* room, int num){
	room->numOutboundConnections = num;
}

// Gets a room (name only) coming out of the room
char* getOutbound(struct room* room, int n){
	return room->outboundConnections[n];
}

// Sets the start room in game struct
void setStart(struct game* game, char* start){
	//Allocate new memory
	game->start = (char*)malloc(sizeof(char) * (strlen(start) + 1));
	strcpy(game->start, start);
	//Add null terminator
	game->start[strlen(start)] = '\0';
}

// Set the end room in game struct
void setEnd(struct game* game, char* end){
	//Allocate new memory
	game->end = (char*)malloc(sizeof(char) * (strlen(end) + 1));
	strcpy(game->end, end);
	//Add null terminator
	game->end[strlen(end)] = '\0';
}

// Gets turn count
int getCount(struct game* game){
	return game->turnCount;
}

// Gets game path
char* getPath(struct game* game){
	return game->path;
}

// Adds room to path
void addPath(struct game* game, struct room* room){
	char* old_path;
	int n_strlen = strlen(getPath(game)) + strlen(getName(room));
	game->path = (char*)malloc(sizeof(char) * (n_strlen + 3));
	sprintf(game->path, "%s\n%s", old_path, room);
	game->path[n_strlen] = '\0';
}

// Initializes values in rooms array
void initializeRoom(struct room* room){
	room = (struct room*)malloc(sizeof(struct room));
	setName(room, "");
	setType(room, "");
	setNumOut(room, 0);
}

void initializeGame(struct game* game){
	game->turnCount = 0;
	setStart(game, "[NO VALUE]");
	setEnd(game, "[NO VALUE]");
	initializeRoom(game->currRoom);
}

int assignRoom(struct room* x, struct room* y){
	free (x);
	x = (struct room*)malloc(sizeof(struct room));
	setName(x, getName(y));
	setType(x, getType(y));
	setNumOut(x, getNumOut(y));
}

// Parses file of desired name into currRoom;
void parseRoom(struct room* c_room, struct room* n_room, char* name){
	int i;

	//assign room name;
	//Determine correct capitalization from file.
	
	//check room type in file
	if(1 == 1){	//if room type is END_ROOM
		//Game is complete
		return;
	}
	//Look for greatest integer value: Save to numOutboundConnections.
	
	//Use for loop to parse remaining outbound connections.
	//set current line to be 2, read until on last connection.
	for(i = 0; i < n_room->numOutboundConnections; i++){
		printf("test %d\n", i);
	}
}

int turn(struct game* game){
	int i;
	int running = 1;
	char* n_name = malloc(64 * sizeof(char));
	char* selection = malloc(64 * sizeof(char));
	
	//Print initial information
	printf("CURRENT LOCATION: %s\nPOSSIBLE CONNECTIONS: ", getName(game->currRoom));
	for (i = 0; i < getNumOut(game->currRoom); i++){
		printf("%s", getName(game->currRoom));
		//Print comma, unless in last spot
		if(i < (getNumOut(game->currRoom) - 1)){
			printf(", ");
		}
	}
	printf(".\nWHERE TO? >");
	//User input
	//Sets the current room
	
	struct room* n_room;
	initializeRoom(n_room);
	parseRoom(game->currRoom, n_room, selection);
	
	assignRoom(game->currRoom, n_room);
	
	printf("\n");
}

void main(){
	struct game* game;
	int running = 1;
	//Open most recent file with stat()
	
	//Game runs until parseRoom find END_ROOM
	while (running > 0){
		running = turn(game);
		game->turnCount++;
	}
	//Game over, prints turncount and path taken
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: %s\n", getCount(game), getPath(game));
	//Terminates
}

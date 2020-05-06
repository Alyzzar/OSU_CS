#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_CONNECTIONS 6
#define TOT_ROOMS 7

struct room { 
	char* name;
	char* type;
	int numOutboundConnections;
	char* outboundConnections[MAX_CONNECTIONS];
};

struct game {
	unsigned int turnCount;
	char* start;
	char* end;
	char* path;
	char* directory;
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

// Automatically finds and sets the file directory to the newest folder with the correct prefix
int setDir(struct game* game){
	int newestDirTime = -1;
	char targetDirPrefix[32] = "chanbe.rooms.";
	char newestDirName[256];
	memset(newestDirName, '\0', sizeof(newestDirName));

	DIR* dirToCheck;
	struct dirent *fileInDir;
	struct stat dirAttributes;

	//Open current directory
	dirToCheck = opendir(".");

	//If successful
	if (dirToCheck > 0){
		//Loop through subdirectories (Searching for chanbe.rooms.*)
		while ((fileInDir = readdir(dirToCheck)) != NULL){
			// If directory has prefix (chanbe.rooms.*)
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL){
				stat(fileInDir->d_name, &dirAttributes);
				if ((int)dirAttributes.st_mtime > newestDirTime){
					newestDirTime = (int)dirAttributes.st_mtime;
					memset(newestDirName, '\0', sizeof(newestDirName));
					strcpy(newestDirName, fileInDir->d_name);
				}
			}
		}
	}
	closedir(dirToCheck);
	// Assign directory if it exists
	if(newestDirName >= 0){
		//Allocate new memory for game->directory
		game->directory = (char*)malloc(sizeof(char) * (strlen(newestDirName) + 1));
		strcpy(game->directory, newestDirName);
		//Add null terminator
		game->directory[strlen(newestDirName)] = '\0';
		//Return 1 if directory was found.
		return 1;
	}
	//Return 0 if directory could not be found.
	return 0;
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

// Gets turn count
int getTurn (struct game* game){
	return game->turnCount;
}

// Returns the room of a specific type. For use with start/end rooms
struct room* findType (struct game* game, char* type){
	char dir[256];
	sprintf(dir, "%s/", game->directory)
	DIR* dir = opendir(dir);
}

// Initializes values in rooms array
void initializeRoom(struct room* room){
	room = (struct room*)malloc(sizeof(struct room));
	setName(room, "");
	setType(room, "");
	setNumOut(room, 0);
}

void initializeGame(struct game* game){
	game = (struct game*)malloc(sizeof(struct game));
	game->turnCount = 0;
	printf("INITIALIZE: TURNCOUNT = %s\n"game->turnCount);
	setStart(game, "");
	setEnd(game, "");
	game->path = (char*)malloc(sizeof(char) * 10);
	printf(" - Game initialized, creating currRoom.\n");
	initializeRoom(game->currRoom);
}

void freeGame(struct game* game){
	free(game->start);
	free(game->end);
	free(game->path);
	free(game->currRoom);
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

//Does printouts, and asks for users input.
int turn(struct game* game){
	int i;
	int running = 1;
	int numChars = -5;
	size_t buffer = 0;
	char* n_name = malloc(64 * sizeof(char));
	char* selection = malloc(64 * sizeof(char));
	char* lineEntered = NULL;
	
	//Print initial information
	printf(" - Running turn().\n");
	
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
	
	printf("\n\n");
	//Loop until valid input
	while (running > 0){
		numChars = getline(&lineEntered, &buffer, stdin);
		if (strcmp(lineEntered, "time")){
			// Return the time
			printf("Here's the time.\n");
			
			// Ask for another input
		} else {
			for (i=0; i < getNumOut(game->currRoom); i++){
				// Loop through connecting rooms to find matching name
				if (strcmp(lineEntered, getName(game->currRoom))){
					//Found the correct room
					break;
				}
			}
			// Didn't match any room names or 'time' function.
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}
	}
	//Sets the current room
	
	struct room* n_room;
	initializeRoom(n_room);
	parseRoom(game->currRoom, n_room, selection);
	
	assignRoom(game->currRoom, n_room);
	
	printf("\n");
}

void main(){
	//Initialize variables and objects.
	int running = 1;
	struct game* game;
	printf("Game object initialized.\n");

	printf();
	initializeGame(game);
	printf("Game values initialized.\n");
	
	if (setDir() == 0){
		printf("NO COMPATIBLE ROOMS FOUND. GAME TERMINATING.\n");
	} else {
		//Directory successfully found. Game continuing.
		
		printf("Directory found and set. Game sequence starting.\n");
		//Open most recent file with stat()
		
		printf("Opening files.\n");
		
		//Parse info to set start room, and save the name of the end room.
		
		
		
		//Game runs until parseRoom find END_ROOM
		while (running > 0){
			printf("Game loop %d.\n", getTurn(game));
			running = turn(game);
			game->turnCount++;
		}
		//Game over, prints turncount and path taken
		printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: %s\n", getCount(game), getPath(game));
	}
	//Terminates
	freeGame(game);
}

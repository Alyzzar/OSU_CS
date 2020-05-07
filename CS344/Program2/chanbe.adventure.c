#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

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
	free(room->name);
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
	free(room->type);
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

// Adds the input string to the array of outbound nodes
void addOutbound(struct room* room, char* connection){
	int index = getNumOut(room);
	//printf("addOutbound( %s, %s) at index %d\n", getName(room), connection, index);
	room->outboundConnections[index] = (char*)malloc(sizeof(char) * (strlen(connection) + 1));
	strcpy(room->outboundConnections[index], connection);
	room->outboundConnections[index][strlen(connection)] = '\0';
	room->numOutboundConnections++;
}

// Frees and reset outboundConnections
void resetOutbound(struct room* room){
	int i = 0;
	for (i = 0; i < getNumOut(room); i++){
		free(room->outboundConnections[i]);
	}
	setNumOut(room, 0);
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
			//printf(" - - FILE: %s\n", fileInDir->d_name);
			
			// If directory has prefix (chanbe.rooms.*)
			if (strstr(fileInDir->d_name, targetDirPrefix) != NULL){
				//printf(" - - - Matching prefix found.\n");
				
				stat(fileInDir->d_name, &dirAttributes);
				if ((int)dirAttributes.st_mtime > newestDirTime){
					//printf(" - - - - Marking as newest folder.\n");
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
		//printf(" - - Assigning directory to %s.\n", newestDirName);
		//Allocate new memory for game->directory string
		game->directory = (char*)malloc(sizeof(char) * (strlen(newestDirName) + 1));
		strcpy(game->directory, newestDirName);
		//Add null terminator
		game->directory[strlen(newestDirName)] = '\0';
		//free(dirToCheck);
		//Return 1 if directory was found.
		return 1;
	}
	//free(dirToCheck);
	//Return 0 if directory could not be found.
	return 0;
}

// Gets game path
char* getPath(struct game* game){
	return game->path;
}

// Adds room to path
void addPath(struct game* game, struct room* room){
	int n_strlen;
	char old_path [256];
	//Check if game->path is NULL
	if(getPath(game) == NULL){
			n_strlen = strlen(getName(room));
			game->path = (char*)malloc(sizeof(char) * ( n_strlen + 1));
			strcpy(game->path, getName(room));
	} else {
		//If not NULL, copy it's contents.
		strcpy(old_path, getPath(game));
		n_strlen = strlen(old_path) + strlen(getName(room));
		free(game->path);
		game->path = (char*)malloc(sizeof(char) * (n_strlen + 2));
		//Assign new value
		sprintf(game->path, "%s\n%s", old_path, getName(room));
	}
	game->path[n_strlen + 1] = '\0';
}

// Gets turn count
int getTurn (struct game* game){
	return game->turnCount;
}

// Initializes values in rooms array
void initializeRoom(struct room** room){
	*room = (struct room*)malloc(sizeof(struct room));
	(*room)->name = (char*)malloc(sizeof(char));
	(*room)->type = (char*)malloc(sizeof(char));
	setNumOut(*room, 0);
}

void freeRoom(struct room* room){
	free(room->name);
	free(room->type);
	resetOutbound(room);
}

void initializeGame(struct game** game){
	*game = (struct game*)malloc(sizeof(struct game));
	(*game)->turnCount = 0;
	//Initialize path with length of 1
	(*game)->path = NULL;
	initializeRoom(&(*game)->currRoom);
	//Initialize directory
	setDir(*game);
}

void freeGame(struct game* game){
	//free(game->turnCount);
	free(game->path);
	free(game->directory);
	freeRoom(game->currRoom);
	free(game->currRoom);
	free(game);
}

// Parses a file into currRoom.
void parseRoom(FILE* f, struct game* game){
	int i;
	int num_lines = 0;
	char name [64];
	char type [64];
	char* connection = (char*)malloc(64 * sizeof(char));
	size_t line_size = 32;
	char** lines = (char**)malloc(sizeof(char*) * 32);
	
	for(i = 0; i < 10; i++){
		lines[i] = NULL;
	}
	
	// Reset file pointer, and loop to store file as array
	fseek(f, 0, SEEK_SET);
	while(getline(&lines[num_lines], &line_size, f) > 0){
		num_lines++;
	}
	
	//printf(" - PARSING ROOM\n");
	// Assign room name;
	sscanf(lines[0], "%*s %*s %s", name);
	setName(game->currRoom, name);
	//printf(" - - Room name set: %s\n", name);
	
	// Assign room type;
	sscanf(lines[num_lines - 1], "%*s %*s %s", type);
	setType(game->currRoom, type);
	//printf(" - - Room type set: %s\n", type);
	
	// Reset outbound connections
	resetOutbound(game->currRoom);
	
	//for loop to parse outbound connections from line 2 onwards
	for(i = 0; i < (num_lines - 2); i++){
		sscanf(lines[i + 1], "%*s %*s %s", connection);
		//printf(" - - Parsed outbound: %s\n", connection);
		addOutbound(game->currRoom, connection);
	}
		
	// Add room name to path taken
	addPath(game, game->currRoom);
	
	// Free the lines[] array
	for(i = 0; i < num_lines; i++){
		free(lines[i]);
	}
	free(lines);
	free(connection);
}

// Finds and parses room of a specific type. 
// For setting the start/end room
// Return 1 on success, 0 on fail.
int findType (struct game* game, const char* type){
	// Set variables
	FILE *f;
	int i, j, numChars;
	//char directory[256];
	char file_name[256];
	char file_path[256];
	size_t buffer = 64 * sizeof(char);
	struct stat st;
	struct dirent* dir_info;
	char* line = (char *) malloc(buffer);
	
	// Iterate to the first dir/file
	DIR* dir = opendir(game->directory);
	dir_info = readdir(dir);
	strcpy(file_name, dir_info->d_name);
	
	//open the first file
	i = 0;
	while (i < TOT_ROOMS) {
		//Print the current room
		//printf(" - - FILE: %s\n", file_name);
		//Search through file line by line until NULL
		if(strstr(dir_info->d_name, "_ROOM") != NULL){
			//printf(" - - - File found, opening: ");
			sprintf(file_path, "%s/%s", game->directory, file_name);
			f = fopen(file_path, "r");
			//printf("Successful.\n");
			//printf(" - - - Getting lines...\n");
			j = 1;
			while (getline(&line, &buffer, f) != -1 && (j < MAX_CONNECTIONS + 3)) {
				line[strlen(line) - 1] = '\0';
				//printf(" - - - - LINE %d: %s\n", j, line);
				//Look for line with substring matching type
				if (strstr(line, type) != NULL) {
					//This file contains the correct room. Parse it.
					//printf(" - - %s ROOM FOUND. PARSING.\n", type);
					free(line);
					parseRoom(f, game);
					closedir(dir);
					return 1;
				}
				//Iterate to next line
				j++;
			}
			fclose(f);
			i++;
		}
		//Iterate to next file.
		//printf(" - - - Iterating files\n");
		dir_info = readdir(dir);
		sprintf(file_name, dir_info->d_name);
	}
	free(line);
	closedir(dir);
	return 0;
}

// Returns the room of a specific name. For use with traversing to outbound rooms
int findName (struct game* game, char* name){
	// Set variables
	FILE *f;
	int i, j, numChars;
	//char directory[256];
	char file_name[256];
	char file_path[256];
	struct stat st;
	struct dirent* dir_info;
	
	// Iterate to the first dir/file
	DIR* dir = opendir(game->directory);
	dir_info = readdir(dir);
	strcpy(file_name, dir_info->d_name);
	
	i = 0;
	while (i < TOT_ROOMS) {
		//printf(" - - FILE: %s\n", file_name);
		if(strstr(dir_info->d_name, "_ROOM") != NULL){
			// Copy file name into variable
			//printf(" - - - Comparing file name.\n");
			file_name[strlen(file_name) - 5] = '\0';
			//printf(" - - - - FILE: %s, TARGET: %s.\n", file_name, name);
			// Compare file name
			if(strcmp(file_name, name) == 0) {
				//printf(" - - - - TARGET FOUND. GENERATING FILE PATH:\n");
				// File has matching name.
				// Create file path
				sprintf(file_path, "%s/%s_ROOM", game->directory, file_name);
				//printf(" %s\n",file_path);
				f = fopen(file_path, "r");
				parseRoom(f, game);
				
				closedir(dir);
				return 1;
			}
		}
		//Iterate to next file.
		dir_info = readdir(dir);
		strcpy(file_name, dir_info->d_name);
		sprintf(file_name, dir_info->d_name);
	}
	closedir(dir);
	return 0;
}

//This function gets and prints the time to the console.
//Also exports the time to a text file.
//Format: 0:00pm, WEEKDAY, MONTH DAY, YEAR
void getTime(struct game* game){
	//Time variables
	time_t t;
	struct tm *tmp;
	char MY_TIME[50];
	//File writing variables
	struct stat st = {0};
	int file_descriptor;
	char file_path[256];
	
	//Getting the time
	time( &t ); 
    tmp = localtime( &t ); 
    // use strftime to display 
    strftime(MY_TIME, sizeof(MY_TIME), "%I:%M%p, %A, %B %d, %Y", tmp); 
    printf("%s\n", MY_TIME);
	
	//Saving the time to a file
	//Creating the file using known directory, and preset file name.
	sprintf(file_path, "%s/currentTime.txt", game->directory);
	if (stat(file_path, &st) == -1){
		mkdir(file_path, 0700);
	}
	//Open the file for write
	file_descriptor = open(file_path, O_RDWR | O_CREAT | O_TRUNC, 0700);
	write(file_descriptor, MY_TIME, (strlen(MY_TIME)) * sizeof(char));
} 

//Does printouts, and asks for users input.
int turn(struct game* game){
	int i;
	int running = 1;
	size_t buffer = 0;
	char* lineEntered = NULL;
	
	//Print initial information
	//printf(" - Running turn().\n");
	
	// Check if currRoom is the END
	if(strcmp(getType(game->currRoom), "END_ROOM") == 0){
		free(lineEntered);
		return 0;
	}
	
	printf("CURRENT LOCATION: %s\nPOSSIBLE CONNECTIONS: ", getName(game->currRoom));
	for (i = 0; i < getNumOut(game->currRoom); i++){
		printf("%s", getOutbound(game->currRoom, i));
		//Print comma, unless in last spot
		if(i < (getNumOut(game->currRoom) - 1)){
			printf(", ");
		}
	}
	printf(".\n");
	while (running > 0){
		printf("WHERE TO? >");
		getline(&lineEntered, &buffer, stdin);
		//Remove '\n' character using '\0'
		lineEntered[strlen(lineEntered)-1] = '\0';
		
		printf("\n");
		if (strcmp(lineEntered, "time") == 0){
			// Return the time
			getTime(game);
			printf("\n");
			// Ask for another input
		} else {
			for (i=0; i < getNumOut(game->currRoom); i++){
				// Loop through connecting rooms to find matching name
				if (strcmp(lineEntered, getOutbound(game->currRoom, i)) == 0){
					//Found the correct room
					//Assign the 'found' room as the currRoom
					if (findName(game, lineEntered) != 1){
						printf("NO MATCHING ROOM FOUND. GAME TERMINATING.\n\n");
						free(lineEntered);
						return 0;
					}
					//Break out of loop, since room has already been found.
					printf("\n");
					free(lineEntered);
					return 1;
				}
			}
			// Didn't match any room names or 'time' function.
			printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}
	}
}

//Main Function
int main(){
	
	//Initialize variables and objects.
	int running = 1;
	struct game* game;
	//printf("Initialized game object.\n");
	
	//printf("Initializing game values.\n");
	initializeGame(&game);
	
	if (game->directory == NULL || game->directory == 0){
		printf("NO COMPATIBLE ROOMS FOUND. GAME TERMINATING.\n");
		return 0;
	}
	//Directory successfully found. Game continuing.
	//Parse info to set start room	
	if (findType(game, "START_ROOM") == 0){
		printf("NO START ROOM FOUND. GAME TERMINATING.\n");
		return 0;
	}
	//Game runs until currRoom has type END_ROOM
	while (running > 0){
		//printf("Game loop %d.\n", getTurn(game));
		running = turn(game);
		if (running != 0) game->turnCount++;
	}
	//Game over, prints turncount and path taken
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n%s\n\n", getTurn(game), getPath(game));
	//Terminates
	freeGame(game);
	return 0;
}

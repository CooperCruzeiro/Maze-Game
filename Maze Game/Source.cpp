#include <iostream>
#include <raylib.h>
#include <algorithm>
#include <stack>
#include <random>

class Ball {
public:
	float x;
	float y;
	float speed;
	float radius = 10;
	Color newColor;

	void Draw() {

		DrawCircle(x, y, radius, newColor);
	}
};

void mazeWalls(bool maze[13][17], int rows, int columns, int cellSize) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			if (maze[i][j] == true) {
				//draw block/wall
				DrawRectangle(j * cellSize, i * cellSize, cellSize, cellSize, BLUE);
			}
			//open space
		}
	}
}

void generateMaze(bool(&maze)[13][17], int rows, int cols) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, 1);

	int randNum;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			maze[i][j] = true; // Initialize all cells as walls
		}
	}

	for (int i = 1; i < rows; i += 2) {	//create grid pattern as basis for maze
		for (int j = 1; j < cols; j += 2) {
			maze[i][j] = false;
		}

	}

	//creating gaps in the grid to create maze pattern
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			// Check if the cell is at an odd row and even column OR even row and odd column
			// If true, this means we are at a potential gap cell
			if ((i % 2 == 1 && j % 2 == 0) || (i % 2 == 0 && j % 2 == 1)) {
				randNum = dist(gen);
				if (randNum == 1) {
					maze[i][j] = false; // Mark the cell as a passage (gap).
				}
			}
		}
	}

	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {

			if (maze[i][j] == true) {//fine tune maze, make it more likely to be connected throughout with less chance of island cells
				if (maze[i - 1][j - 1] == true && maze[i][j - 1] == true && maze[i + 1][j - 1] == true && maze[i - 1][j + 1] == true && maze[i][j + 1] == true && maze[i + 1][j + 1] == true && maze[i - 1][j] == false && maze[i + 1][j] == false) {
					maze[i][j] = false;
				}

				if (maze[i - 1][j - 1] == true && maze[i][j - 1] == false && maze[i + 1][j - 1] == true && maze[i - 1][j + 1] == true && maze[i][j + 1] == false && maze[i + 1][j + 1] == true && maze[i - 1][j] == true && maze[i + 1][j] == true) {
					maze[i][j] = false;
				}

				if (maze[i - 1][j - 1] == true && maze[i][j - 1] == true && maze[i + 1][j - 1] == true && maze[i - 1][j + 1] == true && maze[i][j + 1] == true && maze[i + 1][j + 1] == false && maze[i - 1][j] == false && maze[i + 1][j] == false) {
					maze[i][j] = false;
				}
			}

		}
	}
	//MAYBE COMBINE THESE TWO FOR LOOPS 
	for (int i = 1; i < rows - 1; i++) {
		for (int j = 1; j < cols - 1; j++) {
			if (maze[i + 1][j] == true && maze[i][j + 1] == true && maze[i - 1][j] == true && maze[i][j - 1] == true) {
				maze[i][j - 1] = false;
			}

			if (maze[i + 1][j] == false && maze[i][j + 1] == false && maze[i - 1][j] == false && maze[i][j - 1] == false) {
				maze[i + 1][j] = true;
			}
		}
	}

	// Ensure that the start and finish cells are open spaces	
	maze[rows - 1][cols - 2] = false; // Start
	maze[1][8] = false; // Finish
}

void printPath(std::stack<std::pair<int, int>> path) {
	while (!path.empty()) {
		std::pair<int, int> cell = path.top();
		std::cout << "(" << cell.first << ", " << cell.second << ") ";
		path.pop();
	}
	std::cout << std::endl;
}

bool navigateMaze(bool maze[13][17], int totalSpaces, int finishCol, int finishRow, bool (&visit)[13][17], std::stack<std::pair<int, int>>& finishPath) {
	//bool visit[13][17];
	int visitCount = 0;
	int curCellX = 12; //rows - 1
	int curCellY = 15; //cols - 1
	int lastCellX = 12;
	int lastCellY = 15;
	bool foundFlag = false;

	//push the start onto the stack and mark it as visited, increase the visit counter 
	std::stack <std::pair<int, int>> visitStack;

	visitStack.push(std::make_pair(12, 15)); //mark the start as visited
	visit[curCellX][curCellY] = true;
	visitCount++;

	std::pair<int, int> prevCell = visitStack.top();
	
	//search for an adjacent open space from left, up, right, down - exclude previous space. 
	while (!visitStack.empty() && visitCount < totalSpaces) {
		//std::cout << std::endl << "Size of the Stack: " << visitStack.size() << std::endl;

		if (maze[curCellX][curCellY - 1] == false && visit[curCellX][curCellY - 1] == false) { //search left cell to check if it's a space and not been visited
			curCellY = curCellY - 1;
			//push the next space onto the stack and mark as visited, increase the visit counter
			visitStack.push(std::make_pair(curCellX, curCellY));
			visit[curCellX][curCellY] = true;
			visitCount++;
			//std::cout << "left" << std::endl;

		}

		else if (maze[curCellX - 1][curCellY] == false && visit[curCellX - 1][curCellY] == false) { //search up cell
			curCellX = curCellX - 1;

			visitStack.push(std::make_pair(curCellX, curCellY));
			visit[curCellX][curCellY] = true;
			visitCount++;
			//std::cout << "up" << std::endl;

		}

		else if (maze[curCellX][curCellY + 1] == false && visit[curCellX][curCellY + 1] == false) { //search right cell
			curCellY = curCellY + 1;

			visitStack.push(std::make_pair(curCellX, curCellY));
			visit[curCellX][curCellY] = true;
			visitCount++;
			//std::cout << "right" << std::endl;

		}

		else if (maze[curCellX + 1][curCellY] == false && visit[curCellX + 1][curCellY] == false) { //search down cell
			curCellX = curCellX + 1;
			visitStack.push(std::make_pair(curCellX, curCellY));
			visit[curCellX][curCellY] = true;
			visitCount++;
			//std::cout << "down" << std::endl;

		}

		//Once a deadend is reached, start popping visited cells from the stack and retrace steps until there are adjacent open unvisited cells.
		else {
			visitStack.pop();

			if (!visitStack.empty()) {
				prevCell = visitStack.top();
				curCellX = prevCell.first;
				curCellY = prevCell.second;
			}
		}

		//std::cout << "Current Cell: " << curCellX << ", " << curCellY << std::endl << std::endl;

		if (visit[finishRow][finishCol] == true && foundFlag == false) {	// If the finish is reached, copy the navigateMaze stack onto finishPath stack.

			finishPath = visitStack;
			foundFlag = true;
		}

	}

	if (visit[finishRow][finishCol] == true && visitCount == totalSpaces) {	// If the Finish is reached, copy the navigateMaze stack onto finishPath stack.
		//printPath(finishPath);

		return true;
	}

	else {
		
		std::cout << "Visit Count: " << visitCount << std::endl;
		return false;
	}

	//continue while the visit counter is less than the total number of open spaces in the maze and the stack is not empty.
	//If the stack is empty, the maze isn't fully connected. A new maze must be generated and the process must start over.

}

//TODO: Fix the border, create a proper game loop, expand the maze, maybe touch up the maze to make it look better.

int main() {
	InitWindow(800, 600, "Maze");
	SetTargetFPS(60);
	const int rows = 13;
	const int cols = 17;

	int cellSize = std::min(GetScreenWidth() / cols, GetScreenHeight() / rows);

	float playerStartX = (15.53) * cellSize;//Make player start position proportionate to Maze cells 15 and 12 
	float playerStartY = (12.53) * cellSize;
	float enemyStartX = (15.53) * cellSize;
	float enemyStartY = (12.53) * cellSize;

	Ball player;
	player.newColor = DARKGREEN;
	player.x = playerStartX; 
	player.y = playerStartY; 
	player.speed = 2;

	Ball enemy;
	enemy.newColor = RED;
	enemy.x = enemyStartX;
	enemy.y = enemyStartY;
	enemy.speed = 2;

	float wallWidth = 10;	

	bool maze[rows][cols];
	int totalSpaces = 0;
	bool mazeConnected = false;
	std::stack<std::pair<int, int>> finishPath;

	float nextCell = -1;

	//mark which direction the player moves;
	bool moveFlagLeft = false;
	bool moveFlagRight = false;
	bool moveFlagUp = false;
	bool moveFlagDown = false;

	//Keep track of the previous position to know when the player isn't moving anymore.
	float prevPosX;
	float prevPosY;
	bool isMoving = false;

	int counter = 0;

	while (mazeConnected == false) {
		totalSpaces = 0;

		generateMaze(maze, rows, cols);

		//initialize all cells as unvisited
		bool visit[rows][cols];
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				visit[i][j] = false;
			}

			std::cout << std::endl;
		}

		// Calculate the total number of open spaces in the maze
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (maze[i][j] == false) {
					totalSpaces++;
				}
			}
		}

		mazeConnected = navigateMaze(maze, totalSpaces, 8, 1, visit, finishPath);

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {

				if (maze[i][j] == true) {
					std::cout << "X ";
				}

				else {
					if (visit[i][j] == true) {
						std::cout << "O ";
					}

					else {
						std::cout << "  ";
					}
				}
			}

			std::cout << std::endl;
		}

		// Print the generated maze to the console for visual inspection

		std::cout << "Total Spaces: " << totalSpaces << std::endl;
		std::cout << "Fully Connected Maze: " << mazeConnected << std::endl;

		counter++;
	}

	const char* winnerText = "You Win";

	while (!WindowShouldClose()) {
		
		if (isMoving == false) {
			if (IsKeyPressed(KEY_RIGHT)) {
				if (player.x < (cols - 1) * cellSize && maze[int(player.y / cellSize)][int((player.x + cellSize) / cellSize)] == false) {

					nextCell = player.x + cellSize;

					moveFlagRight = true;
					moveFlagLeft = false;
					moveFlagDown = false;
					moveFlagUp = false;
				}
			}

			if (IsKeyPressed(KEY_LEFT)) {
				if (player.x > 0 && maze[int(player.y / cellSize)][int((player.x - cellSize) / cellSize)] == false) {

					nextCell = player.x - cellSize;

					moveFlagRight = false;
					moveFlagLeft = true;
					moveFlagDown = false;
					moveFlagUp = false;
				}
			}

			if (IsKeyPressed(KEY_DOWN)) {
				if (player.y < (rows - 1) * cellSize && maze[int((player.y + cellSize) / cellSize)][int(player.x / cellSize)] == false) {

					nextCell = player.y + cellSize;

					moveFlagRight = false;
					moveFlagLeft = false;
					moveFlagDown = true;
					moveFlagUp = false;
				}
			}

			if (IsKeyPressed(KEY_UP)) {
				if (player.y > 0 && maze[int((player.y - cellSize) / cellSize)][int(player.x / cellSize)] == false) {

					nextCell = player.y - cellSize;

					moveFlagRight = false;
					moveFlagLeft = false;
					moveFlagDown = false;
					moveFlagUp = true;
				}
			}

		}

		if (IsKeyPressed(KEY_SPACE)) {

			mazeConnected = false;
			int counter = 0;

			while (mazeConnected == false) {
				totalSpaces = 0;

				generateMaze(maze, rows, cols);

				//initialize all cells as unvisited
				bool visit[rows][cols];
				for (int i = 0; i < rows; i++) {			//DOES THIS NEED TO BE HERE?, MAYBE PUT IN navigateMaze function
					for (int j = 0; j < cols; j++) {
						visit[i][j] = false;
					}

					std::cout << std::endl;
				}

				// Calculate the total number of open spaces in the maze
				for (int i = 0; i < rows; i++) {
					for (int j = 0; j < cols; j++) {		//could pass this in generate maze as reference.
						if (maze[i][j] == false) {
							totalSpaces++;
						}
					}
				}

				mazeConnected = navigateMaze(maze, totalSpaces, 8, 1, visit, finishPath); //DO I NEED TO PASS VISIT TO THE FUNCTION?

				for (int i = 0; i < rows; i++) {
					for (int j = 0; j < cols; j++) {

						if (maze[i][j] == true) {
							std::cout << "X ";
						}

						else {
							if (visit[i][j] == true) {
								std::cout << "O ";
							}

							else {
								std::cout << "  ";
							}
						}
					}

					std::cout << std::endl;
				}

				// Print the generated maze to the console for visual inspection

				std::cout << "Total Spaces: " << totalSpaces << std::endl;
				std::cout << "Fully Connected Maze: " << mazeConnected << std::endl;

				player.x = playerStartX;
				player.y = playerStartY;

				moveFlagRight = false;
				moveFlagLeft = false;
				moveFlagDown = false;
				moveFlagUp = false;
				
				counter++;
			}
		}

		//TODO:Make enemy character that uses that path to reach the end.
			//Should fine tune the algorithm so that the shortest path is used as finishPath, maybe use DFS or something like that.
		//Maybe make the bottom part of updating the player a function for both enemy and player to use.
		
		//make finish line proportionate to maze cells 8 and 1
		float finishX = 8.31 * cellSize;
		float finishY = 1.31 * cellSize;
		
		prevPosX = player.x;
		prevPosY = player.y;

		if (moveFlagRight == true) {

			if (player.x != nextCell) {
				player.x += player.speed;
			}

			else {
				moveFlagRight == false;
				player.x = nextCell;
			}

		}

		else if (moveFlagLeft == true) {

			if (player.x != nextCell) {
				player.x -= player.speed;
			}

			else {
				player.x = nextCell;
				moveFlagLeft == false;
			}

		}

		else if (moveFlagDown == true) {

			if (player.y != nextCell) {
				player.y += player.speed;
			}

			else {
				moveFlagDown == false;
				player.y = nextCell;
			}

		}

		else if (moveFlagUp == true) {

			if (player.y != nextCell) {
				player.y -= player.speed;
			}

			else {
				moveFlagUp == false;
				player.y = nextCell;
			}

		}

		//Check to see if the player is still moving. If not, another direction can be input.
		if (prevPosX == player.x && prevPosY == player.y) {
			isMoving = false;
		}
		else {
			isMoving = true;
		}
	

		BeginDrawing();
		ClearBackground(BLACK);

		mazeWalls(maze, rows, cols, cellSize);

		DrawRectangle(finishX, finishY, 20, 20, GREEN); //Finish Line    

		player.Draw();
		enemy.Draw();

		if (CheckCollisionCircleRec(Vector2{ player.x, player.y }, player.radius, Rectangle{ finishX, finishY, 20, 20 })) {
			DrawText(winnerText, GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 - 30, 60, GREEN);
		}

		EndDrawing();
	}
	
	CloseWindow();
	return 0;
}
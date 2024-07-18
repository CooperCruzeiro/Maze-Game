#include <iostream>
#include <raylib.h>
#include <algorithm>
#include <stack>
#include <random>
#include <string>


const int rows = 49;	//need to be odd numbers
const int cols = 65;

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

void mazeWalls(bool maze[rows][cols], int rows, int columns, int cellSize) {
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

void generateMaze(bool(&maze)[rows][cols], int rows, int cols) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, 1);

	int randNum;
	
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			maze[i][j] = true; // Initialize all cells as walls
		}
	}

	const std::vector<std::pair<int, int>> directions = {
	   {-2, 0}, {2, 0}, {0, -2}, {0, 2}			//Passages are carved out two cells at time so that the thickness of the walls is always one cell
	};

	std::stack<std::pair<int, int>> stack;  //stack for processing cells
	int startRow = rows - 2;
	int startCol = 1;
	maze[startRow][startCol] = false;
	stack.push({ startRow, startCol });

	while (!stack.empty()) {
		auto current = stack.top();
			
		int currentRow = current.first;			//Get row and column of current cell
		int currentCol = current.second;

		std::vector<std::pair<int, int>> neighbors; //holds a cell's neighbors which are two cells away

		for (const auto& direction : directions) {			//Checks each direction for valid neighbors

			int newRow = currentRow + direction.first;		//Calculate row and column of neighbor cell
			int newCol = currentCol + direction.second;

			if (newRow > 0 && newRow < rows - 1 && newCol > 0 && newCol < cols - 1 && maze[newRow][newCol]) {	//checks if neighbor is in bounds and is a wall, if so, added to the stack
				neighbors.push_back({ newRow, newCol });
			}
		}

		if (!neighbors.empty()) {
			std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);	
			int randomIndex = dist(gen);
			auto next = neighbors[randomIndex];		//Randomly pick a neighbor

			int nextRow = next.first;				//Get row and column of neighbor
			int nextCol = next.second;

			maze[nextRow][nextCol] = false;			//mark neighbor cell as an open space
			maze[currentRow + (nextRow - currentRow) / 2][currentCol + (nextCol - currentCol) / 2] = false; //Mark cell between current and neighbor as an open space also
			stack.push({ nextRow, nextCol });
		}
		else {				//If a cell has no neighbors it is popped from the stack
			stack.pop();
		}
	}	

	// Ensure start and end are open
	maze[1][1] = false; // Start
	maze[rows - 2][cols - 2] = false; // End

	// Ensure that the start and finish cells are open spaces	
	maze[rows - 1][cols - 2] = false; // Start
	maze[1][cols/2] = false; // Finish
}

void printPath(std::stack<std::pair<int, int>> path) {
	while (!path.empty()) {
		std::pair<int, int> cell = path.top();
		std::cout << "(" << cell.first << ", " << cell.second << ") ";
		path.pop();
	}
	std::cout << std::endl;
}

void reverseStack(std::stack<std::pair<int, int>>& stack) {
	std::stack<std::pair<int, int>> auxStack;

	// Transfer all elements from the original stack to the auxiliary stack
	while (!stack.empty()) {
		auxStack.push(stack.top());
		stack.pop();
	}

	stack = auxStack;
	// Transfer the elements back to the original stack
	//while (!auxStack.empty()) {
	//	stack.push(auxStack.top());
	//	auxStack.pop();
	//}
}

bool approxEqual(float a, float b, float tolerance) {
	return std::abs(a - b) <= tolerance;
}

int EnemyMove(std::stack<std::pair<int, int>>& finishPath) {
	//int curCell;
	//int nextCell;
	float sum1 = NULL; // y-direction
	float sum2 = NULL; // x-direction
	std::pair<int, int> nextCell;

	std::pair<int, int> element = finishPath.top();
	if (!finishPath.empty()) {
		finishPath.pop();

		if (!finishPath.empty()) {
			nextCell = finishPath.top();
		}
	}

	sum1 = nextCell.first - element.first;
	sum2 = nextCell.second - element.second;

	if (sum1 < 0 && sum2 == 0) {			//up
		std::cout << "up" << std::endl;
		return 1;
	}

	else if (sum1 > 0 && sum2 == 0) {		//down
		std::cout << "down" << std::endl;
		return 2;
	}

	else if (sum1 == 0 && sum2 < 0) {		//left
		std::cout << "left" << std::endl;
		return 3;
	}

	else if (sum1 == 0 && sum2 > 0) {		//right
		std::cout << "right" << std::endl;
		return 4;
	}

	else { return -999; }

}

bool navigateMaze(bool maze[rows][cols], int totalSpaces, int finishCol, int finishRow, bool(&visit)[rows][cols], std::stack<std::pair<int, int>>& finishPath) {
	//bool visit[rows][cols];
	int visitCount = 0;
	int curCellX = rows - 2; //
	int curCellY = 1; //start of path
	bool foundFlag = false;

	//push the start onto the stack and mark it as visited, increase the visit counter 
	std::stack <std::pair<int, int>> visitStack;

	visitStack.push(std::make_pair(curCellX, curCellY)); //mark the start as visited
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
		
		if (visit[finishRow][finishCol] == true && foundFlag == false) {	// If the finish is reached, copy the navigateMaze stack onto finishPath stack. LATER: ADJUST IT FOR ONLY EXECUTING FOR A COMPLETE MAZE.

			finishPath = visitStack;
			//reverseStack(finishPath);
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

//TODO: Fix the border, create a proper game loop, maybe touch up the maze to make it look better, reposition the finish, player and enemy so that they placed in the maze properly.

int main() {
	InitWindow(1600, 1200, "Maze");
	SetTargetFPS(60);
	//const int rows = 13;
	//const int cols = 17;

	int cellSize = std::min(GetScreenWidth() / cols, GetScreenHeight() / rows);

	float playerStartX = (cols - 1.47) * cellSize;//Make player start position proportionate to Maze cells cols - 1 and rows - 1 
	float playerStartY = (rows - 0.47) * cellSize;
	float enemyStartX = (1.53) * cellSize;
	float enemyStartY = (rows - 1.47) * cellSize;

	Ball player;
	player.newColor = DARKGREEN;
	player.x = playerStartX;
	player.y = playerStartY;
	player.speed = 2;

	Ball enemy;
	enemy.newColor = RED;
	enemy.x = enemyStartX;
	enemy.y = enemyStartY;
	enemy.speed = 1.5;

	float wallWidth = 10;

	bool maze[rows][cols];
	int totalSpaces = 0;
	bool mazeConnected = false;
	std::stack<std::pair<int, int>> finishPath;
	std::stack<std::pair<int, int>> emptyStack; //To empty the finsishPath stack when maze is restarted.

	float nextCell = -1;
	float EnNextCell = -1;
	std::stack<std::pair<int, int>> CurrCell;
	std::stack<std::pair<int, int>> NextCell;

	//mark which direction the player moves.
	bool moveFlagLeft = false;
	bool moveFlagRight = false;
	bool moveFlagUp = false;
	bool moveFlagDown = false;

	//Mark which direction the enemy moves.
	bool EnMoveFlagLeft = false;
	bool EnMoveFlagRight = false;
	bool EnMoveFlagUp = false;
	bool EnMoveFlagDown = false;

	//Keep track of the previous position to know when the player isn't moving anymore.
	float prevPosX;
	float prevPosY;
	bool isMoving = false;

	int counter = 0;
	int EnMovDir = 0;
	bool finishFoundPlayer = false;
	bool finishFoundEnemy = false;

	int points = 10000;

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

		mazeConnected = navigateMaze(maze, totalSpaces, cols / 2, 1, visit, finishPath);

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {

				if (maze[i][j] == true) {
					//std::cout << "X ";
				}

				else {
					if (visit[i][j] == true) {
						//std::cout << "O ";
					}

					else {
						//std::cout << "  ";
					}
				}
			}

			//std::cout << std::endl;
		}

		// Print the generated maze to the console for visual inspection

		std::cout << "Total Spaces: " << totalSpaces << std::endl;
		std::cout << "Fully Connected Maze: " << mazeConnected << std::endl;

		counter++;
	}

	const char* winnerText = "You Win";
	const char* loserText = "You Lose";

	bool EnMovFlag = false;
	reverseStack(finishPath);

	while (!WindowShouldClose()) {
		// Convert points to string
		std::string pointsStr = "Points: " + std::to_string(points);

		// Convert string to C-string
		const char* pointsCStr = pointsStr.c_str();

		//Check input from user to see what direction they chose to move
		if (isMoving == false) {
			if (IsKeyDown(KEY_RIGHT)) {
				if (player.x < (cols - 1) * cellSize && maze[int(player.y / cellSize)][int((player.x + cellSize) / cellSize)] == false) {

					nextCell = player.x + cellSize; //next cell/position player moves to

					moveFlagRight = true;
					moveFlagLeft = false;
					moveFlagDown = false;
					moveFlagUp = false;
				}
			}

			if (IsKeyDown(KEY_LEFT)) {
				if (player.x > 0 && maze[int(player.y / cellSize)][int((player.x - cellSize) / cellSize)] == false) {

					nextCell = player.x - cellSize;

					moveFlagRight = false;
					moveFlagLeft = true;
					moveFlagDown = false;
					moveFlagUp = false;
				}
			}

			if (IsKeyDown(KEY_DOWN)) {
				if (player.y < (rows - 1) * cellSize && maze[int((player.y + cellSize) / cellSize)][int(player.x / cellSize)] == false) {

					nextCell = player.y + cellSize;

					moveFlagRight = false;
					moveFlagLeft = false;
					moveFlagDown = true;
					moveFlagUp = false;
				}
			}

			if (IsKeyDown(KEY_UP)) {
				if (player.y > 0 && maze[int((player.y - cellSize) / cellSize)][int(player.x / cellSize)] == false) {

					nextCell = player.y - cellSize;

					moveFlagRight = false;
					moveFlagLeft = false;
					moveFlagDown = false;
					moveFlagUp = true;
				}
			}

		}

		//in program restart
		if (IsKeyPressed(KEY_SPACE)) {

			mazeConnected = false;
			int counter = 0;
			EnMovDir = 0;
			finishFoundPlayer = false;
			finishFoundEnemy = false;
			finishPath.swap(emptyStack);
			points = 10000;

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

				mazeConnected = navigateMaze(maze, totalSpaces, cols / 2, 1, visit, finishPath); //DO I NEED TO PASS VISIT TO THE FUNCTION?

				for (int i = 0; i < rows; i++) {
					for (int j = 0; j < cols; j++) {

						if (maze[i][j] == true) {
							//std::cout << "X ";
						}

						else {
							if (visit[i][j] == true) {
								//std::cout << "O ";
							}

							else {
								//std::cout << "  ";
							}
						}
					}

					//std::cout << std::endl;
				}

				// Print the generated maze to the console for visual inspection

				std::cout << "Total Spaces: " << totalSpaces << std::endl;
				std::cout << "Fully Connected Maze: " << mazeConnected << std::endl;


				//Reset player and enemy postions and other variables to restart.
				player.x = playerStartX;
				player.y = playerStartY;

				moveFlagRight = false;
				moveFlagLeft = false;
				moveFlagDown = false;
				moveFlagUp = false;

				counter++;

				EnMovFlag = false;
				enemy.x = enemyStartX;
				enemy.y = enemyStartY;
				reverseStack(finishPath);

				EnMovDir = -1;
			}
		}

		//TODO:Make enemy character that uses that path to reach the end.
			//Should fine tune the algorithm so that the shortest path is used as finishPath, maybe use DFS or something like that.
		//Maybe make the bottom part of updating the player a function for both enemy and player to use.

		//make finish line proportionate to maze cells 8 and 1
		float finishX = (cols/2) * cellSize;
		float finishY = cellSize;

		prevPosX = player.x;
		prevPosY = player.y;


		//When the player is moving, they keep moving until the next cell is reached
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

		////////////////////


		//create series of if else loops to decide which way the enemy should go and mark the corresponding flag.
			//create function using finishPath to determine the next cell.
				//Subtract the values of the current cell from the previous cell, if one of the values is different, that is the direction.


		//Enemy uses finishPath to reach end
		if (!finishPath.empty() && EnMovFlag == false) {
			EnMovDir = EnemyMove(finishPath);
		}

		if (EnMovDir == 1) {
			if (EnMovFlag == false) {
				EnNextCell = enemy.y - cellSize;		//distance between the enemy and the next cell

			}
			EnMovFlag = true;

			if (enemy.y != EnNextCell) {
				enemy.y -= enemy.speed;
			}

			else {
				EnMovDir = 0;
				EnMovFlag = false;
				enemy.y = EnNextCell;
			}

		}

		else if (EnMovDir == 2) {

			if (EnMovFlag == false) {
				EnNextCell = enemy.y + cellSize;

			}
			EnMovFlag = true;

			if (enemy.y != EnNextCell) {
				enemy.y += enemy.speed;
			}

			else {
				EnMovDir = 0;
				EnMovFlag = false;
				enemy.y = EnNextCell;
			}

		}

		else if (EnMovDir == 3) {

			if (EnMovFlag == false) {
				EnNextCell = enemy.x - cellSize;

			}
			EnMovFlag = true;

			if (enemy.x != EnNextCell) {
				enemy.x -= enemy.speed;
			}

			else {
				EnMovDir = 0;
				EnMovFlag = false;
				enemy.x = EnNextCell;
			}

		}

		else if (EnMovDir == 4) {

			if (EnMovFlag == false) {
				EnNextCell = enemy.x + cellSize;

			}
			EnMovFlag = true;

			if (enemy.x != EnNextCell) {
				enemy.x += enemy.speed;
			}

			else {
				EnMovDir = 0;
				EnMovFlag = false;
				enemy.x = EnNextCell;
			}

		}


		BeginDrawing();
		ClearBackground(BLACK);

		mazeWalls(maze, rows, cols, cellSize);

		DrawRectangle(finishX, finishY, cellSize, cellSize, GREEN); //Finish Line    

		player.Draw();
		enemy.Draw();
		DrawText(pointsCStr, 10, 10, 40, DARKGRAY);

		//Win screen if player reaches end
		if (CheckCollisionCircleRec(Vector2{ player.x, player.y }, player.radius, Rectangle{ finishX, finishY, 20, 20 }) && finishFoundEnemy == false) {
			DrawText(winnerText, GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 - 30, 60, GREEN);
			finishFoundPlayer = true;
		}

		//Lose screen if enemy reaches end first
		else if (CheckCollisionCircleRec(Vector2{ enemy.x, enemy.y }, enemy.radius, Rectangle{ finishX, finishY, 20, 20 }) && finishFoundPlayer == false) {
			DrawText(loserText, GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 - 30, 60, RED);
			finishFoundEnemy = true;
			points = 0;
		}

		if (finishFoundPlayer == false && finishFoundEnemy == false) {
			points--;
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
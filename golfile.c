#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_BOARDS 10
#define MAX_SIZE 10
#define ALIVE 1
#define DEAD 0
#define MAX_HISTORY 1000  // Memory generation limit

int count_neighbors(int size, int grid[MAX_SIZE][MAX_SIZE], int x, int y) {
    int count = 0;
    
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int ni = (x + i + size) % size;
            int nj = (y + j + size) % size;
            count += grid[ni][nj];
        }
    }
    return count;
}

void next_generation(int size, int current[MAX_SIZE][MAX_SIZE], int next[MAX_SIZE][MAX_SIZE]) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int neighbors = count_neighbors(size, current, i, j);
            if (current[i][j] == ALIVE) {
                next[i][j] = (neighbors == 2 || neighbors == 3) ? ALIVE : DEAD;
            } else {
                next[i][j] = (neighbors == 3) ? ALIVE : DEAD;
            }
        }
    }
}

int grids_are_equal(int size, int grid1[MAX_SIZE][MAX_SIZE], int grid2[MAX_SIZE][MAX_SIZE]) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid1[i][j] != grid2[i][j]) {
                return 0;  // Grids are not equal
            }
        }
    }
    return 1;  // Grids are identical
}

int detect_loop(int size, int grid[MAX_SIZE][MAX_SIZE], int history[][MAX_SIZE][MAX_SIZE], int history_count) {
    for (int h = 0; h < history_count; h++) {
        if (grids_are_equal(size, grid, history[h])) {
            return 1;  // Loop detected
        }
    }
    return 0;  // No loop detected
}




int count_live_cells(int size, int grid[MAX_SIZE][MAX_SIZE]){
    int live_cells = 0;
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            if (grid[i][j] == ALIVE) {
                live_cells++;
            }
        }
    }
    return live_cells;
}



void write_array_to_file(const char *filename, int rep[MAX_BOARDS],int size){
    printf("Writing output to file : %s\n",filename);
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error during opening file");
        return;
    }

    for (int i = 0; i < size; i++){
        fprintf(file,"%d\n", rep[i]);
        printf("Wrote live_cells[%d] = %d\n", i, rep[i]);

    }
    fclose(file);
    printf("Finished writing output to file.\n");

}


void print2Dtab(int size, int grid[MAX_SIZE][MAX_SIZE]){

    printf("grid %dx%d: \n" ,size , size);
             for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    if (grid[i][j]==1){
                        printf("# ");
                    }
                    else {
                         printf(". ");
                    }
                    printf(" ");
                    
                    }
    printf("\n");
}
}


int is_perfect_square(int number){ // chech if it's a real square
    int root =sqrt(number);
    if (root*root == number && root < MAX_SIZE){
        return root;
    }
    return 0;
}


int extract_boards(const char *filename, int boards[MAX_BOARDS][MAX_SIZE][MAX_SIZE], int *boards_sizes){
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error during opening file");
        return -1;
    }
    char line[1024];
    int board_count = 0;

    while(fgets(line,sizeof(line),file)) {

        line[strcspn(line, "\n")] = '\0';  //remove newline character (strcspn(line, "\n")) return the position of the first "\n"
        int len = strlen(line);

        if (is_perfect_square(len) == 0){
            printf("the line '%s' is not a perfect square, next line.\n",line);
            continue;
            }

        int size = is_perfect_square(len);
        boards_sizes[board_count] = size;

        int k=0;
        printf("Extrating grid of size %d from line : %s\n",size,line);
        for (int i =0; i < size; i++){
            for (int j =0; j < size; j++){
                boards[board_count][i][j] = line[k++] - '0'; //convert to integer by passing to ASCII code
            }
        }
        board_count++;

        if (board_count >= MAX_BOARDS){
            printf("Maximum number of grids reached\n"); //check with MAX_BOARDS number
            break;
        }
    }
fclose(file);
return board_count;
}

void tab3D_to_2D(int k,int size, int grid[][MAX_SIZE][MAX_SIZE], int output[MAX_SIZE][MAX_SIZE]) {
    
    printf(" grid %dx%d: \n" ,size , size);
             for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    output[i][j]=grid[k][i][j];
        }
        printf("\n");

    }    
 }


int main(int argc, char *argv[]) {
     if (argc != 4) {
        printf("Usage: %s <input_file> <generations> <output_file> \n", argv[0]);
        return 1;
    }
    const char *input_file = argv[1];
    int generations = atoi(argv[2]);
    const char *output_file = argv[3];

    if (generations>MAX_HISTORY){
        perror("Error number of generation too high ");
        return 1;
    }


    //check if input_txt exist

    FILE *fp = fopen(input_file,"r");
    if (!fp){
        perror("Error opening imput.txt file");
        return 1;
    }
    fclose(fp);

    if (generations <= 0){
    printf("Error : generations must be a positive integer.\n");
    return 1;
    }
    int boards[MAX_BOARDS][MAX_SIZE][MAX_SIZE]; 
    int boards_sizes[MAX_BOARDS]; 
    int board_count = extract_boards(input_file, boards, boards_sizes);
   

    if (board_count > 0) { 
         int live_cells_output[MAX_BOARDS];
        for (int k = 0 ; k<board_count;k++){

            printf("processing for line number %d\n",k+1);
            int size=boards_sizes[k];
            int grid[MAX_SIZE][MAX_SIZE] = {0};
            int next[MAX_SIZE][MAX_SIZE] = {0};
            int history[MAX_HISTORY][MAX_SIZE][MAX_SIZE];  // Array to store previous generations
            int history_count = 0;  
            tab3D_to_2D(k,size,boards,grid);

            for (int gen = 1; gen < generations; gen++) {
                printf("\n====================\n");
                printf("Generation %d:\n", gen);
                print2Dtab(size,grid);

                if (gen > 0 && grids_are_equal(size, grid, history[history_count - 1])) {
                    printf("Static pattern detected at generation %d, stopping...\n", gen + 1);
                    break;
                }

                if (detect_loop(size, grid, history, history_count)) {
                    printf("Loop pattern detected at generation %d, stopping...\n", gen + 1);
                    break;
                }

                if (history_count < MAX_HISTORY) {
                    memcpy(history[history_count], grid, sizeof(grid));
                    history_count++;
                }
                next_generation(size, grid, next);
                memcpy(grid, next, sizeof(grid));  // Update the grid for the next generation

                usleep(200000);  // Pause for 200 milliseconds
            }
            
            printf("\nfinal grid after %d gen :\n", generations);
            print2Dtab(size, grid);
            live_cells_output[k] = count_live_cells(size, grid);
            printf(" Number of alives cells = %d\n", live_cells_output[k]);
            }  
            write_array_to_file(output_file,live_cells_output,board_count);     
        } else {
        printf("Error, with the boards");
    }
    return 0; 
}



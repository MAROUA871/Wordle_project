#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>

#define MAX_WORDS 10000
#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6

/* Letter status */
#define UNUSED 0
#define WRONG_POS 1
#define CORRECT_POS 2
#define NOT_IN_WORD 3

/* Windows Console Colors */
#define COLOR_GREEN 10
#define COLOR_YELLOW 14
#define COLOR_GRAY 8
#define COLOR_WHITE 15
#define COLOR_BLACK 0
#define COLOR_RED 12
#define COLOR_CYAN 11
#define COLOR_BLUE 9

/* Function prototypes */
int loadDictionary(char dictionary[][WORD_LENGTH + 1], const char *filename);
void selectRandomWord(char dictionary[][WORD_LENGTH + 1], int wordCount, char *target);
int isValidGuess(char dictionary[][WORD_LENGTH + 1], int wordCount, const char *guess);
void provideFeedback(const char *target, const char *guess, int *feedback);
void updateLetterStatus(const char *guess, int *feedback, int letterStatus[26]);
void displayBoard(char guesses[][WORD_LENGTH + 1], int feedbacks[][WORD_LENGTH], int currentAttempt);
void displayKeyboard(int letterStatus[26]);
void displayStats(int attempts, int won);
void toUpperCase(char *str);
void clearScreen();
void setConsoleColor(int background, int text);
void resetConsoleColor();
void printTitle();
void printBox(char c, int feedback);
void waitForEnter();

int main() {
    char dictionary[MAX_WORDS][WORD_LENGTH + 1];
    char target[WORD_LENGTH + 1];
    char guess[WORD_LENGTH + 2];
    char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1];
    int feedbacks[MAX_ATTEMPTS][WORD_LENGTH];
    int letterStatus[26];
    int wordCount;
    int attempts;
    int won;
    int i, j;
    int validGuess;
    char playAgain;
    
    /* Seed random number generator */
    srand(time(NULL));
    
    /* Load dictionary from file */
    setConsoleColor(COLOR_BLACK, COLOR_CYAN);
    printf("\n  Loading dictionary...  \n");
    resetConsoleColor();
    wordCount = loadDictionary(dictionary, "words.txt");
    
    if (wordCount == 0) {
        setConsoleColor(COLOR_BLACK, COLOR_RED);
        printf("\n  ERROR: Could not load dictionary!  \n");
        resetConsoleColor();
        printf("\n  Make sure 'words.txt' exists in the same directory.\n");
        printf("  Each line should contain one 5-letter word.\n\n");
        return 1;
    }
    
    setConsoleColor(COLOR_BLACK, COLOR_GREEN);
    printf("  Successfully loaded %d words!  \n", wordCount);
    resetConsoleColor();
    printf("\n  Press ENTER to start the game...");
    waitForEnter();
    
    do {
        /* Initialize letter status (all unused) */
        for (i = 0; i < 26; i++) {
            letterStatus[i] = UNUSED;
        }
        
        /* Initialize guesses and feedbacks */
        for (i = 0; i < MAX_ATTEMPTS; i++) {
            strcpy(guesses[i], "");
            for (j = 0; j < WORD_LENGTH; j++) {
                feedbacks[i][j] = -1;
            }
        }
        
        /* Select random target word */
        selectRandomWord(dictionary, wordCount, target);
        
        won = 0;
        
        /* Game loop */
        for (attempts = 0; attempts < MAX_ATTEMPTS; attempts++) {
            clearScreen();
            printTitle();
            
            /* Display current board */
            displayBoard(guesses, feedbacks, attempts);
            printf("\n");
            
            /* Display keyboard with letter status */
            displayKeyboard(letterStatus);
            printf("\n");
            
            /* Get user input */
            printf("  ");
            setConsoleColor(COLOR_BLACK, COLOR_CYAN);
            printf(" ATTEMPT %d/%d ", attempts + 1, MAX_ATTEMPTS);
            resetConsoleColor();
            printf("\n\n  Enter your guess: ");
            
            if (fgets(guess, sizeof(guess), stdin) == NULL) {
                printf("\n  Error reading input.\n");
                break;
            }
            
            /* Remove newline character */
            i = 0;
            while (guess[i] != '\0') {
                if (guess[i] == '\n') {
                    guess[i] = '\0';
                    break;
                }
                i++;
            }
            
            /* Convert to uppercase */
            toUpperCase(guess);
            
            /* Validate guess length */
            if (strlen(guess) != WORD_LENGTH) {
                printf("\n  ");
                setConsoleColor(COLOR_BLACK, COLOR_RED);
                printf(" ERROR ");
                resetConsoleColor();
                printf(" Please enter exactly %d letters!\n", WORD_LENGTH);
                printf("\n  Press ENTER to continue...");
                waitForEnter();
                attempts--;
                continue;
            }
            
            /* Check for non-alphabetic characters */
            validGuess = 1;
            for (i = 0; i < WORD_LENGTH; i++) {
                if (!isalpha(guess[i])) {
                    validGuess = 0;
                    break;
                }
            }
            
            if (!validGuess) {
                printf("\n  ");
                setConsoleColor(COLOR_BLACK, COLOR_RED);
                printf(" ERROR ");
                resetConsoleColor();
                printf(" Only letters are allowed!\n");
                printf("\n  Press ENTER to continue...");
                waitForEnter();
                attempts--;
                continue;
            }
            
            /* Validate guess is in dictionary */
            validGuess = isValidGuess(dictionary, wordCount, guess);
            if (!validGuess) {
                printf("\n  ");
                setConsoleColor(COLOR_BLACK, COLOR_RED);
                printf(" ERROR ");
                resetConsoleColor();
                printf(" Word not in dictionary!\n");
                printf("\n  Press ENTER to continue...");
                waitForEnter();
                attempts--;
                continue;
            }
            
            /* Store guess */
            strcpy(guesses[attempts], guess);
            
            /* Provide feedback */
            provideFeedback(target, guess, feedbacks[attempts]);
            
            /* Update letter status */
            updateLetterStatus(guess, feedbacks[attempts], letterStatus);
            
            /* Check if player won */
            won = 1;
            for (i = 0; i < WORD_LENGTH; i++) {
                if (feedbacks[attempts][i] != 2) {
                    won = 0;
                    break;
                }
            }
            
            if (won) {
                clearScreen();
                printTitle();
                displayBoard(guesses, feedbacks, attempts + 1);
                printf("\n");
                displayStats(attempts + 1, won);
                printf("\n  The word was: ");
                for (i = 0; i < WORD_LENGTH; i++) {
                    setConsoleColor(COLOR_GREEN, COLOR_BLACK);
                    printf(" %c ", target[i]);
                    resetConsoleColor();
                    printf(" ");
                }
                printf("\n");
                break;
            }
        }
        
        /* Game over - player lost */
        if (!won) {
            clearScreen();
            printTitle();
            displayBoard(guesses, feedbacks, MAX_ATTEMPTS);
            printf("\n");
            displayStats(MAX_ATTEMPTS, won);
            printf("\n  The word was: ");
            for (i = 0; i < WORD_LENGTH; i++) {
                setConsoleColor(COLOR_GREEN, COLOR_BLACK);
                printf(" %c ", target[i]);
                resetConsoleColor();
                printf(" ");
            }
            printf("\n");
        }
        
        /* Ask to play again */
        printf("\n  Play again? (Y/N): ");
        scanf(" %c", &playAgain);
        while (getchar() != '\n');
        playAgain = toupper(playAgain);
        
    } while (playAgain == 'Y');
    
    clearScreen();
    printf("\n");
    setConsoleColor(COLOR_BLACK, COLOR_CYAN);
    printf("  Thanks for playing WORDLE!  ");
    resetConsoleColor();
    printf("\n\n");
    
    return 0;
}

/* Load dictionary from file */
int loadDictionary(char dictionary[][WORD_LENGTH + 1], const char *filename) {
    FILE *file;
    char buffer[100];
    int count;
    int i;
    int len;
    
    file = fopen(filename, "r");
    if (file == NULL) {
        return 0;
    }
    
    count = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && count < MAX_WORDS) {
        /* Remove newline */
        i = 0;
        while (buffer[i] != '\0') {
            if (buffer[i] == '\n' || buffer[i] == '\r') {
                buffer[i] = '\0';
                break;
            }
            i++;
        }
        
        /* Convert to uppercase */
        toUpperCase(buffer);
        
        /* Check if word is exactly 5 letters */
        len = strlen(buffer);
        if (len == WORD_LENGTH) {
            strcpy(dictionary[count], buffer);
            count++;
        }
    }
    
    fclose(file);
    return count;
}

/* Select a random word from dictionary */
void selectRandomWord(char dictionary[][WORD_LENGTH + 1], int wordCount, char *target) {
    int randomIndex;
    
    randomIndex = rand() % wordCount;
    strcpy(target, dictionary[randomIndex]);
}

/* Check if guess is in dictionary */
int isValidGuess(char dictionary[][WORD_LENGTH + 1], int wordCount, const char *guess) {
    int i;
    
    for (i = 0; i < wordCount; i++) {
        if (strcmp(dictionary[i], guess) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Provide feedback for a guess */
void provideFeedback(const char *target, const char *guess, int *feedback) {
    int i, j;
    int targetUsed[WORD_LENGTH];
    int guessUsed[WORD_LENGTH];
    
    /* Initialize arrays */
    for (i = 0; i < WORD_LENGTH; i++) {
        targetUsed[i] = 0;
        guessUsed[i] = 0;
        feedback[i] = 0;
    }
    
    /* First pass: mark correct positions (green) */
    for (i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == target[i]) {
            feedback[i] = 2;
            targetUsed[i] = 1;
            guessUsed[i] = 1;
        }
    }
    
    /* Second pass: mark wrong positions (yellow) */
    for (i = 0; i < WORD_LENGTH; i++) {
        if (guessUsed[i]) {
            continue;
        }
        
        for (j = 0; j < WORD_LENGTH; j++) {
            if (!targetUsed[j] && guess[i] == target[j]) {
                feedback[i] = 1;
                targetUsed[j] = 1;
                break;
            }
        }
    }
}

/* Update letter status based on feedback */
void updateLetterStatus(const char *guess, int *feedback, int letterStatus[26]) {
    int i;
    int letterIndex;
    
    for (i = 0; i < WORD_LENGTH; i++) {
        letterIndex = guess[i] - 'A';
        
        if (letterIndex < 0 || letterIndex >= 26) {
            continue;
        }
        
        if (feedback[i] == 2) {
            letterStatus[letterIndex] = CORRECT_POS;
        } else if (feedback[i] == 1) {
            if (letterStatus[letterIndex] != CORRECT_POS) {
                letterStatus[letterIndex] = WRONG_POS;
            }
        } else {
            if (letterStatus[letterIndex] == UNUSED) {
                letterStatus[letterIndex] = NOT_IN_WORD;
            }
        }
    }
}

/* Display the game board with colored boxes */
void displayBoard(char guesses[][WORD_LENGTH + 1], int feedbacks[][WORD_LENGTH], int currentAttempt) {
    int i, j;
    
    printf("\n");
    for (i = 0; i < MAX_ATTEMPTS; i++) {
        printf("       ");
        
        if (i < currentAttempt) {
            /* Display completed guess with colors */
            for (j = 0; j < WORD_LENGTH; j++) {
                printBox(guesses[i][j], feedbacks[i][j]);
            }
        } else {
            /* Display empty boxes */
            for (j = 0; j < WORD_LENGTH; j++) {
                setConsoleColor(COLOR_WHITE, COLOR_BLACK);
                printf("   ");
                resetConsoleColor();
                printf(" ");
            }
        }
        printf("\n");
    }
}

/* Display QWERTY keyboard with letter status */
void displayKeyboard(int letterStatus[26]) {
    char row1[] = "QWERTYUIOP";
    char row2[] = "ASDFGHJKL";
    char row3[] = "ZXCVBNM";
    int i;
    int letterIndex;
    char letter;
    
    printf("  KEYBOARD:\n");
    printf("  ");
    for (i = 0; i < 50; i++) {
        printf("-");
    }
    printf("\n");
    
    /* First row */
    printf("    ");
    i = 0;
    while (row1[i] != '\0') {
        letter = row1[i];
        letterIndex = letter - 'A';
        
        if (letterStatus[letterIndex] == CORRECT_POS) {
            setConsoleColor(COLOR_GREEN, COLOR_BLACK);
        } else if (letterStatus[letterIndex] == WRONG_POS) {
            setConsoleColor(COLOR_YELLOW, COLOR_BLACK);
        } else if (letterStatus[letterIndex] == NOT_IN_WORD) {
            setConsoleColor(COLOR_GRAY, COLOR_WHITE);
        } else {
            setConsoleColor(COLOR_WHITE, COLOR_BLACK);
        }
        
        printf(" %c ", letter);
        resetConsoleColor();
        printf(" ");
        i++;
    }
    printf("\n");
    
    /* Second row */
    printf("     ");
    i = 0;
    while (row2[i] != '\0') {
        letter = row2[i];
        letterIndex = letter - 'A';
        
        if (letterStatus[letterIndex] == CORRECT_POS) {
            setConsoleColor(COLOR_GREEN, COLOR_BLACK);
        } else if (letterStatus[letterIndex] == WRONG_POS) {
            setConsoleColor(COLOR_YELLOW, COLOR_BLACK);
        } else if (letterStatus[letterIndex] == NOT_IN_WORD) {
            setConsoleColor(COLOR_GRAY, COLOR_WHITE);
        } else {
            setConsoleColor(COLOR_WHITE, COLOR_BLACK);
        }
        
        printf(" %c ", letter);
        resetConsoleColor();
        printf(" ");
        i++;
    }
    printf("\n");
    
    /* Third row */
    printf("       ");
    i = 0;
    while (row3[i] != '\0') {
        letter = row3[i];
        letterIndex = letter - 'A';
        
        if (letterStatus[letterIndex] == CORRECT_POS) {
            setConsoleColor(COLOR_GREEN, COLOR_BLACK);
        } else if (letterStatus[letterIndex] == WRONG_POS) {
            setConsoleColor(COLOR_YELLOW, COLOR_BLACK);
        } else if (letterStatus[letterIndex] == NOT_IN_WORD) {
            setConsoleColor(COLOR_GRAY, COLOR_WHITE);
        } else {
            setConsoleColor(COLOR_WHITE, COLOR_BLACK);
        }
        
        printf(" %c ", letter);
        resetConsoleColor();
        printf(" ");
        i++;
    }
    printf("\n");
    
    printf("  ");
    for (i = 0; i < 50; i++) {
        printf("-");
    }
    printf("\n");
}

/* Display statistics after game ends */
void displayStats(int attempts, int won) {
    int i;
    
    printf("\n");
    printf("  ");
    for (i = 0; i < 50; i++) {
        printf("=");
    }
    printf("\n");
    
    if (won) {
        printf("            ");
        setConsoleColor(COLOR_GREEN, COLOR_WHITE);
        printf(" C O N G R A T U L A T I O N S ! ");
        resetConsoleColor();
        printf("\n\n");
        printf("              You won in %d attempt(s)!\n", attempts);
        
        if (attempts == 1) {
            printf("              AMAZING! First try!\n");
        } else if (attempts <= 3) {
            printf("              Excellent performance!\n");
        } else if (attempts <= 5) {
            printf("              Well done!\n");
        } else {
            printf("              You made it!\n");
        }
    } else {
        printf("               ");
        setConsoleColor(COLOR_RED, COLOR_WHITE);
        printf(" G A M E   O V E R ");
        resetConsoleColor();
        printf("\n\n");
        printf("              Better luck next time!\n");
    }
    
    printf("  ");
    for (i = 0; i < 50; i++) {
        printf("=");
    }
    printf("\n");
}

/* Print game title */
void printTitle() {
    int i;
    
    printf("\n");
    printf("  ");
    for (i = 0; i < 50; i++) {
        printf("=");
    }
    printf("\n");
    
    printf("           ");
    setConsoleColor(COLOR_GREEN, COLOR_BLACK);
    printf(" W ");
    resetConsoleColor();
    printf(" ");
    setConsoleColor(COLOR_YELLOW, COLOR_BLACK);
    printf(" O ");
    resetConsoleColor();
    printf(" ");
    setConsoleColor(COLOR_GREEN, COLOR_BLACK);
    printf(" R ");
    resetConsoleColor();
    printf(" ");
    setConsoleColor(COLOR_YELLOW, COLOR_BLACK);
    printf(" D ");
    resetConsoleColor();
    printf(" ");
    setConsoleColor(COLOR_GREEN, COLOR_BLACK);
    printf(" L ");
    resetConsoleColor();
    printf(" ");
    setConsoleColor(COLOR_YELLOW, COLOR_BLACK);
    printf(" E ");
    resetConsoleColor();
    printf("   G A M E\n");
    
    printf("  ");
    for (i = 0; i < 50; i++) {
        printf("=");
    }
    printf("\n");
}

/* Print a single colored box */
void printBox(char c, int feedback) {
    if (feedback == 2) {
        setConsoleColor(COLOR_GREEN, COLOR_BLACK);
    } else if (feedback == 1) {
        setConsoleColor(COLOR_YELLOW, COLOR_BLACK);
    } else {
        setConsoleColor(COLOR_GRAY, COLOR_WHITE);
    }
    
    printf(" %c ", c);
    resetConsoleColor();
    printf(" ");
}

/* Convert string to uppercase */
void toUpperCase(char *str) {
    int i;
    
    i = 0;
    while (str[i] != '\0') {
        str[i] = toupper((unsigned char)str[i]);
        i++;
    }
}

/* Clear screen */
void clearScreen() {
    system("cls");
}

/* Set console color using Windows API */
void setConsoleColor(int background, int text) {
    HANDLE hConsole;
    int colorAttribute;
    
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    colorAttribute = (background << 4) | text;
    SetConsoleTextAttribute(hConsole, colorAttribute);
}

/* Reset console color to default */
void resetConsoleColor() {
    HANDLE hConsole;
    
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
}

/* Wait for user to press ENTER */
void waitForEnter() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
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
typedef enum { UNUSED, WRONG_POS, CORRECT_POS, NOT_IN_WORD } LetterStatus;

/* Windows Console Colors */
#define COLOR_GREEN 10
#define COLOR_YELLOW 14
#define COLOR_GRAY 8
#define COLOR_WHITE 15
#define COLOR_BLACK 0
#define COLOR_RED 12
#define COLOR_CYAN 11

/* Global handle for console */
static HANDLE hConsole;

/* Function prototypes */
int loadDictionary(char dictionary[][WORD_LENGTH + 1], const char *filename);
void selectRandomWord(char dictionary[][WORD_LENGTH + 1], int wordCount, char *target);
int isValidGuess(char dictionary[][WORD_LENGTH + 1], int wordCount, const char *guess);
void provideFeedback(const char *target, const char *guess, int *feedback);
void updateLetterStatus(const char *guess, const int *feedback, LetterStatus letterStatus[26]);
void displayBoard(char guesses[][WORD_LENGTH + 1], int feedbacks[][WORD_LENGTH], int currentAttempt);
void displayKeyboard(const LetterStatus letterStatus[26]);
void displayStats(int attempts, int won);
void toUpperCase(char *str);
void setColor(int background, int text);
void printTitle();
void printBox(char c, int feedback);

int main() {
    char dictionary[MAX_WORDS][WORD_LENGTH + 1];
    char target[WORD_LENGTH + 1];
    char guess[WORD_LENGTH + 2];
    char guesses[MAX_ATTEMPTS][WORD_LENGTH + 1] = {0};
    int feedbacks[MAX_ATTEMPTS][WORD_LENGTH];
    LetterStatus letterStatus[26];
    int wordCount, attempts, won;
    char playAgain;
    
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    srand((unsigned)time(NULL));
    
    /* Load dictionary */
    setColor(COLOR_BLACK, COLOR_CYAN);
    printf("\n  Loading dictionary...  \n");
    setColor(0, 7);
    
    wordCount = loadDictionary(dictionary, "words.txt");
    if (wordCount == 0) {
        setColor(COLOR_BLACK, COLOR_RED);
        printf("\n  ERROR: Could not load dictionary!  \n");
        setColor(0, 7);
        printf("\n  Make sure 'words.txt' exists in the same directory.\n"
               "  Each line should contain one 5-letter word.\n\n");
        return 1;
    }
    
    setColor(COLOR_BLACK, COLOR_GREEN);
    printf("  Successfully loaded %d words!  \n", wordCount);
    setColor(0, 7);
    printf("\n  Press ENTER to start the game...");
    while (getchar() != '\n');
    
    do {
        /* Initialize game state */
        memset(letterStatus, UNUSED, sizeof(letterStatus));
        memset(guesses, 0, sizeof(guesses));
        memset(feedbacks, -1, sizeof(feedbacks));
        
        selectRandomWord(dictionary, wordCount, target);
        won = 0;
        
        /* Game loop */
        for (attempts = 0; attempts < MAX_ATTEMPTS && !won; attempts++) {
            system("cls");
            printTitle();
            displayBoard(guesses, feedbacks, attempts);
            printf("\n");
            displayKeyboard(letterStatus);
            
            printf("\n  ");
            setColor(COLOR_BLACK, COLOR_CYAN);
            printf(" ATTEMPT %d/%d ", attempts + 1, MAX_ATTEMPTS);
            setColor(0, 7);
            printf("\n\n  Enter your guess: ");
            
            if (!fgets(guess, sizeof(guess), stdin)) break;
            
            /* Remove newline and convert to uppercase */
            guess[strcspn(guess, "\n")] = '\0';
            toUpperCase(guess);
            
            /* Validate input */
            if (strlen(guess) != WORD_LENGTH) {
                printf("\n  ");
                setColor(COLOR_BLACK, COLOR_RED);
                printf(" ERROR ");
                setColor(0, 7);
                printf(" Please enter exactly %d letters!\n\n  Press ENTER to continue...", WORD_LENGTH);
                while (getchar() != '\n');
                attempts--;
                continue;
            }
            
            int valid = 1;
            for (int i = 0; i < WORD_LENGTH && valid; i++)
                valid = isalpha((unsigned char)guess[i]);
            
            if (!valid) {
                printf("\n  ");
                setColor(COLOR_BLACK, COLOR_RED);
                printf(" ERROR ");
                setColor(0, 7);
                printf(" Only letters are allowed!\n\n  Press ENTER to continue...");
                while (getchar() != '\n');
                attempts--;
                continue;
            }
            
            if (!isValidGuess(dictionary, wordCount, guess)) {
                printf("\n  ");
                setColor(COLOR_BLACK, COLOR_RED);
                printf(" ERROR ");
                setColor(0, 7);
                printf(" Word not in dictionary!\n\n  Press ENTER to continue...");
                while (getchar() != '\n');
                attempts--;
                continue;
            }
            
            /* Process guess */
            strcpy(guesses[attempts], guess);
            provideFeedback(target, guess, feedbacks[attempts]);
            updateLetterStatus(guess, feedbacks[attempts], letterStatus);
            
            /* Check win condition */
            won = 1;
            for (int i = 0; i < WORD_LENGTH; i++)
                if (feedbacks[attempts][i] != CORRECT_POS) { won = 0; break; }
        }
        
        /* Display final board */
        system("cls");
        printTitle();
        displayBoard(guesses, feedbacks, won ? attempts : MAX_ATTEMPTS);
        printf("\n");
        displayStats(won ? attempts : MAX_ATTEMPTS, won);
        
        printf("\n  The word was: ");
        for (int i = 0; i < WORD_LENGTH; i++) {
            setColor(COLOR_GREEN, COLOR_BLACK);
            printf(" %c ", target[i]);
            setColor(0, 7);
            printf(" ");
        }
        
        printf("\n\n  Play again? (Y/N): ");
        scanf(" %c", &playAgain);
        while (getchar() != '\n');
        
    } while (toupper(playAgain) == 'Y');
    
    system("cls");
    printf("\n");
    setColor(COLOR_BLACK, COLOR_CYAN);
    printf("  Thanks for playing WORDLE!  ");
    setColor(0, 7);
    printf("\n\n");
    
    return 0;
}

int loadDictionary(char dictionary[][WORD_LENGTH + 1], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    
    char buffer[100];
    int count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && count < MAX_WORDS) {
        buffer[strcspn(buffer, "\r\n")] = '\0';
        toUpperCase(buffer);
        
        if (strlen(buffer) == WORD_LENGTH) {
            strcpy(dictionary[count++], buffer);
        }
    }
    
    fclose(file);
    return count;
}

void selectRandomWord(char dictionary[][WORD_LENGTH + 1], int wordCount, char *target) {
    strcpy(target, dictionary[rand() % wordCount]);
}

int isValidGuess(char dictionary[][WORD_LENGTH + 1], int wordCount, const char *guess) {
    for (int i = 0; i < wordCount; i++)
        if (strcmp(dictionary[i], guess) == 0) return 1;
    return 0;
}

void provideFeedback(const char *target, const char *guess, int *feedback) {
    int targetUsed[WORD_LENGTH] = {0}, guessUsed[WORD_LENGTH] = {0};
    
    /* Mark correct positions */
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == target[i]) {
            feedback[i] = CORRECT_POS;
            targetUsed[i] = guessUsed[i] = 1;
        } else {
            feedback[i] = 0;
        }
    }
    
    /* Mark wrong positions */
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (!guessUsed[i]) {
            for (int j = 0; j < WORD_LENGTH; j++) {
                if (!targetUsed[j] && guess[i] == target[j]) {
                    feedback[i] = WRONG_POS;
                    targetUsed[j] = 1;
                    break;
                }
            }
        }
    }
}

void updateLetterStatus(const char *guess, const int *feedback, LetterStatus letterStatus[26]) {
    for (int i = 0; i < WORD_LENGTH; i++) {
        int idx = guess[i] - 'A';
        if (idx < 0 || idx >= 26) continue;
        
        if (feedback[i] == CORRECT_POS) {
            letterStatus[idx] = CORRECT_POS;
        } else if (feedback[i] == WRONG_POS && letterStatus[idx] != CORRECT_POS) {
            letterStatus[idx] = WRONG_POS;
        } else if (letterStatus[idx] == UNUSED) {
            letterStatus[idx] = NOT_IN_WORD;
        }
    }
}

void displayBoard(char guesses[][WORD_LENGTH + 1], int feedbacks[][WORD_LENGTH], int currentAttempt) {
    printf("\n");
    for (int i = 0; i < MAX_ATTEMPTS; i++) {
        printf("       ");
        
        if (i < currentAttempt) {
            for (int j = 0; j < WORD_LENGTH; j++)
                printBox(guesses[i][j], feedbacks[i][j]);
        } else {
            for (int j = 0; j < WORD_LENGTH; j++) {
                setColor(COLOR_WHITE, COLOR_BLACK);
                printf("   ");
                setColor(0, 7);
                printf(" ");
            }
        }
        printf("\n");
    }
}

void displayKeyboard(const LetterStatus letterStatus[26]) {
    const char *rows[] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
    const int spacing[] = {4, 5, 7};
    
    printf("  KEYBOARD:\n  ");
    for (int i = 0; i < 50; i++) printf("-");
    printf("\n");
    
    for (int r = 0; r < 3; r++) {
        printf("%*s", spacing[r], "");
        for (int i = 0; rows[r][i]; i++) {
            int idx = rows[r][i] - 'A';
            
            switch (letterStatus[idx]) {
                case CORRECT_POS: setColor(COLOR_GREEN, COLOR_BLACK); break;
                case WRONG_POS: setColor(COLOR_YELLOW, COLOR_BLACK); break;
                case NOT_IN_WORD: setColor(COLOR_GRAY, COLOR_WHITE); break;
                default: setColor(COLOR_WHITE, COLOR_BLACK);
            }
            
            printf(" %c ", rows[r][i]);
            setColor(0, 7);
            printf(" ");
        }
        printf("\n");
    }
    
    printf("  ");
    for (int i = 0; i < 50; i++) printf("-");
    printf("\n");
}

void displayStats(int attempts, int won) {
    printf("\n  ");
    for (int i = 0; i < 50; i++) printf("=");
    printf("\n");
    
    if (won) {
        printf("            ");
        setColor(COLOR_GREEN, COLOR_WHITE);
        printf(" C O N G R A T U L A T I O N S ! ");
        setColor(0, 7);
        printf("\n\n              You won in %d attempt(s)!\n", attempts);
        
        const char *msgs[] = {"AMAZING! First try!", "Excellent performance!", 
                             "Well done!", "You made it!"};
        printf("              %s\n", msgs[attempts <= 1 ? 0 : attempts <= 3 ? 1 : attempts <= 5 ? 2 : 3]);
    } else {
        printf("               ");
        setColor(COLOR_RED, COLOR_WHITE);
        printf(" G A M E   O V E R ");
        setColor(0, 7);
        printf("\n\n              Better luck next time!\n");
    }
    
    printf("  ");
    for (int i = 0; i < 50; i++) printf("=");
    printf("\n");
}

void printTitle() {
    printf("\n  ");
    for (int i = 0; i < 50; i++) printf("=");
    printf("\n           ");
    
    int colors[] = {COLOR_GREEN, COLOR_YELLOW, COLOR_GREEN, COLOR_YELLOW, COLOR_GREEN, COLOR_YELLOW};
    const char *letters = "WORDLE";
    
    for (int i = 0; i < 6; i++) {
        setColor(colors[i], COLOR_BLACK);
        printf(" %c ", letters[i]);
        setColor(0, 7);
        printf(" ");
    }
    
    printf("  G A M E\n  ");
    for (int i = 0; i < 50; i++) printf("=");
    printf("\n");
}

void printBox(char c, int feedback) {
    setColor(feedback == CORRECT_POS ? COLOR_GREEN : 
             feedback == WRONG_POS ? COLOR_YELLOW : COLOR_GRAY,
             feedback == NOT_IN_WORD ? COLOR_WHITE : COLOR_BLACK);
    printf(" %c ", c);
    setColor(0, 7);
    printf(" ");
}

void toUpperCase(char *str) {
    for (int i = 0; str[i]; i++)
        str[i] = toupper((unsigned char)str[i]);
}

void setColor(int background, int text) {
    SetConsoleTextAttribute(hConsole, (background << 4) | text);
}
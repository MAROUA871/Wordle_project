#ifndef WORDLE_GAME_H
#define WORDLE_GAME_H

/* ===== Includes nécessaires ===== */
#include <windows.h>

/* ===== Constantes ===== */
#define MAX_WORDS 10000
#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6

/* ===== Couleurs console ===== */
#define COLOR_GREEN 10
#define COLOR_YELLOW 14
#define COLOR_GRAY 8
#define COLOR_WHITE 15
#define COLOR_BLACK 0
#define COLOR_RED 12
#define COLOR_CYAN 11

/* ===== Types ===== */
typedef enum {
    UNUSED,
    WRONG_POS,
    CORRECT_POS,
    NOT_IN_WORD
} LetterStatus;

/* ===== Variables globales ===== */
extern HANDLE hConsole;

/* ===== Prototypes des fonctions ===== */
int loadDictionary(char dictionary[][WORD_LENGTH + 1], const char *filename);
void selectRandomWord(char dictionary[][WORD_LENGTH + 1], int wordCount, char *target);
int isValidGuess(char dictionary[][WORD_LENGTH + 1], int wordCount, const char *guess);
void provideFeedback(const char *target, const char *guess, int *feedback);
void updateLetterStatus(const char *guess, const int *feedback, LetterStatus letterStatus[26]);
void displayBoard(char guesses[][WORD_LENGTH + 1], int feedbacks[][WORD_LENGTH], int currentAttempt);
void displayKeyboard(const LetterStatus letterStatus[26]);
void displayStats(int attempts, int won);
void printTitle();
void printBox(char c, int feedback);
void toUpperCase(char *str);
void setColor(int background, int text);

#endif /* WORDLE_GAME_H */
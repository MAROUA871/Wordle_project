#ifndef SOLVER_GAME_H
#define SOLVER_GAME_H

#define WORD_LEN 5
#define MAX_WORDS 10000

// Feedback
#define NOT_IN_WORD 0
#define WRONG_POS   1
#define CORRECT_POS 2

// Dictionnaire
extern char dictionary[MAX_WORDS][WORD_LEN + 1];
extern int wordCount;

// Fonctions du solver
void loadDictionary(const char *filename);
void computeFeedback(const char *secret, const char *guess, int feedback[WORD_LEN]);
int  isWordCompatible(const char *word, const char *guess, int feedback[WORD_LEN]);
void filterDictionary(const char *guess, int feedback[WORD_LEN]);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 10000
#define WORD_LENGTH 5
#define MAX_ATTEMPTS 6

typedef enum { UNUSED, WRONG_POS, CORRECT_POS, NOT_IN_WORD } LetterStatus;

int loadDictionary(char dict[][WORD_LENGTH + 1], const char *filename);
void toUpperCase(char *str);
int scoreWord(const char *word, const int letterFreq[26], const LetterStatus status[26]);
void updateConstraints(const char *guess, const int *feedback, char known[WORD_LENGTH], 
                       LetterStatus status[26], char excluded[26]);
int matchesConstraints(const char *word, const char known[WORD_LENGTH], 
                       const LetterStatus status[26], const char excluded[26]);
void calculateLetterFreq(char dict[][WORD_LENGTH + 1], int count, int freq[26]);

int main() {
    char dict[MAX_WORDS][WORD_LENGTH + 1];
    char known[WORD_LENGTH] = {0};
    LetterStatus status[26] = {UNUSED};
    char excluded[26] = {0};
    int letterFreq[26] = {0};
    int wordCount, attempt;
    int i;
    
    printf("\n=== WORDLE SOLVER ===\n\n");
    
    wordCount = loadDictionary(dict, "words.txt");
    if (wordCount == 0) {
        printf("ERROR: Could not load dictionary!\n");
        return 1;
    }
    
    printf("Loaded %d words.\n\n", wordCount);
    calculateLetterFreq(dict, wordCount, letterFreq);
    
    for (attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
        int bestScore = -1, bestIdx = 0, candidates = 0;
        
        /* Find best word */
        for ( i = 0; i < wordCount; i++) {
            if (matchesConstraints(dict[i], known, status, excluded)) {
                candidates++;
                int score = scoreWord(dict[i], letterFreq, status);
                if (score > bestScore) {
                    bestScore = score;
                    bestIdx = i;
                }
            }
        }
        
        printf("Attempt %d: ", attempt);
        printf("Suggest '%s' (%d candidates)\n", dict[bestIdx], candidates);
        
        if (candidates == 1) {
            printf("\nSOLVED! The word is: %s\n\n", dict[bestIdx]);
            break;
        }
        
        /* Get feedback */
        printf("Enter feedback (G=Green, Y=Yellow, .):\n> ");
        char feedback[WORD_LENGTH + 2];
        if (!fgets(feedback, sizeof(feedback), stdin)) break;
        feedback[strcspn(feedback, "\n")] = 0;
        toUpperCase(feedback);
        
        if (strlen(feedback) != WORD_LENGTH) {
            printf("Invalid feedback length!\n");
            attempt--;
            continue;
        }
        
        /* Check if won */
        int won = 1;
        for (int i = 0; i < WORD_LENGTH; i++)
            if (feedback[i] != 'G') { won = 0; break; }
        
        if (won) {
            printf("\nSOLVED in %d attempts!\n\n", attempt);
            break;
        }
        
        /* Update constraints */
        int fb[WORD_LENGTH];
        for (i = 0; i < WORD_LENGTH; i++) {
            if (feedback[i] == 'G') fb[i] = CORRECT_POS;
            else if (feedback[i] == 'Y') fb[i] = WRONG_POS;
            else fb[i] = NOT_IN_WORD;
        }
        updateConstraints(dict[bestIdx], fb, known, status, excluded);
        printf("\n");
    }
    
    return 0;
}

int loadDictionary(char dict[][WORD_LENGTH + 1], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    
    char buffer[100];
    int count = 0;
    
    while (fgets(buffer, sizeof(buffer), file) && count < MAX_WORDS) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        toUpperCase(buffer);
        if (strlen(buffer) == WORD_LENGTH)
            strcpy(dict[count++], buffer);
    }
    
    fclose(file);
    return count;
}

void calculateLetterFreq(char dict[][WORD_LENGTH + 1], int count, int freq[26]) {
	int i;
    for ( i = 0; i < count; i++)
        for (int j = 0; j < WORD_LENGTH; j++) {
            int idx = dict[i][j] - 'A';
            if (idx >= 0 && idx < 26) freq[idx]++;
        }
}

int scoreWord(const char *word, const int letterFreq[26], const LetterStatus status[26]) {
    int score = 0;
    int used[26] = {0};
    
    for (int i = 0; i < WORD_LENGTH; i++) {
        int idx = word[i] - 'A';
        if (idx < 0 || idx >= 26) continue;
        
        if (!used[idx]) {
            score += letterFreq[idx];
            used[idx] = 1;
        }
        
        if (status[idx] == UNUSED) score += 50;
    }
    
    return score;
}

void updateConstraints(const char *guess, const int *feedback, char known[WORD_LENGTH], 
                       LetterStatus status[26], char excluded[26]) {
    for (int i = 0; i < WORD_LENGTH; i++) {
        int idx = guess[i] - 'A';
        if (idx < 0 || idx >= 26) continue;
        
        if (feedback[i] == CORRECT_POS) {
            known[i] = guess[i];
            status[idx] = CORRECT_POS;
        } else if (feedback[i] == WRONG_POS) {
            if (status[idx] != CORRECT_POS)
                status[idx] = WRONG_POS;
        } else {
            excluded[idx] = 1;
        }
    }
}

int matchesConstraints(const char *word, const char known[WORD_LENGTH], 
                       const LetterStatus status[26], const char excluded[26]) {
    /* Check known positions */
    int i;
    for ( i = 0; i < WORD_LENGTH; i++)
        if (known[i] && word[i] != known[i]) return 0;
    
    /* Check excluded letters */
    for (i = 0; i < WORD_LENGTH; i++) {
        int idx = word[i] - 'A';
        if (idx >= 0 && idx < 26 && excluded[idx]) return 0;
    }
    
    /* Check letters that must be present but in wrong position */
    for ( i = 0; i < 26; i++) {
        if (status[i] == WRONG_POS) {
            char letter = 'A' + i;
            int found = 0;
            for (int j = 0; j < WORD_LENGTH; j++)
                if (word[j] == letter) { found = 1; break; }
            if (!found) return 0;
        }
    }
    
    return 1;
}

void toUpperCase(char *str) {
	int i;
    for ( i = 0; str[i]; i++)
        str[i] = toupper((unsigned char)str[i]);
}
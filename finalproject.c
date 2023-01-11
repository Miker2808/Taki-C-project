// Michael Pogodin - 323446070
//
// Start of includes area
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
// End of includes area
//
// Start of defines area
#define MAX_NAME_LENGTH 20
#define START_CARDS_COUNT 4
#define INT_TO_ASCII 48
// cards types
#define PLUS_CARD 10
#define STOP_CARD 11
#define DIRECTION_CARD 12
#define CHANGE_COLOR_CARD 13
#define TAKI_CARD 14
#define RED_CARD 'R'
#define BLUE_CARD 'B'
#define GREEN_CARD 'G'
#define YELLO_CARD 'Y'
#define COLORLESS_CARD ' '


// End of defines area
//
// Structures area
struct Card {
    char cardColor;
    int cardNumber;
};

struct Player{
    char playerName[MAX_NAME_LENGTH];
    struct Card* cardsArray;
    int cardsCount;

};

// End of structures area
//
// Start of declerations area
void printWelcome();
void printCard(struct Card card);
int askPlayersCount();
struct Player * queryPlayers(int playersCount);
struct Card generateCard();
void freePlayersMemory(struct Player * playersArray, int playersCount);
// End of declerations area

void main(){
    int playersCount;
    int statsArray[14][50];
    struct Player * playersArray;

    // random seed
    srand(time(NULL));

    printWelcome();
    playersCount = askPlayersCount();
    playersArray = queryPlayers(playersCount);

    // allocation and freeing works.
    for(int i=0; i < playersCount; i++){

        printf("Name: %s\n", playersArray[i].playerName);
        
        for(int card=0; card<START_CARDS_COUNT; card++){
            printf("Card #%d:\n", card+1);
            printCard(playersArray[i].cardsArray[card]);
            printf("\n");
        }
        
    }

    freePlayersMemory(playersArray, playersCount);
    printf("finished.");

}

// start of definitions area
void printWelcome(){
    printf("************  Welcome to TAKI game !!! ***********\n");
}

int askPlayersCount(){
    int playersCount;
    printf("Please enter the number of players:\n");
    scanf("%d", &playersCount);
    return playersCount;
}

struct Player * queryPlayers(int playersCount){
    struct Player * playersArray = malloc(sizeof(struct Player) * playersCount);

    for(int playerIdx=0; playerIdx < playersCount; playerIdx++){
        // ask, name, assign cards, and so on..
        printf("Please enter the first name of player #%d:\n", playerIdx+1);
        scanf("%s", playersArray[playerIdx].playerName);

        // create cards array pointer
        struct Card * cardsArray = malloc(sizeof(struct Card) * START_CARDS_COUNT);
        // assign pointer for the player
        playersArray[playerIdx].cardsArray = cardsArray;
        for(int cardIdx=0; cardIdx < START_CARDS_COUNT; cardIdx++){
            // generate card and assign into array
            playersArray[playerIdx].cardsArray[cardIdx] = generateCard();

        }
        
    }

    return playersArray;
}

void printCard(struct Card card){

    // print two lines
    printf("*********\n");
    printf("*       *\n");
    // print card type line based on switch.
    switch (card.cardNumber)
    {
    case PLUS_CARD:
        printf("*   +   *\n");
        break;
    case STOP_CARD:
        printf("*  STOP *\n");
        break;
    case DIRECTION_CARD:
        printf("*  <->  *\n");
        break;
    case CHANGE_COLOR_CARD:
        printf("* COLOR *\n");
        break;
    case TAKI_CARD:
        printf("*  TAKI *\n");
        break;

    default:
        printf("*   %d   *\n", card.cardNumber);
    }
    printf("*   %c   *\n", card.cardColor);
    printf("*       *\n");
    printf("*********\n");
}

struct Card generateCard(){
    struct Card newCard;
    int cardNumber = 1 + rand() % 14; // random card (1-9, or type (10-14))
    int cardColor = rand() % 4; // random from 0 to 3
    newCard.cardNumber = cardNumber;
    // assign card as colorless if it is a COLOR_CARD
    if(newCard.cardNumber == CHANGE_COLOR_CARD){
        newCard.cardColor = COLORLESS_CARD;
    }
    else{
        switch (cardColor)
        {
        case 0:
            newCard.cardColor = 'R';
            break;
        case 1:
            newCard.cardColor = 'B';
            break;
        case 2:
            newCard.cardColor = 'G';
            break;
        case 3:
            newCard.cardColor = 'Y';
            break;
        default:
            break;
        }
    }

    return newCard;
}


void freePlayersMemory(struct Player * playersArray, int playersCount){
    struct Card * cardPointer;
    for(int i=0; i<playersCount; i++){
        // free each card array from its pointer for each player
        free(playersArray[i].cardsArray);
        printf("freed cards %d\n", i+1);
    }
    // free the players array.
    free(playersArray);
    printf("freed array\n");
}
// end of file
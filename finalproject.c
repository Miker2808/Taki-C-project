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
#define DECK_RESIZE_VALUE 5
#define NUMBER_CARD_ONLY true
#define ANY_CARD false
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
    unsigned int cardNumber;
};

struct Player{
    char playerName[MAX_NAME_LENGTH];
    struct Card* cardsArray;
    unsigned int cardCount;
    unsigned int cardDeckSize;

};
// End of structures area
//
// Start of declerations area
void printWelcome();
void printCard(struct Card card);
int askPlayersCount();
struct Player * queryPlayers(unsigned int playersCount);
struct Card generateCard(bool numberOnly);
void appendCardToPlayer(struct Player * player, struct Card card);
void removePlayerCard(struct Player * player, unsigned int cardIndex);
void printPlayersDeck(struct Player player);

// do not put functions here! (Note for me, mike)
void checkValidAllocation(void * ptr);
void freePlayersMemory(struct Player * playersArray, int playersCount);
// End of declerations area

void main(){
    // use system time as the random seed, keep as first call on main.
    srand(time(NULL));
    // dynamic variables.
    bool gameFinished = false;
    int playersCount;
    int statsArray[15];
    struct Player * playersArray;
    struct Card upperCard = generateCard(NUMBER_CARD_ONLY);
    int currentPlayerIndex = 0;
    int gameDirectionIsForward = true;

    
    

    printWelcome();
    playersCount = askPlayersCount();
    playersArray = queryPlayers(playersCount);

    while(!gameFinished){
        // (V) print upper card
        printf("Upper card:\n");
        printCard(upperCard);
        // (V) print the players whose turn it is his cards deck
        printPlayersDeck(playersArray[currentPlayerIndex]);
        // () ask the player whose turn it is to choose cards

        // () check validity of input (inside above step)

        // () if color card, ask to assign color

        // () check validity of cards

        // () assign chosen card as upper card
        
        // () append card to statistics array
        printf("Continue?\n");
        getchar();
    }
    
    // allocation and freeing works.
    // for(int i=0; i < playersCount; i++){

    //     printf("Name: %s\n", playersArray[i].playerName);
        
    //     for(int card=0; card<START_CARDS_COUNT; card++){
    //         printf("Card #%d:\n", card+1);
    //         printCard(playersArray[i].cardsArray[card]);
    //     }
        
    // }

    freePlayersMemory(playersArray, playersCount);
    printf("finished.");

}

// start of definitions area
void printWelcome(){
    printf("************  Welcome to TAKI game !!! ***********\n");
}

int askPlayersCount(){
    int playersCount = 0;
    while(playersCount <= 0){
        printf("Please enter the number of players:\n");
        scanf("%d", &playersCount);
        if(playersCount <= 0){
            printf("Invalid input. positive integer is required (n > 0).\n");
        }
    }
    
    return playersCount;
}

struct Player * queryPlayers(unsigned int playersCount){
    struct Player * playersArray = malloc(sizeof(struct Player) * playersCount);

    for(int playerIdx=0; playerIdx < playersCount; playerIdx++){
        // ask, name, assign cards, and so on..
        printf("Please enter the first name of player #%d:\n", playerIdx+1);
        scanf("%s", playersArray[playerIdx].playerName);

        // create cards array pointer
        struct Card * cardsArray = malloc(sizeof(struct Card) * START_CARDS_COUNT);
        // assign pointer for the player
        playersArray[playerIdx].cardsArray = cardsArray;
        // assign deck size and card count to keep track of changes to the deck.
        playersArray[playerIdx].cardDeckSize = START_CARDS_COUNT;
        playersArray[playerIdx].cardCount = START_CARDS_COUNT;
        
        // generate new cards for the player.
        for(int cardIdx=0; cardIdx < START_CARDS_COUNT; cardIdx++){
            // generate card and assign into array
            playersArray[playerIdx].cardsArray[cardIdx] = generateCard(ANY_CARD);

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

struct Card generateCard(bool numberOnly){
    struct Card newCard;
    int cardNumber;
    if(numberOnly){
        cardNumber = 1 + rand() % 9; // random number card (1-9).
    }
    else{
        cardNumber = 1 + rand() % 14; // random number card (1-9) or type (10-14)
    }
    
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

// adds card to cardArray of the player, resize's the deck if not enough space is 
// available
void appendCardToPlayer(struct Player * player, struct Card card){
    player->cardCount += 1; // Count the new card
    
    // increase deck size if necessary
    if(player->cardCount > player->cardDeckSize){
        // increase deck by DECK_RESIZE_VALUE to avoid reallocating memory too frequently.
        player->cardDeckSize += DECK_RESIZE_VALUE;
        // reallocate cards array to new larger dynamically allocated array.
        player->cardsArray = realloc(player->cardsArray ,sizeof(struct Card) * player->cardDeckSize);
        // check that the reallocation worked.
        checkValidAllocation(player);
    }

    // append new card to the cardsArray
    player->cardsArray[player->cardCount - 1] = card;

}

void removePlayerCard(struct Player * player, unsigned int cardIndex){
    for(int i=cardIndex; i < player->cardCount; i++){
        // set all cards ahead to go back by one index.
        player->cardsArray[cardIndex] = player->cardsArray[cardIndex + 1];
    }
    // after resize is complete, change cardsCount
    player->cardCount -= 1;
}

void printPlayersDeck(struct Player player){
    printf("\n%s's turn:\n", player.playerName);
    for(int i=0; i < player.cardCount; i++){
        printf("Card #%d:\n", i+1);
        printCard(player.cardsArray[i]);
        printf("\n");
    }
}

// simply checks if ptr is "NULL", exit(1) if yes
// nothing otherwise.
void checkValidAllocation(void * ptr){
    if(ptr == NULL){
        printf("Pointer is NULL, allocation may have failed.\n");
        exit(1);
    }
}

// Keep this function as last.
void freePlayersMemory(struct Player * playersArray, int playersCount){
    struct Card * cardPointer;
    for(int i=0; i<playersCount; i++){
        // free each card array from its pointer for each player
        free(playersArray[i].cardsArray);
        printf("Freed cards deck %d\n", i+1);
    }
    // free the players array.
    free(playersArray);
    printf("Freed players array\n");
}
// end of file
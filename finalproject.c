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
#define FORWARD 1
#define BACKWARD -1
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
void appendCardToPlayer(struct Player * playerPtr, struct Card card);
void removePlayerCard(struct Player * playerPtr, unsigned int cardIndex);
void printPlayersDeck(struct Player player);
int chooseCard(struct Player * playerPtr, struct Card upperCard);
bool validateCardChoice(struct Card chosenCard, struct Card upperCard);
void cardHandler(struct Player * playerPtr, int * gameDirection, int * playersTurnIndex, struct Card * upperCard, int chosenCardIndex);

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
    int gameDirection = FORWARD;
    int chosenCardIndex;

    printWelcome();
    playersCount = askPlayersCount();
    playersArray = queryPlayers(playersCount);


    struct Card randomCard;

    while(!gameFinished){
        // each iteration equals a turn.

        // (V) print upper card
        printf("\nUpper card:\n");
        printCard(upperCard);
        // (V) print the players whose turn it is his cards deck
        printPlayersDeck(playersArray[currentPlayerIndex]);
        // (V) ask the player whose turn it is to choose cards
        chosenCardIndex = chooseCard(&playersArray[currentPlayerIndex], upperCard);
        
        // (V) check validity of input (inside "chooseCard")

        // () Function to decide what to do with the card (Based on color, taki or basic).

        // () if color card, ask to assign color

        // () check validity of cards

        // () assign chosen card as upper card
        
        // () append card to statistics array
        printf("Continue?\n");
        getchar();
    }

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

// based on the count of players, generate a Player object
// for each player, for each Player object, generate a deck of cards
// with 4 random cards. 
struct Player * queryPlayers(unsigned int playersCount){
    struct Player * playersArray = malloc(sizeof(struct Player) * playersCount);
    checkValidAllocation(playersArray);

    for(int playerIdx=0; playerIdx < playersCount; playerIdx++){
        // ask, name, assign cards, and so on..
        printf("Please enter the first name of player #%d:\n", playerIdx+1);
        scanf("%s", playersArray[playerIdx].playerName);

        // create cards array pointer
        struct Card * cardsArray = malloc(sizeof(struct Card) * START_CARDS_COUNT);
        checkValidAllocation(cardsArray);

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

// prints the given "card" in a visual way.
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

// generate a random card using the "rand()" function.
// will generate a card from 1-9 or of special type
// will generate a color if the card is eligible for a color.
// returns in form of Card struct.
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
            newCard.cardColor = RED_CARD;
            break;
        case 1:
            newCard.cardColor = BLUE_CARD;
            break;
        case 2:
            newCard.cardColor = GREEN_CARD;
            break;
        case 3:
            newCard.cardColor = YELLO_CARD;
            break;
        default:
            break;
        }
    }

    return newCard;
}

// adds card to cardArray of the player, resize's the deck if not enough space is 
// available
void appendCardToPlayer(struct Player * playerPtr, struct Card card){
    playerPtr->cardCount += 1; // Count the new card
    
    // increase deck size if necessary
    if(playerPtr->cardCount > playerPtr->cardDeckSize){
        // increase deck by DECK_RESIZE_VALUE to avoid reallocating memory too frequently.
        playerPtr->cardDeckSize += DECK_RESIZE_VALUE;
        // reallocate cards array to new larger dynamically allocated array.
        playerPtr->cardsArray = realloc(playerPtr->cardsArray ,sizeof(struct Card) * playerPtr->cardDeckSize);
        // check that the reallocation worked.
        checkValidAllocation(playerPtr);
    }

    // append new card to the cardsArray
    playerPtr->cardsArray[playerPtr->cardCount - 1] = card;

}

// remove the card at the assigned index from the deck, by overwritting with the cards ahead.
void removePlayerCard(struct Player * playerPtr, unsigned int cardIndex){
    for(int i=cardIndex; i < playerPtr->cardCount; i++){
        // set all cards ahead to go back by one index.
        playerPtr->cardsArray[cardIndex] = playerPtr->cardsArray[cardIndex + 1];
    }
    // after resize is complete, change cardsCount
    playerPtr->cardCount -= 1;
}

// print the cards that the "player" has in his deck.
void printPlayersDeck(struct Player player){
    printf("\n%s's turn:\n", player.playerName);
    for(int i=0; i < player.cardCount; i++){
        printf("Card #%d:\n", i+1);
        printCard(player.cardsArray[i]);
        printf("\n");
    }
}

// given a player pointer (not an playerArray), and the upperCard
// ask the following player for a card to choose or to take a card from the deck.
// returns the index of the card the player chose, returns -1 if player decided to take a card
// from the deck.
int chooseCard(struct Player * playerPtr, struct Card upperCard){
    bool validCardChosen = false;
    int chosenCardIndex;
    struct Card newCard;

    // ask user for input, check for valid input.
    while(!validCardChosen){
        printf("Please enter 0 if you want to take a card from the deck\n");
        printf("or 1-%d if you want to put one of your cards in the middle:", playerPtr->cardCount);
        scanf("%d", &chosenCardIndex);
        
        if(chosenCardIndex >= 0 && chosenCardIndex <= playerPtr->cardCount ){
            // player decides to take a card from the deck
            if(chosenCardIndex == 0){
                // generate new card and append it.
                newCard = generateCard(ANY_CARD);
                appendCardToPlayer(playerPtr, newCard);
                return -1;
            }
            // player decides to place a card of choice.
            else{
                chosenCardIndex -= 1; // align input to be a valid index.
                // check if the card is a legal move. request input again for invalid move
                validCardChosen = validateCardChoice(playerPtr->cardsArray[chosenCardIndex], upperCard);
                if(validCardChosen == false){
                    printf("Invalid card! Try again.\n");
                }

            }
        }
        else{
            printf("Invalid choice! Try again.\n");
        }
    }

    // change to return cardIndex
    return chosenCardIndex;
}

// check if chosenCard is a valid move compared to the card in the middle.
// returns true if card is a valid move, returns false otherwise.
bool validateCardChoice(struct Card chosenCard, struct Card upperCard){
    // check if one of the three requirements are satisfied:

    // 1) Colorless change color card.
    if(chosenCard.cardNumber == CHANGE_COLOR_CARD){
        return true;
    }
    // 2) equal number card
    else if(chosenCard.cardNumber == upperCard.cardNumber){
        return true;
    }
    // 3) equal color card
    else if(chosenCard.cardColor == upperCard.cardColor){
        return true;
    }
    else{
        return false;
    }
    
}


void cardHandler(struct Player * playerPtr, int * gameDirection, int * playersTurnIndex, struct Card * upperCard, int chosenCardIndex){
    // 1) check card type,
    // if card is of basic value (1-9), assign upperCard as this card, and remove this card from player
    struct Card currentCard = playerPtr->cardsArray[chosenCardIndex];
    removePlayerCard(playerPtr, chosenCardIndex);
    // handle basic cards (1-9)
    if(currentCard.cardNumber > 1 && currentCard.cardNumber < 10){
        // simply assign this card as the upperCard and finish.
        return;
    }
    // handle Plus card
    else if(currentCard.cardNumber == PLUS_CARD){
        *playersTurnIndex -= 1; // next turn will assign to the same player.
    }
    // handle ChangeDirection card
    else if(currentCard.cardNumber == DIRECTION_CARD){
        *gameDirection *= -1; // change direction
        
    }
    // handle stop card
    else if(currentCard.cardNumber == STOP_CARD){
        *playersTurnIndex += 1; // next turn will skip one player.
    }
    else if(currentCard.cardNumber == CHANGE_COLOR_CARD){
        // prompt user to choose color:
        printf("Please enter your color choice:\n");
        printf("1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
        int colorChoice;
        
        // set this color
        // remove card from player.
    }


    *upperCard = currentCard; // set value of upperCard as value of currentCard.

}

// asks user for card color, checks validity of input
// returns char of chosen color
char queryCardColor(){   
    int inputChoice;
    char chosenColor;
    printf("Please enter your color choice:\n");
    printf("1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
    scanf("%d", &inputChoice);

    switch (inputChoice)
    {
    case 1:
        chosenColor = YELLO_CARD;
        break;
    case 2:
        chosenColor = RED_CARD;
        break;
    case 3:
        chosenColor = BLUE_CARD;
        break;
    case 4:
        chosenColor = GREEN_CARD;
        break;
    default:
        break;
    }

    return chosenColor;
    
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
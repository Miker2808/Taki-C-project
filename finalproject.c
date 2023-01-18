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
#define NUM_OF_CARD_TYPES 14

#define CARD_NAME_LENGTH 8
#define PLUS_CARD_TEXT "   +   "
#define STOP_CARD_TEXT "  STOP "
#define DIRECTION_CARD_TEXT "  <->  "
#define CHANGE_COLOR_TEXT " COLOR "
#define TAKI_CARD_TEXT "  TAKI "

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
struct Card cardHandler(struct Player * playerPtr, struct Card chosenCard ,
                int * gameDirectionPtr, int * playersTurnIndexPtr, const int playersCount);
char queryCardColor();
struct Card handleTakiCard(struct Player * playerPtr, struct Card upperCard);
//int mapPlayerTurnIndex(int playersTurnIndex, int playersCount);
int mapValueEdges(int value, int leftEdge, int rightEdge);
void resetArray(int array[], int size);
void numerateArray(int array[], int size);
void addToStatistics(struct Card card);
void printStatistics();
void sortStatisticsList();
void swapStats(int leftStat[2], int rightStat[2]);
void convertCardToText(int cardNumber, char outputString[]);

void checkValidAllocation(void * ptr);
void freePlayersMemory(struct Player * playersArray, int playersCount);
// End of declerations area

// initialize 2D array, first index will hold the card type, the second the count of usage.
int g_statsArray[NUM_OF_CARD_TYPES][2];

void main(){
    // use system time as the random seed, keep as first call on main.
    srand(time(NULL));
    // dynamic variables.
    bool gameFinished = false;
    int playersCount;
    struct Player * playersArray;
    struct Card upperCard = generateCard(NUMBER_CARD_ONLY);
    int currentPlayerIndex = 0;
    int gameDirection = FORWARD;
    int chosenCardIndex;
    
    // initialize g_statsArray
    numerateArray(g_statsArray[0], NUM_OF_CARD_TYPES);
    resetArray(g_statsArray[1], NUM_OF_CARD_TYPES);

    printWelcome();
    playersCount = askPlayersCount();
    playersArray = queryPlayers(playersCount);

    struct Card chosenCard;
    struct Player * currentPlayerPtr;

    while(!gameFinished){
        // each iteration equals a turn.

        // pointer for the player that has this turn.
        currentPlayerPtr = &playersArray[currentPlayerIndex];

        // Print the cards
        printf("\nUpper card:\n");
        printCard(upperCard);
        printPlayersDeck(*currentPlayerPtr);

        // ask the user for a card, until a valid choice was given. (-1 if no card was chosen)
        chosenCardIndex = chooseCard(currentPlayerPtr, upperCard);

        if(chosenCardIndex >= 0){

            chosenCard = currentPlayerPtr->cardsArray[chosenCardIndex];
            
            addToStatistics(chosenCard);
            removePlayerCard(currentPlayerPtr, chosenCardIndex);

            if(chosenCard.cardNumber == TAKI_CARD){
                // Enter taki waterfall loop.
                chosenCard = handleTakiCard(currentPlayerPtr, chosenCard);
            }
            // Then handle the chosen card, or the last card of the Taki waterfall.
            chosenCard = cardHandler(currentPlayerPtr, chosenCard, &gameDirection, &currentPlayerIndex, playersCount);
            upperCard = chosenCard;
            
        }
        
        if(playersArray[currentPlayerIndex].cardCount == 0){
            // game finished, player got rid of all cards. 
            // special cases are handled in cardHandler.
            gameFinished == true;
        }

        // assign index of next player based on game's direction.
        currentPlayerIndex += gameDirection;
        
        // correct index in case it got through the borders.
        //currentPlayerIndex = mapPlayerTurnIndex(currentPlayerIndex, playersCount);
        currentPlayerIndex = mapValueEdges(currentPlayerIndex, 0, playersCount);
    }

    printf("The winner is... %s! Congratulations!\n");
    
    printStatistics();

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

// Prints input Card in a visually apealing method.
void printCard(struct Card card){
    char cardName[8];

    printf("*********\n");
    printf("*       *\n");
    // print card type line based on switch.
    convertCardToText(card.cardNumber, cardName);
    printf("*%s*\n");
    // switch (card.cardNumber)
    // {
    // case PLUS_CARD:
    //     printf("*   +   *\n");
    //     break;
    // case STOP_CARD:
    //     printf("*  STOP *\n");
    //     break;
    // case DIRECTION_CARD:
    //     printf("*  <->  *\n");
    //     break;
    // case CHANGE_COLOR_CARD:
    //     printf("* COLOR *\n");
    //     break;
    // case TAKI_CARD:
    //     printf("*  TAKI *\n");
    //     break;

    // default:
    //     printf("*   %d   *\n", card.cardNumber);
    // }
    printf("*   %c   *\n", card.cardColor);
    printf("*       *\n");
    printf("*********\n");
}

// generate a random card using the "rand()" function.
// will generate a card from 1-9 or of special type
// will generate a color if the card is eligible for a color.
// will generate only number cards (excluding special cards) if "numberOnly" is true
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
    // make the cardCount smaller by 1
    playerPtr->cardCount -= 1;
    // iterate all cards but the last one (we removed it using above line)
    for(int i=cardIndex; i < playerPtr->cardCount; i++){
        // set all cards ahead to go back by one index.
        playerPtr->cardsArray[i] = playerPtr->cardsArray[i + 1];
    }
    
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
// returns the copy of the card the player chose, returns NULL if player decided to take a card
// from the deck.
int chooseCard(struct Player * playerPtr, struct Card upperCard){
    bool validCardChosen = false;
    int chosenCardIndex;

    // ask user for input, check for valid input.
    while(!validCardChosen){
        printf("Please enter 0 if you want to take a card from the deck\n");
        printf("or 1-%d if you want to put one of your cards in the middle:\n", playerPtr->cardCount);
        scanf("%d", &chosenCardIndex);
        
        // card chosen is in valid range of choices  
        if(chosenCardIndex >= 0 && chosenCardIndex <= playerPtr->cardCount ){
            // player decides to take a card from the deck
            if(chosenCardIndex == 0){
                // generate new card and append it.
                appendCardToPlayer(playerPtr, generateCard(ANY_CARD));
                return -1; // no card was chosen.
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

// the most important function!
// decides what to do based on card chosen!
struct Card cardHandler(struct Player * playerPtr, struct Card chosenCard ,int * gameDirectionPtr, int * playersTurnIndexPtr, const int playersCount){

    // handle special cards, normal cards are passed as is.
    switch (chosenCard.cardNumber)
    {
        case PLUS_CARD:
            *playersTurnIndexPtr -= 1; // next turn will assign to the same player.
            // automatically give card to player if this was his last card:
            if(playerPtr->cardCount == 0){
                appendCardToPlayer(playerPtr, generateCard(ANY_CARD));
            }
            break;
            
        case DIRECTION_CARD:
            *gameDirectionPtr *= -1; // change direction
            break;

        case STOP_CARD:
            *playersTurnIndexPtr += 1; // next turn will skip one player.
            
            // automaticall give card to player if this is his last card and playersCount
            // is two, or less.
            if(playerPtr->cardCount == 0 && playersCount == 2){
                appendCardToPlayer(playerPtr, generateCard(ANY_CARD));
            }
            break;
        case CHANGE_COLOR_CARD:
            // prompt user to choose color:
            // and assign the color to the upper deck card
            chosenCard.cardColor = queryCardColor();
            break;
    }

    return chosenCard;

}

// asks user for card color, checks validity of input
// returns char of chosen color
char queryCardColor(){   
    int inputChoice = -1;
    char chosenColor;
    while(true){
        printf("Please enter your color choice:\n");
        printf("1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
        scanf("%d", &inputChoice);
        if(inputChoice >= 1 && inputChoice <= 4){
            break;
        }
        else{
            printf("Invalid choice! Try again.\n");
        }
    }

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

// handle a taki card
// place all valid cards for the taki, and return the last card placed.
struct Card handleTakiCard(struct Player * playerPtr, struct Card upperCard){
    bool endTurn = false;
    bool validCardChosen;
    int playerInput;
    int chosenCardIndex;
    char takiCardColor = upperCard.cardColor;
    struct Card chosenCard;
    
    // iterate until player chooses to stop, or he has no more cards
    while(!endTurn){
        
        validCardChosen = false; // reset flag for new choice.
        // print deck info.
        printf("\nUpper card:\n");
        printCard(upperCard);
        printPlayersDeck(*playerPtr);
        
        while(!validCardChosen){
            printf("Please enter 0 if you want to finish your turn\n");
            printf("or 1-%d if you want to put one of your cards in the middle:\n", playerPtr->cardCount);
            scanf("%d", &playerInput);
            // card chosen is in valid range of choices  
            if(playerInput >= 0 && playerInput <= playerPtr->cardCount ){
                // player decides to end the turn
                if(playerInput == 0){
                    return upperCard;
                }
                // player decides to place a card of choice.
                else{
                    chosenCardIndex = playerInput - 1;
                    chosenCard = playerPtr->cardsArray[chosenCardIndex];

                    // If "Change color" card was chosen end the loop.
                    if(chosenCard.cardNumber == CHANGE_COLOR_CARD){
                        endTurn = true;
                    }
                    // if not a "Change color" card, check if the color is the same as the Taki Card
                    // refuse the choice if the color is different.
                    else if(chosenCard.cardColor != takiCardColor){
                        printf("Invalid card! Try again.\n");
                        continue;
                    }
                    
                    // set upperCard as the chosen taki card,
                    upperCard = chosenCard;
                    // remove card from player
                    removePlayerCard(playerPtr, chosenCardIndex);
                    // input is valid, continue to the next input request
                    validCardChosen = true;
                    // add the card to the statistics
                    addToStatistics(upperCard);
                }
            }
            else{
                printf("Invalid choice! Try again.\n");
            }
        }
    }

    return upperCard;

}

// int mapPlayerTurnIndex(int playersTurnIndex, int playersCount){
//     // checks current index against number of players
//     // resets value if index passed a border.
//     if(playersTurnIndex < 0){
//         playersTurnIndex = playersCount + playersTurnIndex;
//     }
//     else if(playersTurnIndex >= playersCount){
//         playersTurnIndex %= playersCount;
//     }
//     return playersTurnIndex;
// }


// checks current value, and maps the values like the edges are connected
// (like a mobius strip, but one dimensional)
// leftEdge is included in valid range and rightEdge is discluded from valid range
// Example: for mapValueEdges(9, 0, 9), the output will be 0, as 9 is past the valid range.
int mapValueEdges(int value, int leftEdge, int rightEdge){
    if(value < leftEdge){
        value += rightEdge;
    }
    else if(value >= rightEdge){
        value %= rightEdge;
    }
    return value;
}

// given array, and its size
// function will reset all its elements to 0
void resetArray(int array[], int size){
    for(int i=0; i<size; i++){
        array[i] = 0;
    }
}

// given array and its size.
// the function will numerate the array from 1 to the value of size
void numerateArray(int array[], int size){
    for(int i=0; i<size; i++){
        array[i] = i+1;
    }
}

// Receives a card struct
// increments the count of the card to the relevant index
void addToStatistics(struct Card card){
    g_statsArray[card.cardNumber - 1][1] += 1;
}

// Sorts the statistics array, and prints the values
// from the most frequent to least frequent card.
void printStatistics(){
    char cardName[CARD_NAME_LENGTH];
    int cardCount;

    sortStatisticsList();
    printf("************ Game Statistics ************\n");
    printf("Card # | Frequency\n");
    printf("__________________\n");

    char cardName[CARD_NAME_LENGTH];
    int cardCount;

    for(int i=0; i < NUM_OF_CARD_TYPES; i++){
        convertCardToText(g_statsArray[i][0], cardName);
        cardCount = g_statsArray[i][1];
        printf("%s|    %d\n", cardName, cardCount);
    }
    
}

// given a cardNumber, writes a visually appealing text into "outputString"
void convertCardToText(int cardNumber, char outputString[]){
    char blankString[] = "       ";

    if(cardNumber < 0 || cardNumber > TAKI_CARD){
        return;
    }
    
    switch (cardNumber)
    {
    case PLUS_CARD:
        strcpy(outputString, PLUS_CARD_TEXT);
        break;
    case CHANGE_COLOR_CARD:
        strcpy(outputString, CHANGE_COLOR_TEXT);
        break;
    case STOP_CARD:
        strcpy(outputString, STOP_CARD_TEXT);
        break;
    case DIRECTION_CARD:
        strcpy(outputString, DIRECTION_CARD_TEXT);
        break;
    case TAKI_CARD:
        strcpy(outputString, TAKI_CARD_TEXT);
    
    default:
        // assign number to center of string
        blankString[3] = cardNumber + INT_TO_ASCII;
        strcpy(outputString, blankString);
    }
}

// sorts the global stats array using insertion sort
void sortStatisticsList(){
    // sort the list using insertion sort method

    //using a 'for' loop, traverse each element
    for(int index = 1; index < NUM_OF_CARD_TYPES; index++){
        int leftItemIndex = index - 1;
        // check if number to the left is larger, if yes, swap places and check again until leftItemIndex reaches beginning of array;
        while(leftItemIndex >= 0 && g_statsArray[leftItemIndex][1] > g_statsArray[leftItemIndex + 1][1] ){
            // do a swap
            swapStats(g_statsArray[leftItemIndex], g_statsArray[leftItemIndex + 1]);
            leftItemIndex -= 1;
        }
    }

}

// swap stats entries while keeping track of card type.
// stats are array of size 2, one for the card type, and one for the counting of the card
// rightStat is a 1D array of size2, same as leftStat
void swapStats(int leftStat[2], int rightStat[2]){
    int temp[2] = leftStat;
    leftStat = rightStat;
    rightStat = temp;
}

// Given a pointer,
// function simply checks if ptr is "NULL", exit(1) if yes
// nothing otherwise.
void checkValidAllocation(void * ptr){
    if(ptr == NULL){
        printf("Pointer is NULL, allocation may have failed.\n");
        exit(1);
    }
}

// Given playersArray and count of players,
// free all dynamically allocated arrays of each player, 
// and in the end input array
void freePlayersMemory(struct Player * playersArray, int playersCount){
    struct Card * cardPointer;
    for(int i=0; i<playersCount; i++){
        // free each card array from its pointer for each player
        free(playersArray[i].cardsArray);
    }
    // free the players array.
    free(playersArray);
}
// end of file
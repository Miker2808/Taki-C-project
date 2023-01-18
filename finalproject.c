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
void cardHandler(struct Player * playerPtr, int * gameDirection, int * playersTurnIndex, 
                struct Card * upperCard, int chosenCardIndex, const int playersCount);
char queryCardColor();
int handleTakiCard(struct Player * playerPtr, struct Card * upperCard);
int mapPlayerTurnIndex(int playersTurnIndex, int playersCount);
int mapValueEdges(int value, int leftEdge, int rightEdge);
void initializeArray(int array[], int size);
void addToStatistics(int statsArray[], struct Card card);
// do not put functions here! (Note for me, mike)
void checkValidAllocation(void * ptr);
void freePlayersMemory(struct Player * playersArray, int playersCount);
// End of declerations area

int globalStatsArray[NUM_OF_CARD_TYPES];

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
    initializeArray(globalStatsArray, NUM_OF_CARD_TYPES);

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

        if(chosenCardIndex >= 0){
            cardHandler(&playersArray[currentPlayerIndex], &gameDirection, 
                        &currentPlayerIndex, &upperCard, chosenCardIndex, playersCount);
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
    // record this card to statistics:
    // The logic: cards do not just disappear, if I need to remove a card from player, he most
    // definitly placed the card in the upperDeck. recording the stats here is the best solution
    // as it doesn't require to check where each card was drawn.
    addToStatistics(globalStatsArray, playerPtr->cardsArray[cardIndex]);
    
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
void cardHandler(struct Player * playerPtr, int * gameDirectionPtr, int * playersTurnIndexPtr, struct Card * upperCardPtr, int chosenCardIndex, const int playersCount){
    // 1) check card type,
    // if card is of basic value (1-9), assign upperCard as this card, and remove this card from player
    struct Card currentCard = playerPtr->cardsArray[chosenCardIndex];
    removePlayerCard(playerPtr, chosenCardIndex);
    
    // handle special cards, normal cards are passed as is.
    switch (currentCard.cardNumber)
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
            currentCard.cardColor = queryCardColor();
            break;
        case TAKI_CARD:
            // begin a new loop to drop all cards based on the Taki card rules
            // rules are different from the basic card choices, therefore I decided to initiate a special loop
            // for this case.
            chosenCardIndex = handleTakiCard(playerPtr, upperCardPtr);
            currentCard = playerPtr->cardsArray[chosenCardIndex]; // update current card as Taki may have changed it
            // Handle the upperCard after player finished his Taki loop.
            cardHandler(playerPtr, gameDirectionPtr, playersTurnIndexPtr, upperCardPtr, chosenCardIndex, playersCount);
            break;
    }

    *upperCardPtr = currentCard; // set value of upperCard as value of currentCard.

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
int handleTakiCard(struct Player * playerPtr, struct Card * upperCardPtr){
    bool endTurn = false;
    bool validCardChosen;
    int chosenCardIndex;
    char takiCardColor = upperCardPtr->cardColor;
    struct Card chosenCard;
        
    while(!endTurn || playerPtr->cardCount == 0){
        // print the deck of the player and ask to choose a card to place.
        validCardChosen = false; // reset flag for new choice.

        // print deck info.
        printf("\nUpper card:\n");
        printCard(*upperCardPtr);
        printPlayersDeck(*playerPtr);
        
        while(!validCardChosen){
            printf("Please enter 0 if you want to finish your turn\n");
            printf("or 1-%d if you want to put one of your cards in the middle:\n", playerPtr->cardCount);
            scanf("%d", &chosenCardIndex);
            // card chosen is in valid range of choices  
            if(chosenCardIndex >= 0 && chosenCardIndex <= playerPtr->cardCount ){
                // player decides to end the turn
                if(chosenCardIndex == 0){
                    endTurn == true;
                    break;
                }
                // player decides to place a card of choice.
                else{
                    chosenCardIndex -= 1; // align input to be a valid index.
                    chosenCard = playerPtr->cardsArray[chosenCardIndex];

                    // If "Change color" card was chosen, ask for the color and end the taki loop.
                    if(chosenCard.cardNumber == CHANGE_COLOR_CARD){
                        chosenCard.cardColor = queryCardColor();
                        endTurn = true;
                    }

                    // if not a "Change color" card, check if the color is the same as the Taki Card
                    // refuse the choice if the color is different.
                    else if(chosenCard.cardColor != takiCardColor){
                        printf("Invalid card! Try again.\n");
                        continue;
                    }
                    
                    // set upperCard as the chosen taki card,
                    *upperCardPtr = playerPtr->cardsArray[chosenCardIndex];
                    // remove card from player
                    removePlayerCard(playerPtr, chosenCardIndex);
                    // valid card input, continue to next input.
                    validCardChosen = true;
                }
            }
            else{
                printf("Invalid choice! Try again.\n");
            }
        }
    }

    return chosenCardIndex;

}

int mapPlayerTurnIndex(int playersTurnIndex, int playersCount){
    // checks current index against number of players
    // resets value if index passed a border.
    if(playersTurnIndex < 0){
        playersTurnIndex = playersCount + playersTurnIndex;
    }
    else if(playersTurnIndex >= playersCount){
        playersTurnIndex %= playersCount;
    }
    return playersTurnIndex;
}


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

void initializeArray(int array[], int size){
    for(int i=0; i<size; i++){
        array[i] = 0;
    }
}

void addToStatistics(int statsArray[], struct Card card){
    statsArray[card.cardNumber - 1] += 1;
}

void sortArray(){

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
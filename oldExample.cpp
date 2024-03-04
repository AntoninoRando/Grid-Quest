#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <random>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define ENTER '\r'

using namespace std;

enum class Suit
{
    HEARTS,
    DIAMONS,
    CLUBS,
    SPADES
};

enum class CardName
{
    ACE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    JACK = 11,
    QUEEN = 12,
    KING = 13
};

struct Card
{
    Suit suit;
    CardName name;

    string ToString()
    {
        string suit_repr;
        switch (suit)
        {
        case Suit::HEARTS:
            suit_repr = "\u001b[1;31m(3\u001b[0m";
            break;
        case Suit::DIAMONS:
            suit_repr = "\u001b[1;31m<>\u001b[0m";
            break;
        case Suit::CLUBS:
            suit_repr = "\u001b[1;34m%'\u001b[0m";
            break;
        case Suit::SPADES:
            suit_repr = "\u001b[1;34m|>\u001b[0m";
            break;
        }
        string name_repr;
        switch (name)
        {
        case CardName::ACE:
            name_repr = "A";
            break;
        case CardName::JACK:
            name_repr = "J";
            break;
        case CardName::QUEEN:
            name_repr = "Q";
            break;
        case CardName::KING:
            name_repr = "K";
            break;
        default:
            name_repr = to_string((int)name);
            break;
        }

        return name_repr + suit_repr;
    }
};

struct Deck
{
    Card cards[52];

    void createCards()
    {
        for (int s = (int)Suit::HEARTS; s <= (int)Suit::SPADES; s++)
        {
            for (int i = (int)CardName::ACE; i <= (int)CardName::KING; i++)
            {
                Card card;
                card.suit = (Suit)s;
                card.name = (CardName)i;
                cards[(13 * s) + (i - 1)] = card;
            };
        };
    }

    void randomize()
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(std::begin(cards), std::end(cards), g);
    }
};

struct Hand
{
    Card cards[13];

    void printHand()
    {
        int i = 1;
        for (Card card : cards)
        {
            cout << "(" << i << ")" << card.ToString() << "  ";
            i++;
        }
        cout << endl;
    }
};

void printBinding(int x = -1, int y = -1, int locked = -1)
{
    system("CLS");
    cout << "---------------------------" << endl;
    cout << "     MAKE YOUR BINDING!" << endl;
    cout << "---------------------------" << endl;

    string suits[4] = {"\u001b[1;31m(3\u001b[0m",
                       "\u001b[1;31m<>\u001b[0m",
                       "\u001b[1;34m%'\u001b[0m",
                       "\u001b[1;34m|>\u001b[0m"};

    for (int i = 0; i < 7; i++)
    {
        for (int s = 0; s < 4; s++)
        {
            if ((i == y && s == x))
            {
                cout << "\u001b[40;1m";
            }
            else if ((4 * i + s) <= locked)
            {
                cout << "\u001b[47m";
            }

            cout << (i + 1) << suits[s] << "\t";
        }
        cout << endl
             << flush;
    }
}

void gameLoop(Hand p1, Hand p2, Hand p3, Hand p4)
{
    while (true)
    {
        p1.printHand();
        int x;
        cout << "Gioca una carta";
        cin >> x;
    }
}

int main()
{
    // Serve per visualizzare i caratteri speciali su vs code.
    system("chcp 65001");
    Deck deck;
    deck.createCards();
    deck.randomize();

    Hand p1;
    Hand p2;
    Hand p3;
    Hand p4;
    
    for (int i = 0; i < 13; i++)
    {
        p1.cards[i] = deck.cards[i];
        p2.cards[i] = deck.cards[i + 13];
        p3.cards[i] = deck.cards[i + 26];
        p4.cards[i] = deck.cards[i + 29];
    }

    int x = 0;
    int y = 0;
    int locked = -1;

    bool confirmed = false;
    while (!confirmed)
    {
        printBinding(x, y, locked);

        switch (getch())
        {
        case KEY_UP:
        {
            int y2 = (y > 0) ? y - 1 : 6;
            if ((4 * y2 + x) > locked)
                y = y2;
            break;
        }
        case KEY_DOWN:
        {
            int y2 = (y < 7) ? y + 1 : max(0, (int)(locked / 4));
            if ((4 * y2 + x) > locked)
                y = y2;
            break;
        }
        case KEY_LEFT:
        {
            int x2 = (x > 0) ? x - 1 : 3;
            if ((4 * y + x2) > locked)
                x = x2;
            break;
        }
        case KEY_RIGHT:
        {
            int x2 = (x < 4) ? x + 1 : max(0, (int)(locked / 4));
            if ((4 * y + x2) > locked)
                x = x2;
            break;
        }
        case ENTER:
            locked = 4 * y + x;
            confirmed = true;
            break;
        default:
            break;
        }
    }

    gameLoop(p1, p2, p3, p4);
}
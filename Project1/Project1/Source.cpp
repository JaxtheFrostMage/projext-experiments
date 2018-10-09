#include <time.h>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
# include <windows.h>
using namespace std;


const string Suits= "CDHS", Ranks = "A23456789TJQK";
const int handCards = 9, drawCards = 3;



class card {
	
	
public:
	friend ostream& operator<< (ostream& os, const card& c) {
		os << Ranks[c.rank] << Suits[c.suit];
		return os;
	}
	bool isValid() { return rank > -1; }
	void set(char Suits, char Ranks) { suit = Suits; rank = Ranks; }
	char getRank() { return Ranks[rank]; }
	bool operator == (const char o) { return Ranks[rank] == o; }
	bool operator < (const card& a) { if (rank == a.rank) return suit < a.suit; return rank < a.rank; }
private:
	char                                 suit, rank;
};
class deck {
public:
	static deck* instance() {
		if (!inst) inst = new deck();
		return inst;
	}
	void destroy() {
		delete inst;
		inst = 0;
	}
	card draw() {
		card c;
		if (cards.size() > 0) {
			c = cards.back();
			cards.pop_back();
			return c;
		}
		c.set(-1, -1);
		return c;
	}
private:
	deck() {
		newDeck();
	}
	void newDeck() {
		card c;
		for (char Suits = 0; Suits < 4; Suits++) {
			for (char Ranks = 0; Ranks < 13; Ranks++) {
				c.set(Suits, Ranks);
				cards.push_back(c);
			}
		}
		random_shuffle(cards.begin(), cards.end());
		random_shuffle(cards.begin(), cards.end());
	}
	static deck* inst;
	vector<card> cards;
};
class player {
public:
	player(string n) : nm(n) {
		for (int x = 0; x < handCards; x++)
			hand.push_back(deck::instance()->draw());
		sort(hand.begin(), hand.end());
	}
	void outputHand() {
		for (vector<card>::iterator x = hand.begin(); x != hand.end(); x++)
			cout << (*x) << " ";
		cout << "\n";
	}
	bool addCard(card c) {
		hand.push_back(c);
		return checkForBook();
	}
	string name() {
		return nm;
	}
	bool holds(char c) {
		return(hand.end() != find(hand.begin(), hand.end(), c));
	}
	card takeCard(char c) {
		vector<card>::iterator it = find(hand.begin(), hand.end(), c);
		swap((*it), hand.back());
		card d = hand.back();
		hand.pop_back();
		hasCards();
		sort(hand.begin(), hand.end());
		return d;
	}
	size_t getBooksCount() {
		return books.size();
	}
	void listBooks() {
		for (vector<char>::iterator it = books.begin(); it != books.end(); it++)
			cout << (*it) << "'s ";
		cout << "\n";
	}
	bool checkForBook() {
		bool ret = false;
		map<char, int> countMap;
		for (vector<card>::iterator it = hand.begin(); it != hand.end(); it++)
			countMap[(*it).getRank()]++;
		for (map<char, int>::iterator it = countMap.begin(); it != countMap.end(); it++) {
			if ((*it).second == 4) {
				do {
					takeCard((*it).first);
				} while (holds((*it).first));
				books.push_back((*it).first);
				(*it).second = 0;
				ret = true;
			}
		}
		sort(hand.begin(), hand.end());
		return ret;
	}
	bool hasCards() {
		if (hand.size() < 1) {
			card c;
			for (int x = 0; x < drawCards; x++) {
				c = deck::instance()->draw();
				if (c.isValid()) addCard(c);
				else break;
			}
		}
		return(hand.size() > 0);
	}
protected:
	string nm;
	vector<card> hand;
	vector<char> books;
};
class aiPlayer : public player {
public:
	aiPlayer(string n) : player(n), askedIdx(-1), lastAsked(0), nextToAsk(-1) { }
	void rememberCard(char c) {
		if (asked.end() != find(asked.begin(), asked.end(), c) || !asked.size())
			asked.push_back(c);
	}
	char makeMove() {
		if (askedIdx < 0 || askedIdx >= static_cast<int>(hand.size())) {
			askedIdx = rand() % static_cast<int>(hand.size());
		}

		char c;
		if (nextToAsk > -1) {
			c = nextToAsk;
			nextToAsk = -1;
		}
		else {
			while (hand[askedIdx].getRank() == lastAsked) {
				if (++askedIdx == hand.size()) {
					askedIdx = 0;
					break;
				}
			}
			c = hand[askedIdx].getRank();
			if (rand() % 100 > 25 && asked.size()) {
				for (vector<char>::iterator it = asked.begin(); it != asked.end(); it++) {
					if (holds(*it)) {
						c = (*it);
						break;
					}
				}
			}
		}
		lastAsked = c;
		return c;
	}
	void clearMemory(char c) {
		vector<char>::iterator it = find(asked.begin(), asked.end(), c);
		if (asked.end() != it) {
			swap((*it), asked.back());
			asked.pop_back();
		}
	}
	bool addCard(card c) {
		if (!holds(c.getRank()))
			nextToAsk = c.getRank();
		return player::addCard(c);
	}
private:
	vector<char> asked;
	char nextToAsk, lastAsked;
	int askedIdx;
};
class goFish {
public:
	goFish() {
		plr = true;
		string n;
		cout << "Hi there, enter your name: "; cin >> n;
		p1 = new player(n);
		p2 = new aiPlayer("JJ");
	}
	~goFish() {
		if (p1) delete p1;
		if (p2) delete p2;
		deck::instance()->destroy();
	}
	void play() {
		while (true) {
			if (process(getInput())) break;
		}
		cout << "\n\n";
		showBooks();
		if (p1->getBooksCount() > p2->getBooksCount()) {
			cout << "\n\n\t*** !!! CONGRATULATIONS !!! ***\n\n\n";
		}
		else {
			cout << "\n\n\t*** !!! YOU LOSE - HA HA HA !!! ***\n\n\n";
		}
	}
private:
	void showBooks() {
		if (p1->getBooksCount() > 0) {
			cout << "\nYour Book(s): ";
			p1->listBooks();
		}
		if (p2->getBooksCount() > 0) {
			cout << "\nMy Book(s): ";
			p2->listBooks();
		}
	}
	void showPlayerCards() {
		cout << "\n\n" << p1->name() << ", these are your cards:\n";
		p1->outputHand();
		showBooks();
	}
	char getInput() {
		char c;
		if (plr) {
			if (!p1->hasCards()) return -1;
			showPlayerCards();
			string w;
			while (true) {
				cout << "\nWhat card(rank) do you want? "; cin >> w;
				c = toupper(w[0]);
				if (p1->holds(c)) break;
				cout << p1->name() << ", you can't ask for a card you don't have!\n\n";
			}
		}
		else {
			if (!p2->hasCards()) return -1;
			c = p2->makeMove();
			showPlayerCards();
			string r;
			cout << "\nDo you have any " << c << "'s? (Y)es / (G)o Fish ";
			do {
				getline(cin, r);
				r = toupper(r[0]);
			} while (r[0] != 'Y' && r[0] != 'G');
			bool hasIt = p1->holds(c);
			if (hasIt && r[0] == 'G')
				cout << "Are you trying to cheat me?! I know you do...\n";
			if (!hasIt && r[0] == 'Y')
				cout << "Nooooo, you don't have it!!!\n";
		}
		return c;
	}
	bool process(char c) {
		if (c < 0) return true;
		if (plr) p2->rememberCard(c);

		player *a, *b;
		a = plr ? p2 : p1;
		b = plr ? p1 : p2;
		bool r;
		if (a->holds(c)) {
			while (a->holds(c)) {
				r = b->addCard(a->takeCard(c));
			}
			if (plr && r)p2->clearMemory(c);
		}
		else {
			fish();
			plr = !plr;
		}
		return false;
	}
	void fish() {
		cout << "\n\n\t  *** GO FISH! ***\n\n";
		card c = deck::instance()->draw();
		if (plr) {
			cout << "Your new card: " << c << ".\n\n******** Your turn is over! ********\n" << string(36, '-') << "\n\n";
			if (p1->addCard(c)) p2->clearMemory(c.getRank());
		}
		else {
			cout << "\n********* My turn is over! *********\n" << string(36, '-') << "\n\n";
			p2->addCard(c);
		}
	}

	player        *p1;
	aiPlayer    *p2;
	bool        plr;
};
deck* deck::inst = 0;
int main(int argc, char* argv[]) {
	srand(static_cast<unsigned>(time(NULL)));
	goFish f;  f.play();
	//system("pause");
	return 0;
}
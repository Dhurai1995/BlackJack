#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<ctime>

class Card
{
public:
	enum rank {ACE =1,TWO,THREE, FOUR,FIVE,SIX,SEVEN,EIGHT,NINE,TEN,JACK,QUEEN,KING};
	enum suitEnum {CLUBS,DIAMONDS,HEARTS,SPADES};
	friend std::ostream& operator<<(std::ostream& os, const Card& aCard);

	Card(rank r = ACE, suitEnum s = SPADES, bool ifu = true);

	int GetValue() const;
	void Flip();

private:
	rank mRank;
    suitEnum mSuit;
	bool mIsFaceUp;
};

Card::Card(rank r, suitEnum s, bool ifu) : mRank(r), mSuit(s), mIsFaceUp(ifu)
{}

int Card::GetValue() const
{
	int value = 0;
	if (mIsFaceUp)
	{
		value = mRank;
		if (value > 10)
		{
			value = 10;
		}
	}
	return value;
}

void Card::Flip()
{
	mIsFaceUp = !(mIsFaceUp);
}

// Hand Clas

class Hand
{
public:
	Hand();
	virtual ~Hand();
	void Add(Card* pCard);
	void Clear();
	int GetTotal() const;

protected:
	std::vector<Card*> mCard;
};

Hand::Hand()
{
	mCard.reserve(7);
}

Hand::~Hand()
{
	Clear();
}

void Hand::Add(Card* pCard)
{
	mCard.push_back(pCard);
}

void Hand::Clear()
{
	std::vector<Card*>::iterator iter = mCard.begin();
	for (iter = mCard.begin(); iter != mCard.end(); iter++)
	{
		delete* iter;
		*iter = 0;
	}
	mCard.clear();
}

int Hand::GetTotal() const
{
	if (mCard.empty())
	{
		return 0;
	}

	if (mCard[0]->GetValue() == 0)
	{
		return 0;
	}

	int total = 0;
	std::vector<Card*>::const_iterator iter;
	for (iter = mCard.begin(); iter != mCard.end(); iter++)
	{
		total += (*iter)->GetValue();
	}

	bool containsAce = false;
	for (iter = mCard.begin(); iter != mCard.end(); iter++)
	{
		if ((*iter)->GetValue() == Card::ACE)
		{
			containsAce = true;
		}
	}
	if (containsAce && total <= 11)
	{
		total += 10;
	}
	return total;
}

// Generic class

class GenericPlayer : public Hand
{
public:
	GenericPlayer(const std::string& name = "");
	virtual ~GenericPlayer();
	virtual bool isHitting() const = 0;
	bool isBusted() const;
	void bust() const;

protected:
	std::string mName;
};

GenericPlayer::GenericPlayer(const std::string& name) :mName(name)
{}

GenericPlayer::~GenericPlayer()
{}

bool GenericPlayer::isBusted() const
{
	return (GetTotal() > 21);
}

void GenericPlayer::bust() const
{
	std::cout << mName << " Busted \n";
}

// Player class

class Player :public GenericPlayer
{
public:
	Player(const std::string& name = "");
	virtual ~Player();
	virtual bool IsHitting() const;
	void Win() const;
	void Lose() const;
	void Push() const;

};

Player::Player(const std::string& name) :GenericPlayer(name)
{}

Player::~Player()
{}

bool Player::IsHitting() const
{
	std::cout << "Do you want to hit? (Y/N)" << std::endl;
	char response;
	std::cin >> response;
	return(response == 'y' || response == 'Y');
}

void Player::Win() const
{
	std::cout << mName << " Wins \n";
}

void Player::Lose() const
{
	std::cout << mName << " Loses \n";
}

void Player::Push() const
{
	std::cout << mName << " Pushes \n";
}

// House Class

class House : public GenericPlayer
{
public:
	House(const std::string& name = "");
	virtual ~House();
	virtual bool IsHitting() const;
	void FlipFirstCard();
};

House::House(const std::string& name) :GenericPlayer(name)
{}

House::~House()
{}

bool House::IsHitting() const
{
	return (GetTotal() < 16);
}

void House::FlipFirstCard()
{
	if (!(mCard.empty()))
	{
		mCard[0]->Flip();
	}
	else
	{
		std::cout << "No card to flip\n";
	}
}

// Deck Class

class Deck :public Hand
{
public:
	Deck();
	virtual ~Deck();
	void Populate();
	void Shuffle();
	void Deal(Hand& aHand);
	void AdditionalCards(GenericPlayer& aGenericPlayer);
};

Deck::Deck()
{
	
	mCard.reserve(52);
	Populate();
}

Deck::~Deck()
{}

void Deck::Populate()
{
	Clear();

	for (int s = Card::CLUBS; s <= Card::SPADES; ++s)
	{
		for (int r = Card::ACE; r <= Card::KING; ++r)
		{
			Add(new Card(static_cast<Card::rank>(r), static_cast<Card::suitEnum>(s)));
		}
	}
}

void Deck::Shuffle()
{
	random_shuffle(mCard.begin(), mCard.end());
}

void Deck::Deal(Hand& aHand)
{
	if (!mCard.empty())
	{
		aHand.Add(mCard.back());
		mCard.pop_back();
	}
	else
	{
		std::cout << "Out of cards. Unable to deal" << std::endl;
	}
}

void Deck::AdditionalCards(GenericPlayer& aGenericPlayer)
{
	std::cout << std::endl;
	while (!(aGenericPlayer.isBusted()) && aGenericPlayer.isHitting())
	{
		Deal(aGenericPlayer);
		std::cout << aGenericPlayer << std::endl;
		if (aGenericPlayer.isBusted())
		{
			aGenericPlayer.bust();
		}
	}
}

// Game class

class Game
{
public:
	Game(const std::vector<std::string>& name);
	~Game();
	void Play();

private:
	Deck mDeck;
	House mHouse;
	std::vector<Player> mPlayer;
};

Game::Game(const std::vector<std::string>& name)
{
	std::vector<std::string>::const_iterator pName;
	for (pName = name.begin(); pName != name.end(); pName++)
	{
		mPlayer.push_back(Player(*pName));
	}

	srand(static_cast<unsigned int>(time(0)));
	mDeck.Populate();
	mDeck.Shuffle();
}

Game::~Game()
{}

void Game::Play()
{
	std::vector<Player>::iterator pPlayer;
	for (int i = 0; i < 2; i++)
	{
		for (pPlayer = mPlayer.begin(); pPlayer != mPlayer.end(); pPlayer++)
		{
			mDeck.Deal(*pPlayer);
		}
		mDeck.Deal(mHouse);
	}

	mHouse.FlipFirstCard();

	for (pPlayer = mPlayer.begin(); pPlayer != mPlayer.end(); pPlayer++)
	{
		std::cout << *pPlayer << std::endl;
	}

	std::cout << mHouse << std::endl;

	for (pPlayer = mPlayer.begin(); pPlayer != mPlayer.end(); pPlayer++)
	{
		mDeck.AdditionalCards(*pPlayer);
	}

	mHouse.FlipFirstCard();
	std::cout << std::endl << mHouse;

	mDeck.AdditionalCards(mHouse);

	if (mHouse.isBusted())
	{
		for (pPlayer = mPlayer.begin(); pPlayer != mPlayer.end(); ++pPlayer)
		{
			if (!(pPlayer->isBusted()))
			{
				pPlayer->Win()
			}
		}
	}
	else
	{
		for (pPlayer = mPlayer.begin(); pPlayer != mPlayer.end(); pPlayer++)
		{
			if (!(pPlayer->isBusted()))
			{
				if (pPlayer->GetTotal() > mHouse.GetTotal())
				{
					pPlayer->Win();
				}
				else if (pPlayer->GetTotal() < mHouse.GetTotal())
				{
					pPlayer->Lose();
				}
				else
				{
					pPlayer->Push();
				}
			}
		}
	}


	for (pPlayer = mPlayer.begin(); pPlayer != mPlayer.end(); pPlayer++)
	{
		pPlayer->Clear();
	}
	mHouse.Clear();
}

// Main function 
//function prototypes
ostream& operator<<(ostream& os, const Card& aCard);
ostream& operator<<(ostream& os, const GenericPlayer& aGenericPlayer);

int main()
{
	std::cout << "\t\t Welcome to the BlackJack!!\n\n";
	int NumPlayer = 0;
	while (NumPlayer < 1 || NumPlayer>7)
	{
		std::cout << "How many player?(1-7):";
		std::cin >> NumPlayer;
	}

	std::vector<std::string> names;
	std::string name;

	for (int i = 0; i < NumPlayer; i++)
	{
		std::cout << "ENter the player name\n";
		std::cin >> name;
		names.push_back(name);
	}

	std::cout<<std::endl;

	Game aGame(names);

	char again = 'Y';
	while (again != 'n' && again != 'N')
	{
		aGame.Play();
		std::cout << "\n Do you want to play again? (Y/N)";
		std::cin >> again;

	}

	return 0;
}

std::ostream& operator<<(std::ostream& os, const Card& aCard)
{
	const std::string RANKS[] = { "0","A","2","3","4","5","6","7","8","9","10","J","Q","K" };
	const std::string SUITS[] = { "c","d","h","s" };

	if (aCard.mIsFaceUp)
	{
		os << RANKS[aCard.mRank] << SUITS[aCard.mSuit];
	}
	else
	{
		os << "XX";
	}
	return os;
}


std::ostream& operator<<(std::ostream& os, const GenericPlayer& aGenericPlayer)
{
	os << aGenericPlayer.mName << ":\t";
	std::vector<Card*>::const_iterator pCard;
	if (!aGenericPlayer.mCard.empty())
	{
		for (pCard = aGenericPlayer.mCard.begin(); pCard != aGenericPlayer.mCard.end(); ++pCard)
		{
			os << *(*pCard) << "\t";
		}

		if (aGenericPlayer.GetTotal() != 0)
		{
			std::cout << "(" << aGenericPlayer.GetTotal() << ")";
		}
	}
	else
	{
		os << "<empty>";
	}
	return os;
}

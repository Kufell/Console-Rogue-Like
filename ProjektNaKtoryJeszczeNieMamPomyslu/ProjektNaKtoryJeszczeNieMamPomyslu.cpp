#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

class Enemy;

//Odpowiedzialne za gracza
class Character {
private:
    std::string name;
    int health;
    int maxHealth;
    int attack;
    int healingVials;
    int emptyVials = 0;
    int gold;
    bool isGuardUp;
    float guardReducesTo = 0.50;
public:
    Character() : name(""), health(0), maxHealth(0), attack(0), healingVials(0), gold(0), isGuardUp(false) {}
    Character(const std::string& name, int maxHealth, int attack, int healingVials, int gold)
        : name(name), maxHealth(maxHealth), health(maxHealth), attack(attack), healingVials(healingVials), gold(gold), isGuardUp(false) {}
    


    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getAttack() const { return attack; }
    int getHealingVials() const { return healingVials; }
    int getGold() const { return gold; }
    bool isGuarding() const { return isGuardUp; }
    
    float guardReduce()
    {
        return guardReducesTo;
    }
    
    void reduceHealth(int amount) {
        health -= amount;
        if (health < 0) {
            health = 0;
        }
    }

    void increaseHealth(int amount) {
        health += amount;
        if (health > maxHealth) {
            health = maxHealth;
        }
    }

    void getNewVial() {
        emptyVials++;
    }

    void refillHealingVials() {
        if (emptyVials > 0)
        {
            emptyVials--;
            healingVials++;
        }
    }

    void increaseAttack(int amount) {
        attack += amount;
    }

    void addGold(int amount) {
        gold += amount;
    }

    void subtractGold(int amount) {
        gold -= amount;
        if (gold < 0) {
            gold = 0;
        }
    }
    
    void guard() {
        isGuardUp = true;
    }

    void heal() {
        if (healingVials > 0) {
            emptyVials++;
            healingVials--;
            std::cout << "You decide to heal!\n";
            int restoredHealth = static_cast<int>(0.35 * maxHealth); 
            increaseHealth(restoredHealth);
            if (health > maxHealth) {
                health = maxHealth;
            }
            std::cout << "Odzyskujesz " << restoredHealth << " HP!\n";
        } else {
            std::cout << "Nie masz ju¿ ¿adnej fiolki lecz¹cej...\n";
        }
    }

    void run() {
        std::cout << "Decydujesz siê na ucieczkê...\n";
        int penalty = static_cast<int>(0.2 * gold); 
        gold -= penalty;
        std::cout << "...Uciekaj¹c w pop³ochu " << penalty << " monet wypad³o ci z kieszeni.\n";
    }

    void resetGuarding() {
        isGuardUp = false;
    }

    void playerRewards();
    void setHealth(int newHealth) { health = newHealth; }
};

//Nagrody po walce
void Character::playerRewards() {
    std::cout << "Wybierz nagrodê!\n";
    std::cout << "Kamieñ szlifierski (+2 obra¿enia)\n";
    std::cout << "2. Nape³ni³ pust¹ fiolkê (Iloœæ fiolek: " << emptyVials << ")\n";
    std::cout << "3. WeŸ dodatkow¹ pust¹ fiolkê\n";
    std::cout << "4. Poszukaj jeszcze trochê z³ota\n";
    std::cout << "Twój wybór: ";

    int choice;
    std::cin >> choice;
    int goldBonus = 10;
    switch(choice) {
        case 1:
            increaseAttack(2); 
            std::cout << "Naostrzy³eœ swój miecz, wzmacniaj¹c jej obra¿enia!\n";
            break;
        case 2:
            refillHealingVials();
            std::cout << "Nape³ni³eœ fiolkê lecz¹c¹!\n";
            break;
        case 3:
            getNewVial();
            std::cout << "Wzi¹³eœ dodatkow¹ fiolkê lecz¹c¹\n";
            break;
        case 4:
            addGold(goldBonus);
            std::cout << "Zamiast u³atwieñ do walki szukasz garstki z³ota... " << goldBonus << " monet trafi³o do twojej kieszeni!\n";
            break;
        default:
            std::cout << "Nieprawid³owy wybór! Nie bierzesz nic...\n";
            break;
    }
}

//Odpowiedzialne za przeciwników
class Enemy {
private:
    std::string name;
    int health;
    int attack;
    int goldValue;
    int dummyCounter = 0;

public:
    Enemy(const std::string& name, int health, int attack, int goldValue)
        : name(name), health(health), attack(attack), goldValue(goldValue) {}

    std::string getName() const { return name; }
    int getHealth() const { return health; }

    virtual void reduceHealth(int amount) {
        health -= amount;
        if (health < 0) {
            health = 0;
        }
    }

    virtual void attackPlayer(Character& player) {
        int damage = attack;
        if(player.isGuarding())
        {
            damage = attack * player.guardReduce();
        }
        std::cout << name << " atakuje zadaj¹c " << damage << " obra¿eñ!\n";
        player.reduceHealth(damage);
    }

    int getGoldValue() const { return goldValue; }
    int getAttack() const {return attack; }
    int getDummy(){return dummyCounter;}
    void addDummy(int val){dummyCounter += val;}
    void damageUp(int val){attack += val;}
};

//Pijawka
class Leech : public Enemy {
public:
    Leech() : Enemy("Pijawka", 20, 4, 8) {}

    //Pijawka leczy siê z obra¿eñ
    void attackPlayer(Character& player) override {
        int damage = getAttack();
        std::cout << getName() << " wysysa z ciebie " << damage << " HP, leczy siê przez to o "<< damage / 2 << " HP!\n";
        player.reduceHealth(damage);
        this->reduceHealth(- damage / 2);
    }
};
//Wœciek³y trup
class Furiosa : public Enemy {
public:
    Furiosa() : Enemy("Wœciek³y trup", 45, 1, 12) {}

    //Co drug¹ turê zwiêksza obra¿enia
    void attackPlayer(Character& player) override {
        int damage = getAttack();
        std::cout << getName() << " atakuje zadaj¹c " << damage << " HP!\n";
        player.reduceHealth(damage);
        addDummy(1);
        if(getDummy() >= 2)
        {
            std::cout << "Wœciek³oœæ przek³ada siê na wiêksze obra¿enia...";
            this->damageUp(1);
            this->addDummy(-getDummy());
        }
    }
};

//Ca³a logika gry, zosta³o kilka œmieci, gdy projekt by³ trochê bardziej ambitny
class Game {
private:
    std::vector<Enemy*> encounteredEnemies;
    std::vector<Enemy*> allEnemies;
    Character player;
    int defeatedEnemies;
    bool gameEnded;

public:
    Game() : defeatedEnemies(0), gameEnded(false)
    {
        allEnemies.push_back((new Leech));
        allEnemies.push_back((new Furiosa));
        allEnemies.push_back(new Enemy("Ghul", 30, 2, 10));
        allEnemies.push_back(new Enemy("Szkielet", 20, 1, 5));
        allEnemies.push_back(new Enemy("Goblin", 25, 2, 7));
    }

    ~Game() {
        for (Enemy* enemy : allEnemies) {
            delete enemy;
        }
    }

    void startMenu();
    void play();
    void howToPlay();
    void leaderboard();

    void encounterEnemy(Enemy* enemy);
    void playerTurn(Enemy* enemy);
    void fight(Enemy* enemy);
    void postBattleLogic(Enemy* enemy);
    void saveScore();
    void displayLeaderboard();
};

void Game::startMenu() {
    int choice;
    while (true) {
        std::cout << "=== Menu G³ówne ===\n";
        std::cout << "1. Zagraj\n";
        std::cout << "2. Jak graæ\n";
        std::cout << "3. Tablica wyników\n";
        std::cout << "4. WyjdŸ\n";
        std::cout << "Wybierz opcjê: ";
        std::cin >> choice;

        switch(choice) {
            case 1: {
                std::cout << "\nPrzygoda siê zaczyna...\n";
                Game newGame;
                newGame.play();
                break;
            }
            case 2:
                howToPlay();
                break;
            case 3:
                leaderboard();
                break;
            case 4:
                std::cout << "Do zobaczenia!\n";
                return;
            default:
                std::cout << "Nieprawid³owy wybór!\n";
                break;
        }
    }
}

void Game::play() {
    srand(time(NULL));
    std::string playerName;
    std::cout << "Enter your name: ";
    std::cin >> playerName;
    Character player(playerName, 20, 5, 2, 0); 
    this->player = player; 

    std::random_shuffle(allEnemies.begin(), allEnemies.end());

    for (Enemy* enemy : allEnemies) {
        encounterEnemy(enemy);
    }

    if (defeatedEnemies == allEnemies.size()) {
        std::cout << "Wytêpi³eœ wszystkie potwory, prze¿y³eœ i wynios³eœ "<< player.getGold() << " z³otych monet!\n";
        saveScore();
    }
}

void Game::howToPlay() {
    std::cout << "=== Jak graæ? ===\n";
    std::cout << "1. Walka\nPodczas swojej tury zadecyduj czy chcesz walczyæ, broniæ siê gard¹, uleczyæ siê albo uciekaæ\n";
    std::cout << "Walcz¹c zadajesz obra¿enia przeciwnikowi.\nBroni¹c siê redukujesz obra¿enoa od nastêpnego ataku.\n";
    std::cout << "Lecz¹c siê zu¿ywasz fiolkê lecz¹c¹.\n Uciekaj¹c koñczysz swój przebieg.\n";
    std::cout << "Nastêpnie przeciwnik rozpoczyna swoj¹ turê. Aby wygraæ walkê zredukuj zdrowie przeciwnika do 0.\n";
    std::cout << "2. Wygrana walka\nPo wygranej walce mo¿esz:\nNaostrzyæ swoj¹ broñ.\nNape³niæ pust¹ fiolkê (jeœli tak¹ masz).\n";
    std::cout << "Wzi¹æ pust¹ fiolkê.\nWzi¹æ garstkê z³ota.\n";
    std::cout << "3. Wygrana/Przegrana\nJeœli prze¿yjesz wszystkich przeciników iloœæ z³ota jak¹ zdoby³eœ stanie siê twoim wynikiem.\n";
    std::cout << "Jeœli uciek³eœ przed walk¹ czêœæ z³ota 'wypadnie ci z kieszeni'.\nJeœli twoja postaæ umrze twój wynik nie zostanie zapisany, przegra³eœ\n";
    std::cout << "Pamiêtaj, ¿e czêœci¹ gry jest odkrywanie kolejnych zale¿noœci i taktyk.\nPOWODZENIA\n";
}

void Game::leaderboard() {
    displayLeaderboard();
}

void Game::encounterEnemy(Enemy* enemy) {
    if(gameEnded) return;
    if (std::find(encounteredEnemies.begin(), encounteredEnemies.end(), enemy) != encounteredEnemies.end()) {
        return;
    }

    std::cout << "You encounter a " << enemy->getName() << " with " << enemy->getHealth() << " HP!\n";
    playerTurn(enemy);
    encounteredEnemies.push_back(enemy);
}

void Game::playerTurn(Enemy* enemy) {
    while (player.getHealth() > 0 && enemy->getHealth() > 0 && !gameEnded) {
        std::cout << "Twoja tura!\n";
        std::cout << "Opcje:\n";
        std::cout << "1. Walcz\n";
        std::cout << "2. Broñ siê\n";
        std::cout << "3. Ulecz siê";
        if (player.getHealingVials() > 0) {
            std::cout << " (Iloœæ fiolek: " << player.getHealingVials() << ")";
        } else {
            std::cout << " (Tylko czym?...)";
        }
        std::cout << "\n4. Uciekaj\n";
        std::cout << "Wybierz opcjê: ";

        int choice;
        std::cin >> choice;

        switch(choice) {
            case 1:
                fight(enemy);
                break;
            case 2:
                player.guard();
                break;
            case 3:
                if (player.getHealingVials() > 0) {
                    player.heal();
                    std::cout << "Wypi³eœ ca³¹ zawartoœæ fiolki, czujesz siê trochê lepiej!\n";
                    std::cout << "Masz aktualnie " << player.getHealth() << " HP!\n";
                } else {
                    std::cout << "Próbujesz znaleœæ pe³n¹ fiolkê, ale ¿adnej nie masz...\n";
                }
                break;
            case 4:
                player.run();
                gameEnded = true;
                break;
            default:
                std::cout << "Nieprawid³owy wybór!\n";
                break;
        }

        // Enemy's turn
        if(enemy->getHealth() > 0)
        {
            enemy->attackPlayer(player);   
        }
        if (player.getHealth() <= 0) {
            std::cout << "Zosta³eœ pokonany...\n";
            gameEnded = true;
            return;
        }
        std::cout << "Masz aktualnie " << player.getHealth() << " HP!\n";
        player.resetGuarding();
    }
}

void Game::fight(Enemy* enemy) {
    int playerDamage = player.getAttack();
    std::cout << "Decydujesz siê na atak!\n";
    enemy->reduceHealth(playerDamage);
    std::cout << "Zadajesz " << enemy->getHealth() << " HP!\n";

    if (enemy->getHealth() <= 0) {
        defeatedEnemies++;
        if (defeatedEnemies == allEnemies.size()) {
            return;
        }
        postBattleLogic(enemy);
    }
}

void Game::postBattleLogic(Enemy* enemy) {
    std::cout << "Zwyciêstwo! " << enemy->getName() << " pada na ziemiê!\n";
    player.addGold(enemy->getGoldValue());
    player.playerRewards();

    int restoredHealth = static_cast<int>(0.3 * player.getMaxHealth()); // Restore 20% of maximum health
    player.increaseHealth(restoredHealth);
    if (player.getHealth() > player.getMaxHealth()) {
        player.setHealth(player.getMaxHealth());
    }
    std::cout << "Czujesz, ¿e jest trochê bezpiecznie, bierzesz chwilê przerwy...\n" << restoredHealth << " HP zosta³o odnowione!\n";
}

void Game::saveScore() {
    std::ofstream leaderboardFile("leaderboard.txt", std::ios::app); // Open file in append mode
    if (leaderboardFile.is_open()) {
        leaderboardFile << player.getName() << "," << player.getGold() << std::endl;
        leaderboardFile.close();
    } else {
        std::cerr << "Unable to open leaderboard file.\n";
    }
}

void Game::displayLeaderboard() {
    std::ifstream leaderboardFile("leaderboard.txt");
    if (leaderboardFile.is_open()) {
        std::vector<std::pair<std::string, int>> scores;
        std::string line;
        while (std::getline(leaderboardFile, line)) {
            std::stringstream ss(line);
            std::string playerName;
            int score;
            std::getline(ss, playerName, ',');
            ss >> score;
            scores.push_back(std::make_pair(playerName, score));
        }
        leaderboardFile.close();

        //Sortowannie wyników
        std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        // Wyœwietlanie wyników
        std::cout << "=== Tablica Wyników ===\n";
        std::cout << "Miejsce\tGracz\t\tZ³oto\n";
        int rank = 1;
        for (const auto& score : scores) {
            std::cout << rank << "\t" << std::left << std::setw(12) << score.first << "\t" << score.second << "\n";
            rank++;
        }
    } else {
        std::cerr << "Unable to open leaderboard file.\n";
    }
}

int main() {
    Game game;
    game.startMenu();
    return 0;
}

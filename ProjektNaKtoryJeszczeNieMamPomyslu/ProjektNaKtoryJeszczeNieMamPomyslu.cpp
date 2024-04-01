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
            int restoredHealth = static_cast<int>(0.35 * maxHealth); // Restore 35% of maximum health
            increaseHealth(restoredHealth);
            if (health > maxHealth) {
                health = maxHealth;
            }
            std::cout << "You regain " << restoredHealth << " health!\n";
        } else {
            std::cout << "You don't have any healing vials left!\n";
        }
    }

    void run() {
        std::cout << "You decide to run away!\n";
        int penalty = static_cast<int>(0.2 * gold); // Calculate 20% penalty
        gold -= penalty; // Deduct penalty from player's gold
        std::cout << "You lose " << penalty << " gold as a penalty for running away.\n";
    }

    void resetGuarding() {
        isGuardUp = false;
    }

    void playerRewards(); // Declaration
    void setHealth(int newHealth) { health = newHealth; }
};

void Character::playerRewards() {
    std::cout << "Choose your reward:\n";
    std::cout << "1. Sharpening stone (Increase attack)\n";
    std::cout << "2. Refill healing vial (" << emptyVials << " empty vials left\n";
    std::cout << "3. Grab additional empty vial\n";
    std::cout << "4. No thanks\n";
    std::cout << "Enter your choice: ";

    int choice;
    std::cin >> choice;
    int goldBonus = 10;
    switch(choice) {
        case 1:
            increaseAttack(2); // Example: Increase attack by 2 points
            std::cout << "You sharpen your weapon. Attack increased!\n";
            break;
        case 2:
            refillHealingVials();
            std::cout << "You refill a healing vial!\n";
            break;
        case 3:
            getNewVial();
            std::cout << "You grab an additional empty vial!\n";
            break;
        case 4:
            addGold(goldBonus);
            std::cout << "You receive " << goldBonus << " extra gold!\n";
            break;
        default:
            std::cout << "Invalid choice! No reward taken.\n";
            break;
    }
}

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
        std::cout << "The " << name << " attacks you and deals " << damage << " damage!\n";
        player.reduceHealth(damage);
    }

    int getGoldValue() const { return goldValue; }
    int getAttack() const {return attack; }
    int getDummy(){return dummyCounter;}
    void addDummy(int val){dummyCounter += val;}
    void damageUp(int val){attack += val;}
};
class Leech : public Enemy {
public:
    Leech() : Enemy("Leech", 20, 4, 8) {}

    void attackPlayer(Character& player) override {
        int damage = getAttack();
        std::cout << "The " << getName() << " attacks you and deals " << damage << " damage, but it heals itself!\n";
        player.reduceHealth(damage);
        this->reduceHealth(- damage / 2);
    }
};
class Furiosa : public Enemy {
public:
    Furiosa() : Enemy("Furiosa", 45, 1, 12) {}

    void attackPlayer(Character& player) override {
        int damage = getAttack();
        std::cout << "The " << getName() << " attacks you and deals " << damage << "\n";
        player.reduceHealth(damage);
        addDummy(1);
        if(getDummy() >= 2)
        {
            std::cout << "It also amps up damage!";
            this->damageUp(1);
            this->addDummy(-getDummy());
        }
    }
};

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
        allEnemies.push_back(new Enemy("Ghoul", 30, 2, 10));
        allEnemies.push_back(new Enemy("Skeleton", 20, 1, 5));
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
        std::cout << "=== Main Menu ===\n";
        std::cout << "1. Play\n";
        std::cout << "2. How to Play\n";
        std::cout << "3. Leaderboard\n";
        std::cout << "4. Quit\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;

        switch(choice) {
            case 1: {
                std::cout << "\nStarting the game...\n";
                Game newGame;
                newGame.play();
                break;
            }
            case 2:
                howToPlay();
                std::cout << "Returning to main menu...\n";
                break;
            case 3:
                leaderboard();
                std::cout << "Returning to main menu...\n";
                break;
            case 4:
                std::cout << "Quitting the game...\n";
                return;
            default:
                std::cout << "Invalid choice! Please try again.\n";
                break;
        }
    }
}

void Game::play() {
    srand(time(NULL)); // Seed for random number generation
    std::string playerName;
    std::cout << "Enter your name: ";
    std::cin >> playerName;
    Character player(playerName, 20, 5, 2, 0); // Character creation with the provided name
    this->player = player; // Set the player

    std::random_shuffle(allEnemies.begin(), allEnemies.end());

    for (Enemy* enemy : allEnemies) {
        encounterEnemy(enemy);
    }

    if (defeatedEnemies == allEnemies.size()) {
        std::cout << "Congratulations! You have defeated all enemies. You win!\n";
        saveScore();
    }
}

void Game::howToPlay() {
    std::cout << "=== How to Play ===\n";
    // Instructions on how to play
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
        std::cout << "It's your turn!\n";
        std::cout << "Options:\n";
        std::cout << "1. Fight\n";
        std::cout << "2. Guard\n";
        std::cout << "3. Heal";
        if (player.getHealingVials() > 0) {
            std::cout << " (" << player.getHealingVials() << " vials)";
        } else {
            std::cout << " (No vials)";
        }
        std::cout << "\n4. Run\n";
        std::cout << "Enter your choice: ";

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
                } else {
                    std::cout << "You don't have any healing vials left!\n";
                }
                break;
            case 4:
                player.run();
                gameEnded = true;
                break;
            default:
                std::cout << "Invalid choice! Please try again.\n";
                break;
        }

        // Enemy's turn
        if(enemy->getHealth() > 0)
        {
            enemy->attackPlayer(player);   
        }
        if (player.getHealth() <= 0) {
            std::cout << "You have been defeated!\n";
            gameEnded = true;
            return;
        }
        std::cout << "Your current health is " << player.getHealth() << "!\n";
        player.resetGuarding();
    }
}

void Game::fight(Enemy* enemy) {
    int playerDamage = player.getAttack();
    std::cout << "You attack the " << enemy->getName() << "!\n";
    enemy->reduceHealth(playerDamage);
    std::cout << "Enemy " << enemy->getName() << "'s health: " << enemy->getHealth() << "\n";

    if (enemy->getHealth() <= 0) {
        defeatedEnemies++;
        if (defeatedEnemies == allEnemies.size()) {
            return;
        }
        postBattleLogic(enemy);
    }
}

void Game::postBattleLogic(Enemy* enemy) {
    std::cout << "You defeated the " << enemy->getName() << "!\n";
    player.addGold(enemy->getGoldValue());
    player.playerRewards();

    int restoredHealth = static_cast<int>(0.3 * player.getMaxHealth()); // Restore 20% of maximum health
    player.increaseHealth(restoredHealth);
    if (player.getHealth() > player.getMaxHealth()) {
        player.setHealth(player.getMaxHealth());
    }
    std::cout << "You take a breather and regain " << restoredHealth << " health!\n";
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

        // Sort scores based on the score (descending order)
        std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        // Display top 20 scores
        std::cout << "=== Leaderboard ===\n";
        std::cout << "Rank\tPlayer\t\tScore\n";
        int rank = 1;
        for (const auto& score : scores) {
            std::cout << rank << "\t" << std::left << std::setw(12) << score.first << "\t" << score.second << "\n";
            if (rank == 20) // Display only top 20 scores
                break;
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

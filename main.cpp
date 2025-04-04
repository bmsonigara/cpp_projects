#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

class IBoard
{
public:
    virtual void display() const = 0;
    virtual bool placeMark(int row, int col, char mark) = 0;
    virtual bool checkWin(char mark) const = 0;
    virtual bool isFull() const = 0;
    virtual ~IBoard() = default;
};

class Board : public IBoard
{
private:
    std::vector<std::vector<char>> grid;

public:
    Board()
    {
        grid = std::vector<std::vector<char>>(3, std::vector<char>(3, ' '));
    }

    void display() const override
    {
        std::cout << "  0 1 2\n";
        for (int i = 0; i < 3; ++i)
        {
            std::cout << i << " ";
            for (int j = 0; j < 3; ++j)
            {
                std::cout << grid[i][j] << " ";
            }
            std::cout << "\n";
        }
    }

    bool placeMark(int row, int col, char mark) override
    {
        if (row < 0 || row >= 3 || col < 0 || col >= 3 || grid[row][col] != ' ')
        {
            return false;
        }
        grid[row][col] = mark;
        return true;
    }

    bool checkWin(char mark) const override
    {
        // Check rows and columns
        for (int i = 0; i < 3; ++i)
        {
            if ((grid[i][0] == mark && grid[i][1] == mark && grid[i][2] == mark) || 
                (grid[0][i] == mark && grid[1][i] == mark && grid[2][i] == mark))
            {
                return true;
            }
        }
        // Check diagonals
        if ((grid[0][0] == mark && grid[1][1] == mark && grid[2][2] == mark) || 
            (grid[0][2] == mark && grid[1][1] == mark && grid[2][0] == mark))
        {
            return true;
        }
        return false;
    }

    bool isFull() const override
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (grid[i][j] == ' ')
                    return false;
            }
        }
        return true;
    }
};

class IPlayer
{
public:
    virtual char getMark() const = 0;
    virtual std::string getName() const = 0;
    virtual ~IPlayer() = default;
};

class Player : public IPlayer
{
private:
    std::string name;
    char mark;

public:
    Player(std::string playerName, char playerMark)
        : name(std::move(playerName)), mark(playerMark) {}

    char getMark() const override
    {
        return mark;
    }

    std::string getName() const override
    {
        return name;
    }
};

class TicTacToeGame
{
private:
    IBoard &board;
    IPlayer &player1;
    IPlayer &player2;
    IPlayer *currentPlayer;
    
public:
    TicTacToeGame(IBoard &b, IPlayer &p1, IPlayer &p2)
        : board(b), player1(p1), player2(p2), currentPlayer(&player1) {}

    void start()
    {
        int row, col;
        while (true)
        {
            board.display();
            std::cout << currentPlayer->getName() << "'s turn (" << currentPlayer->getMark() << ").\n";
            std::cout << "Enter row and column: ";
            std::cin >> row >> col;

            if (!board.placeMark(row, col, currentPlayer->getMark()))
            {
                std::cout << "Invalid move. Try again.\n";
                continue;
            }

            if (board.checkWin(currentPlayer->getMark()))
            {
                board.display();
                std::cout << currentPlayer->getName() << " wins!\n";
                return;
            }

            if (board.isFull())
            {
                board.display();
                std::cout << "It's a draw!\n";
                return;
            }

            // Switch player
            currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
        }
    }
};

int main()
{
    // Player creation
    Player player1("Player 1", 'X');
    Player player2("Player 2", 'O');
    
    // Board creation
    Board board;
    
    // Game controller
    TicTacToeGame game(board, player1, player2);
    game.start();
    
    return 0;
}

#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include <limits>
#include <memory>
#include <optional>
#include <utility>

constexpr int BOARD_SIZE = 3;

class IBoard
{
public:
    virtual void display() const = 0;
    virtual bool placeMark(int row, int col, char mark) = 0;
    virtual bool checkWin(char mark) const = 0;
    virtual bool isFull() const = 0;
    virtual char getCell(int row, int col) const = 0;
    virtual ~IBoard() = default;
};

class Board : public IBoard
{
private:
    char grid[BOARD_SIZE][BOARD_SIZE];

public:
    Board()
    {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                grid[i][j] = ' ';
            }
        }
    }

    void display() const override
    {
        std::cout << "  0 1 2\n";
        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            std::cout << i << " ";
            for (int j = 0; j < BOARD_SIZE; ++j)
            {
                std::cout << grid[i][j] << " ";
            }
            std::cout << "\n";
        }
    }

    bool placeMark(int row, int col, char mark) override
    {
        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || grid[row][col] != ' ')
        {
            return false;
        }
        grid[row][col] = mark;
        return true;
    }

    char getCell(int row, int col) const override
    {
        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
        {
            throw std::out_of_range("Invalid board position");
        }
        return grid[row][col];
    }

    bool checkWin(char mark) const override
    {
        // Check rows and columns
        for (int i = 0; i < BOARD_SIZE; ++i)
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
        for (int i = 0; i < BOARD_SIZE; ++i)
        {
            for (int j = 0; j < BOARD_SIZE; ++j)
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

class ComputerPlayer : public IPlayer
{
private:
    std::string name;
    char mark;

    // Helper function to check if a move leads to a win
    bool wouldWin(const IBoard& board, int row, int col, char playerMark) const {
        Board tempBoard;
        // Copy the current board state
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                tempBoard.placeMark(i, j, board.getCell(i, j));
            }
        }
        // Try the move
        if (tempBoard.placeMark(row, col, playerMark)) {
            return tempBoard.checkWin(playerMark);
        }
        return false;
    }

public:
    ComputerPlayer(char playerMark) : name("Computer"), mark(playerMark) {}

    char getMark() const override
    {
        return mark;
    }

    std::string getName() const override
    {
        return name;
    }

    std::optional<std::pair<int, int>> getBestMove(const IBoard& board) const
    {
        char opponentMark = (mark == 'X') ? 'O' : 'X';
        
        // Try to find a winning move
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board.getCell(i, j) == ' ' && wouldWin(board, i, j, mark)) {
                    return {{i, j}};
                }
            }
        }

        // Try to block opponent's winning move
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board.getCell(i, j) == ' ' && wouldWin(board, i, j, opponentMark)) {
                    return {{i, j}};
                }
            }
        }

        // Try to take center
        if (board.getCell(1, 1) == ' ') {
            return {{1, 1}};
        }

        // Try to take corners
        const std::pair<int, int> corners[] = {{0,0}, {0,2}, {2,0}, {2,2}};
        for (const auto& corner : corners) {
            if (board.getCell(corner.first, corner.second) == ' ') {
                return corner;
            }
        }

        // Take any available space
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board.getCell(i, j) == ' ') {
                    return {{i, j}};
                }
            }
        }

        return std::nullopt;
    }
};

class TicTacToeGame
{
private:
    IBoard& board;
    IPlayer& player1;
    std::unique_ptr<IPlayer> player2;
    IPlayer* currentPlayer;
    bool isVsComputer;
    
public:
    TicTacToeGame(IBoard& b, IPlayer& p1, std::unique_ptr<IPlayer> p2, bool vsComputer = false)
        : board(b), player1(p1), player2(std::move(p2)), currentPlayer(&player1), isVsComputer(vsComputer) {}

    void start()
    {
        int row, col;
        while (true)
        {
            board.display();
            std::cout << currentPlayer->getName() << "'s turn (" << currentPlayer->getMark() << ").\n";
            
            if (isVsComputer && currentPlayer->getName() == "Computer") {
                // Computer's turn
                auto move = static_cast<const ComputerPlayer*>(currentPlayer)->getBestMove(board);
                if (!move) {
                    throw std::runtime_error("Computer failed to find a valid move");
                }
                std::tie(row, col) = *move;
                std::cout << "Computer chooses position: " << row << " " << col << "\n";
            } else {
                // Human's turn
                std::cout << "Enter row and column (0-2): ";
                while (!(std::cin >> row >> col) || row < 0 || row > 2 || col < 0 || col > 2) {
                    std::cout << "Invalid input! Please enter numbers between 0 and 2: ";
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }

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
            currentPlayer = (currentPlayer == &player1) ? player2.get() : &player1;
        }
    }
};

int main()
{
    std::cout << "Welcome to Tic Tac Toe!\n";
    std::cout << "Choose game mode:\n";
    std::cout << "1. Human vs Human\n";
    std::cout << "2. Human vs Computer\n";
    
    int choice;
    while (!(std::cin >> choice) || (choice != 1 && choice != 2)) {
        std::cout << "Invalid choice! Please enter 1 or 2: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    // Player creation
    Player player1("Player 1", 'X');
    std::unique_ptr<IPlayer> player2;
    
    if (choice == 1) {
        player2 = std::make_unique<Player>("Player 2", 'O');
    } else {
        player2 = std::make_unique<ComputerPlayer>('O');
    }
    
    // Board creation
    Board board;
    
    // Game controller
    TicTacToeGame game(board, player1, std::move(player2), choice == 2);
    game.start();
    
    return 0;
}

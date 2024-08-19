#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "../include/sfmlbutton.hpp"

const int N = 9;
const int SUBGRID = 3;

template <typename T>
T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

std::vector<int> getRandomNumbers() {
    std::vector<int> numbers(N);
    for (int i = 0; i < N; ++i) {
        numbers[i] = i + 1;
    }
    std::random_shuffle(numbers.begin(), numbers.end());
    return numbers;
}

bool isSafeSudoku(int row, int col, int value, const std::vector<std::vector<int>>& grid) {
    for (int i = 0; i < N; ++i) {
        if (grid[row][i] == value || grid[i][col] == value) {
            return false;
        }
    }

    int startRow = row - row % SUBGRID;
    int startCol = col - col % SUBGRID;
    for (int i = 0; i < SUBGRID; ++i) {
        for (int j = 0; j < SUBGRID; ++j) {
            if (grid[startRow + i][startCol + j] == value) {
                return false;
            }
        }
    }
    return true;
}

bool solveSudoku(std::vector<std::vector<int>>& grid) {
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            if (grid[row][col] == 0) {
                std::vector<int> randomNumbers = getRandomNumbers();
                for (int value : randomNumbers) {
                    if (isSafeSudoku(row, col, value, grid)) {
                        grid[row][col] = value;

                        if (solveSudoku(grid)) {
                            return true;
                        }

                        grid[row][col] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

void generateSudoku(std::vector<std::vector<int>>& grid) {
    solveSudoku(grid);

    int cluesToRemove = 120 - (rand() % 10);
    for (int i = 0; i < cluesToRemove; ++i) {
        int row = rand() % N;
        int col = rand() % N;
        grid[row][col] = 0;
    }
}

void drawSudokuGrid(sf::RenderWindow& window, const std::vector<std::vector<int>>& grid, int tileSize, sf::Font& font) {
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            sf::RectangleShape tile(sf::Vector2f(tileSize, tileSize));
            tile.setPosition(col * tileSize, row * tileSize);

            if ((row / SUBGRID + col / SUBGRID) % 2 == 0) {
                tile.setFillColor(sf::Color(220, 220, 220));
            } else {
                tile.setFillColor(sf::Color::White);
            }

            tile.setOutlineThickness(1.f);
            tile.setOutlineColor(sf::Color::Black);

            window.draw(tile);

            if (grid[row][col] != 0) {
                sf::Text text;
                text.setFont(font);
                text.setString(std::to_string(grid[row][col]));
                text.setCharacterSize(tileSize / 2);
                text.setFillColor(sf::Color::Black);

                sf::FloatRect textRect = text.getLocalBounds();
                text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
                text.setPosition(col * tileSize + tileSize / 2.0f, row * tileSize + tileSize / 2.0f);
                window.draw(text);
            }
        }
    }
}

int main() {
    std::srand(std::time(nullptr)); 
    std::vector<std::vector<int>> grid(N, std::vector<int>(N, 0));

    const unsigned int FPS = 60;
    generateSudoku(grid);

    int tileSize = 50;
    const int windowWidth = N * tileSize;
    const int windowHeight = 80 + N * tileSize;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Juego Sudoku");
    window.setFramerateLimit(FPS);

    // Cargar fuentes
    sf::Font ft_arial;
    ft_arial.loadFromFile("fonts/Arial.ttf");

    // Controles
    RectButton btnAleatorizar(ft_arial, sf::Vector2f(120, 35.f), sf::Vector2f(20, windowHeight - 50));
    btnAleatorizar.setButtonLabel(20.f, "Aleatorizar");
    btnAleatorizar.setButtonColor(sf::Color(0, 0, 200), sf::Color(0, 0, 150), sf::Color(0, 0, 100));
    btnAleatorizar.setLabelColor(sf::Color::White);
    RectButton btnResolver(ft_arial, sf::Vector2f(100, 35.f), sf::Vector2f(windowWidth - 120.f, windowHeight - 50));
    btnResolver.setButtonLabel(20.f, "Resolver");
    btnResolver.setButtonColor(sf::Color(0, 200, 0), sf::Color(0, 150, 0), sf::Color(0, 100, 0));
    btnResolver.setLabelColor(sf::Color::White);

    sf::Event event;
    while (window.isOpen()) {
        // Step
        while (window.pollEvent(event)) {
            btnResolver.getButtonStatus(window, event);
            btnAleatorizar.getButtonStatus(window, event);
            if (btnResolver.isPressed) {
                std::vector<std::vector<int>> solution = grid;
                solveSudoku(solution);
                grid = solution;
            }
            if (btnAleatorizar.isPressed) {
                generateSudoku(grid);
            }

            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Dibujar
        window.clear();
        // - Botones
        btnAleatorizar.draw(window);
        btnResolver.draw(window);
        // - Cuadro
        drawSudokuGrid(window, grid, tileSize, ft_arial);

        window.display();
    }

    return 0;
}

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <sstream>
#include <string>


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Breakout Bash");
    window.setFramerateLimit(60);

    // Load background
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("background.jpg")) return -1;
    sf::Sprite background(backgroundTexture);

    // Load paddle
    sf::Texture paddleTexture;
    if (!paddleTexture.loadFromFile("paddle_block.png")) return -1;
    sf::Sprite paddle(paddleTexture);
    paddle.setScale(100.f / paddleTexture.getSize().x, 20.f / paddleTexture.getSize().y);
    paddle.setPosition(350.f, 550.f);

    // Load block textures
    const int blockTypes = 5;
    sf::Texture blockTextures[blockTypes];
    for (int i = 0; i < blockTypes; ++i) {
        std::stringstream ss;
        ss << "block" << (i + 1) << ".png";
        if (!blockTextures[i].loadFromFile(ss.str())) return -1;
    }

    // Font
    sf::Font font;
    if (!font.loadFromFile("Minecraft.ttf")) return -1;

    // Score
    int score = 0;
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10.f, 10.f);

    // Sounds
    sf::SoundBuffer paddleBuffer, blockBuffer, gameOverBuffer;
    if (!paddleBuffer.loadFromFile("paddle.wav")) return -1;
    if (!blockBuffer.loadFromFile("block.wav")) return -1;
    if (!gameOverBuffer.loadFromFile("gameover.wav")) return -1;
    sf::Sound paddleSound(paddleBuffer);
    sf::Sound blockSound(blockBuffer);
    sf::Sound gameOverSound(gameOverBuffer);

    // Startup music
    sf::Music startupMusic;
    if (!startupMusic.openFromFile("startup.wav")) return -1;
    startupMusic.play();

    // Ball
    sf::CircleShape ball(10.f);
    ball.setFillColor(sf::Color::White);
    ball.setPosition(400.f, 300.f);
    sf::Vector2f ballVelocity(4.f, -4.f);

    // Blocks
    std::vector<sf::Sprite> blocks;
    const int rows = 5, cols = 10;
    float blockWidth = 70.f, blockHeight = 20.f;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            sf::Sprite block;
            block.setTexture(blockTextures[i % blockTypes]);
            block.setScale(blockWidth / blockTextures[i % blockTypes].getSize().x,
                           blockHeight / blockTextures[i % blockTypes].getSize().y);
            block.setPosition(40.f + j * (blockWidth + 5.f), 50.f + i * (blockHeight + 5.f));
            blocks.push_back(block);
        }
    }

    // Startup screen
    bool gameStarted = false;
    sf::Text titleText;
    titleText.setFont(font);
    titleText.setString("Breakout Bash");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setPosition(195.f, 200.f);

    sf::Text promptText;
    promptText.setFont(font);
    promptText.setString("Press Enter to Start");
    promptText.setCharacterSize(24);
    promptText.setFillColor(sf::Color::White);
    promptText.setPosition(270.f, 300.f);

    while (window.isOpen() && !gameStarted) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
                gameStarted = true;
                startupMusic.stop(); // Stop startup music here!
            }
        }
        window.clear();
        window.draw(background);
        window.draw(titleText);
        window.draw(promptText);
        window.display();
    }

    // Game Over prompt text
    sf::Text exitPromptText;
    exitPromptText.setFont(font);
    exitPromptText.setString("Press ESC to Exit");
    exitPromptText.setCharacterSize(24);
    exitPromptText.setFillColor(sf::Color::White);
    exitPromptText.setPosition(280.f, 366.f);

    // Game loop
    bool gameOver = false;
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        if (!gameOver) {
            // Paddle movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && paddle.getPosition().x > 0)
                paddle.move(-7.f, 0.f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && paddle.getPosition().x + paddle.getGlobalBounds().width < 800)
                paddle.move(7.f, 0.f);

            // Ball movement
            ball.move(ballVelocity);
            if (ball.getPosition().x <= 0 || ball.getPosition().x + 2 * ball.getRadius() >= 800)
                ballVelocity.x *= -1;
            if (ball.getPosition().y <= 0)
                ballVelocity.y *= -1;

            // Game Over check
            if (ball.getPosition().y > 600) {
                gameOver = true;
                gameOverSound.play();
            }

            // Paddle collision
            if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds())) {
                ballVelocity.y = -abs(ballVelocity.y);  // bounce upward
                ball.setPosition(ball.getPosition().x, paddle.getPosition().y - 2 * ball.getRadius());
                paddleSound.play();
            }

            // Block collisions
            for (size_t i = 0; i < blocks.size(); ++i) {
                if (ball.getGlobalBounds().intersects(blocks[i].getGlobalBounds())) {
                    blocks.erase(blocks.begin() + i);
                    ballVelocity.y *= -1;
                    score += 10;
                    blockSound.play();
                    break;
                }
            }

            // Update score text
            std::stringstream ss;
            ss << "Score: " << score;
            scoreText.setString(ss.str());
        }

        // Drawing
        window.clear();
        window.draw(background);

	if (gameOver) {
	    sf::Text gameOverText;
	    gameOverText.setFont(font);
	    gameOverText.setString("Game Over");
	    gameOverText.setCharacterSize(48);
	    gameOverText.setFillColor(sf::Color::Red);
	    gameOverText.setPosition(250.f, 220.f);
	
	    sf::Text finalScoreText;
	    finalScoreText.setFont(font);
	    std::stringstream ss;
	    ss << "Final Score: " << score;
	    finalScoreText.setString(ss.str());
	    finalScoreText.setCharacterSize(28);
	    finalScoreText.setFillColor(sf::Color::Yellow);
	    finalScoreText.setPosition(280.f, 280.f);
	
	    window.draw(gameOverText);
	    window.draw(finalScoreText);
	    window.draw(exitPromptText);
	}

    else {
            window.draw(ball);
            window.draw(paddle);
            for (std::size_t i = 0; i < blocks.size(); ++i)
                window.draw(blocks[i]);
            window.draw(scoreText);
        }

        window.display();
    }

    return 0;
}


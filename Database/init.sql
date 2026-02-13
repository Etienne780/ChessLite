CREATE DATABASE IF NOT EXISTS game;
USE game;

CREATE TABLE IF NOT EXISTS test_game (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    rating INT NOT NULL
);

INSERT INTO test_game (name, rating) VALUES ('Alice', 1200), ('Bob', 1350);
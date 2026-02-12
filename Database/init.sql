CREATE DATABASE IF NOT EXISTS testdb;
USE testdb;

CREATE TABLE IF NOT EXISTS players (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    rating INT NOT NULL
);

INSERT INTO players (name, rating) VALUES ('Alice', 1200), ('Bob', 1350);

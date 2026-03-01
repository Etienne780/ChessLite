CREATE DATABASE IF NOT EXISTS game;
USE game;

CREATE TABLE IF NOT EXISTS agents (
    id BIGINT AUTO_INCREMENT PRIMARY KEY COMMENT 'Server-ID of the Agent (0 = invalide)',
    version INT DEFAULT 1 COMMENT 'Version of the Agent (for Sync)',
    name VARCHAR(30) NOT NULL COMMENT 'Name of the Agent',
    config LONGTEXT NOT NULL COMMENT 'Agent-Configuration (Base64)',
    
    matches_played INT DEFAULT 0 COMMENT 'Total matches played',
    matches_won INT DEFAULT 0 COMMENT 'Matches won',
    matches_played_white INT DEFAULT 0 COMMENT 'Matches played as white',
    matches_won_white INT DEFAULT 0 COMMENT 'Matches won as white'

) COMMENT='Saves all agents and their stats';

CREATE TABLE IF NOT EXISTS board_states (
    id BIGINT AUTO_INCREMENT PRIMARY KEY COMMENT 'Board-State ID',
    agent_id BIGINT NOT NULL COMMENT 'Reference to Agent',
    
    board_state VARCHAR(255) NOT NULL COMMENT 'Board-State as String (e.g. "222000111")',
    
    CONSTRAINT fk_board_states_agent FOREIGN KEY (agent_id) 
        REFERENCES agents(id) ON DELETE CASCADE ON UPDATE CASCADE
) COMMENT='Saves all board states of an agent';

CREATE TABLE IF NOT EXISTS game_moves (
    id BIGINT AUTO_INCREMENT PRIMARY KEY COMMENT 'Move-ID',
    board_state_id BIGINT COMMENT 'Reference to Board-State',
  
    evaluation FLOAT COMMENT 'Evaluation of the move',
    from_x INT NOT NULL COMMENT 'from X-Position',
    from_y INT NOT NULL COMMENT 'from Y-Position',
    to_x INT NOT NULL COMMENT 'to X-Position',
    to_y INT NOT NULL COMMENT 'to Y-Position',
    
    CONSTRAINT fk_game_moves_board_state FOREIGN KEY (board_state_id) 
        REFERENCES board_states(id) ON DELETE CASCADE ON UPDATE CASCADE
) COMMENT='Saves all moves of a Board-State';
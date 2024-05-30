BEGIN TRANSACTION;

-- Remove previous tables and domains

DROP SCHEMA public CASCADE;
CREATE SCHEMA public;

-- Domains

CREATE DOMAIN cell_cord AS INTEGER CHECK (VALUE >= 0 AND VALUE <= 9);
CREATE DOMAIN s_str AS VARCHAR(20);
CREATE DOMAIN m_str AS VARCHAR(100);
CREATE DOMAIN grid AS VARCHAR(318) CHECK (VALUE ~ '^(\.{0,2}\d+)+$');
CREATE TYPE cells_operation AS ENUM('Add', 'Sub', 'Mul', 'Div', 'Mod', 'Concat');
CREATE TYPE scene_type AS ENUM('Menu', 'Settings', 'Victory', 'Defeat');
CREATE TYPE cell AS (value INTEGER, x cell_cord, y cell_cord);
-- ^       = start of the string;
-- \.{0,2} = between 0 and 2 dots: "", ".", or "..";
-- \d+     = any number of digits;
-- (...)+  = match anything that is inside parentheses any number of times;
-- $       = end of string
--
-- We represent a grid from the left bottom corner to the top right corner,
-- moving from left to right and from bottom to top. Every cell is separated by
-- a dot. Every row is separated by two dots.
--
-- Supposing we have a two-digit number in every cell of a full grid, we need a
-- 3 string per cell, plus 2 strings per row (except the last), for a total of 
-- 318. The grid only represent the initial grid, where cells can't have a
-- higher value than 99. (Remember that a grid is a 10x10.)


-- Tables

CREATE TABLE Profile(
	nickname m_str PRIMARY KEY,
	creation DATE NOT NULL
);

CREATE TABLE Session(
    id SERIAL PRIMARY KEY,
    player m_str NOT NULL,
	startstamp TIMESTAMP NOT NULL,
    UNIQUE (player, startstamp),
	FOREIGN KEY (player) REFERENCES Profile(nickname)
);

CREATE TABLE Scene(
    player_session INTEGER,
    ord INTEGER,
    type_name scene_type NOT NULL,
    time_in INTERVAL NOT NULL,
    max_wait REAL NOT NULL,
    avg_wait REAL NOT NULL,
    PRIMARY KEY (player_session, ord),
	FOREIGN KEY (player_session) REFERENCES session(id),
    CONSTRAINT non_negative_ord CHECK (ord >= 0)
);

CREATE TABLE Quest(
    id SERIAL PRIMARY KEY,
	session_player m_str,
    session_startstamp TIMESTAMP,
    goal INTEGER NOT NULL,
    startstamp TIMESTAMP NOT NULL,
    endstamp TIMESTAMP NOT NULL,
	result INTEGER,
    hp INTEGER NOT NULL,
    grid grid NOT NULL,
    FOREIGN KEY (session_player, session_startstamp) REFERENCES Session(player, startstamp),
    UNIQUE (session_player, session_startstamp, startstamp),
    CONSTRAINT start_before_end CHECK (startstamp < endstamp)
);

CREATE TABLE Operation(
    quest INTEGER,
    ord INTEGER,
	op_type cells_operation NOT NULL, 
	primary_cell cell NOT NULL,
    secondary_cell cell NOT NULL,
    PRIMARY KEY (quest, ord),
    FOREIGN KEY (quest) REFERENCES Quest(id),
    CONSTRAINT non_negative_ord CHECK (ord >= 0)
);

COMMIT;
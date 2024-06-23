BEGIN TRANSACTION;

-- REMOVE PREVIOUS TABLES AND DOMAINS

DROP   SCHEMA public CASCADE;
CREATE SCHEMA public;

-- DOMAINS

CREATE DOMAIN cell_cord  AS INTEGER      CHECK (VALUE >= 0 AND VALUE <= 9);
CREATE DOMAIN s_str      AS VARCHAR(20)  CHECK (VALUE <> '');
CREATE DOMAIN m_str      AS VARCHAR(100) CHECK (VALUE <> '');
CREATE TYPE   cells_op   AS ENUM('Add', 'Sub', 'Mul', 'Div', 'Mod', 'Concat');
CREATE TYPE   scene_type AS ENUM('Menu', 'Settings', 'Quest', 'Victory', 'Defeat');
CREATE TYPE   cell       AS (value INTEGER, x cell_cord, y cell_cord);
CREATE DOMAIN grid       AS VARCHAR(318); -- CHECK (VALUE ~ '^(\.{0,2}\d+)+$');
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


-- TABLES

-- A user profile in which they saves their progress.
CREATE TABLE Profile (
    -- PRIMARY KEY attributes
	nickname      m_str      PRIMARY KEY,

    -- Other attributes
	creation      TIMESTAMP  NOT NULL
);

-- A gaming session of a player (i.e., from when they opened the game to when
-- they closed it).
CREATE TABLE Session (
    -- PRIMARY KEY attributes
    id            SERIAL     PRIMARY KEY,
    
    -- Other attributes
    player        m_str      NOT NULL,
	startstamp    TIMESTAMP  NOT NULL,

    -- CONSTRAINTS
    UNIQUE (player, startstamp),
	FOREIGN KEY (player) REFERENCES Profile(nickname) ON DELETE CASCADE
);

-- A game scene and its load time. It includes the Quest scene type.
CREATE TABLE Scene (
    -- PRIMARY KEY attributes
    session       INTEGER,
    ord           INTEGER,
    PRIMARY KEY (session, ord),

    -- Other attributes
    type_name     scene_type NOT NULL,
    startstamp    TIMESTAMP  NOT NULL,
    endstamp      TIMESTAMP  NOT NULL,
    max_wait      REAL       NOT NULL,
    avg_wait      REAL       NOT NULL,

    -- CONSTRAINTS
	FOREIGN KEY (session) REFERENCES Session(id) ON DELETE CASCADE,
    CONSTRAINT non_negative_ord CHECK (ord >= 0)
);

CREATE TABLE Quest (
	-- PRIMARY KEY attributes
	id            SERIAL     PRIMARY KEY,
	
	-- Other KEY
	scene_session INTEGER    NOT NULL,
	scene_ord     INTEGER    NOT NULL,
	UNIQUE (scene_session, scene_ord),
	
	-- Other attributes
	grid          grid       NOT NULL,
    goal          INTEGER    NOT NULL,
    hp            INTEGER    NOT NULL,
	result        INTEGER,
	
	-- CONSTRAINTS
	FOREIGN KEY (scene_session, scene_ord) REFERENCES Scene(session, ord)
);

-- An operation on two cells of a grid.
CREATE TABLE Operation (
    -- PRIMARY KEY attributes
    quest          INTEGER,
    ord            INTEGER,
    PRIMARY KEY (quest, ord),

    -- Other attributes
	op_type        cells_op NOT NULL, 
	primary_cell   cell     NOT NULL,
    secondary_cell cell     NOT NULL,

    -- CONSTRAINTS
    FOREIGN KEY (quest) REFERENCES Quest(id),
    CONSTRAINT non_negative_ord CHECK (ord >= 0)
);

COMMIT;
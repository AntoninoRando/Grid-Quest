BEGIN TRANSACTION;

DROP FUNCTION IF EXISTS add_user(m_str, TIMESTAMP);
DROP FUNCTION IF EXISTS add_user(m_str, BIGINT);
DROP FUNCTION IF EXISTS delete_user(m_str);
DROP FUNCTION IF EXISTS add_game_session(m_str, TIMESTAMP);
DROP FUNCTION IF EXISTS add_game_session(m_str, BIGINT);
DROP FUNCTION IF EXISTS add_game_scene;
DROP FUNCTION IF EXISTS add_quest;
DROP FUNCTION IF EXISTS last_profile_to_play;

-- Function to add a user to the database. It returns TRUE if the user was
-- successfully added, and FALSE if the user already exists. No errors are
-- thrown if the user already exists.
CREATE FUNCTION add_user(nick m_str, creation_date TIMESTAMP)
	RETURNS BOOLEAN
	LANGUAGE plpgsql
AS $$
DECLARE
    already_exists BOOLEAN;
BEGIN
    SELECT EXISTS(
        SELECT * 
        FROM Profile 
        WHERE nickname = nick)
	INTO already_exists;
    
    IF already_exists THEN
        RAISE NOTICE 'User % already exists', nick;
        RETURN FALSE;
    END IF;
    
    INSERT INTO Profile(nickname, creation)
    VALUES
        (nick, creation_date);
    
    RETURN TRUE;
END;
$$;

-- The overloaded version of 'add_user' in which the session start time is in
-- epoch (in milliseconds).
CREATE FUNCTION add_user(nick m_str, creation_epoch BIGINT)
	RETURNS BOOLEAN
	LANGUAGE plpgsql
AS $$
DECLARE
    creation_date TIMESTAMP;
    already_exists BOOLEAN;
BEGIN
    SELECT TO_TIMESTAMP(creation_epoch / 1000)  INTO creation_date;
    SELECT add_user(nick, creation_date) INTO already_exists;
    RETURN already_exists;
END;
$$;

-- Function to delete a user from the database, with all its associated data.
-- It returns TRUE if the user was successfully deleted, and FALSE if the user
-- does not exist. No errors are thrown if the user does not exist.
CREATE FUNCTION delete_user(nick m_str)
    RETURNS BOOLEAN
    LANGUAGE plpgsql
AS $$
DECLARE
    user_exists BOOLEAN;
BEGIN
    SELECT EXISTS(
        SELECT * 
        FROM Profile 
        WHERE nickname = nick) INTO user_exists;
    
    IF NOT user_exists THEN
        RAISE NOTICE 'User % does not exist', nick;
        RETURN FALSE;
    END IF;
        
    DELETE FROM Profile
    WHERE nickname = nick;
    
    RETURN TRUE;
END;
$$;

-- Function to add a game session of a user. It returns the session ID if the 
-- session was successfully added, and FALSE if it already exists No errors are 
-- thronw if the session already exists.
CREATE FUNCTION add_game_session(pl m_str, startt TIMESTAMP)
    RETURNS INT
    LANGUAGE plpgsql
AS $$
DECLARE
	session_id INT = -1;
BEGIN
    SELECT id
	FROM Session 
	WHERE player = pl AND startstamp = startt
	INTO session_id; 
    
    IF session_id <> -1 THEN
        RAISE NOTICE 'Session played by % at % already exists', pl, startt;
        RETURN session_id;
    END IF;

    INSERT INTO Session(player, startstamp)
    VALUES 
        (pl, startt)
	RETURNING id INTO session_id;
	RETURN session_id;
END;
$$;

-- The overloaded version of 'add_game_session' in which the session start time
-- is in epoch (in milliseconds).
CREATE FUNCTION add_game_session(pl m_str, start_epoch BIGINT)
    RETURNS INT
    LANGUAGE plpgsql
AS $$
DECLARE
	start_stamp TIMESTAMP;
	session_id  INT;
BEGIN
	SELECT TO_TIMESTAMP(start_epoch / 1000)  INTO start_stamp;
	SELECT add_game_session(pl, start_stamp) INTO session_id;
	RETURN session_id;
END;
$$;

CREATE FUNCTION add_game_scene(
    session_id  INT, 
    scene_order INT,
	stype       scene_type,
	start_epoch BIGINT,
    end_epoch   BIGINT,
	mw          REAL,
	aw          REAL
)
    RETURNS BOOLEAN
    LANGUAGE plpgsql
AS $$
DECLARE
    start_ts       TIMESTAMP;
    end_ts         TIMESTAMP;
	already_exists BOOLEAN;
BEGIN
    SELECT EXISTS(
        SELECT * 
        FROM Scene 
        WHERE session = session_id AND ord = scene_order)
    INTO already_exists;
    
    IF already_exists THEN
        RAISE NOTICE 
            '%° scene of the session with id % already exists',
            scene_order, session_id;
        RETURN FALSE;
    END IF;
	
    SELECT TO_TIMESTAMP(start_epoch / 1000) INTO start_ts;
    SELECT TO_TIMESTAMP(end_epoch   / 1000) INTO end_ts;

    INSERT INTO Scene(session, ord, type_name, startstamp, endstamp, max_wait, avg_wait)
    VALUES 
        (session_id, scene_order, stype, start_ts, end_ts, mw, aw);
	
	RETURN TRUE;
END;
$$;

-- Add a quest to the database. It returns TRUE if the quest was successfully
-- added, and FALSE if the quest already exists or if it could not be added due
-- to a missing dependency (e.g., the quest scene). No errors are thrown in case
-- of unsuccessful insert.
--
-- The function uses the player of the quest and the time it was played to find
-- the scene of the quest. If such a scene exists, the quest is added to the
-- database. Otherwise, the function returns FALSE, raising an INFO message.
CREATE FUNCTION add_quest(
    pplayer        m_str,
    quest_time     BIGINT,                         -- Epoch time in milliseconds
    quest_ord      INT,
    grid           grid,
    goal           INT,
    hp             INT,
    result         INT DEFAULT NULL
)
    RETURNS BOOLEAN
    LANGUAGE plpgsql
AS $$
DECLARE
    session_id     INT;
    scene_order    INT = NULL;
    already_exists BOOLEAN;
BEGIN
    -- Invalid quest order
    IF quest_ord < 0 THEN
        RAISE NOTICE 'Invalid quest order %: it must be >= 0', quest_ord;
        RETURN FALSE;
    END IF;

    -- Find the session in which this quest was played. Return False if that
    -- session does not exist.
    SELECT find_session(pplayer, quest_time) INTO session_id;

    IF session_id = -1 THEN
        RAISE NOTICE 
            'Player % were not playing at %, thus they have no quest',
            pplayer, TO_TIMESTAMP(quest_time / 1000);
        RETURN FALSE;
    END IF;

    -- Find the order of the scene that is this quest. Return False if that
    -- scene does not exist.
    SELECT ord
    INTO scene_order
    FROM Scene
    WHERE (session = session_id AND type_name = 'Quest')
    ORDER BY ord ASC
    LIMIT 1 OFFSET quest_ord;

    IF scene_order IS NULL THEN
        RAISE NOTICE 
            'Player % did not play a %th quest at %',
            pplayer, quest_ord, TO_TIMESTAMP(quest_time / 1000);
        RETURN FALSE;
    END IF;


    -- Check if this quest already exists.
    SELECT EXISTS (
        SELECT * 
        FROM Quest 
        WHERE scene_session = session_id AND scene_ord = scene_order)
    INTO already_exists;
    
    IF already_exists THEN
        RAISE NOTICE 
            'Quest of player % at % already exists',
            pplayer, TO_TIMESTAMP(quest_time / 1000);
        RETURN FALSE;
    END IF;
    
    -- Finally, add this quest.
    INSERT INTO Quest(scene_session, scene_ord,   grid, goal, hp, result)
    VALUES           (session_id,    scene_order, grid, goal, hp, result);
    
    RETURN TRUE;
END;
$$;

-- Find the last profile the user used to play a game before they quit.
CREATE FUNCTION last_profile_to_play()
    RETURNS m_str
    LANGUAGE plpgsql
AS $$
DECLARE
    no_player   BOOLEAN;
    no_session  BOOLEAN;
    last_player m_str;
BEGIN
    SELECT NOT EXISTS (                     -- Check at least one player exists.
        SELECT * 
        FROM Profile
        LIMIT 1
    ) INTO no_player;

    IF no_player THEN
        RAISE NOTICE 'No player has played the game yet';
        RETURN NULL;
    END IF;
    
    SELECT player                -- Find the player who played the last session.
    FROM Session
    WHERE startstamp = (
        SELECT MAX(startstamp)
        FROM Session
    ) INTO last_player;

    IF last_player IS NULL THEN    -- If no session was played, get last created
        SELECT nickname                                                -- player
		INTO last_player
        FROM Profile
        ORDER BY creation DESC
		LIMIT 1;
    END IF;
	
	RETURN last_player;
END;
$$;

COMMIT;
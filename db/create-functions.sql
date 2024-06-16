BEGIN TRANSACTION;


DROP FUNCTION IF EXISTS add_user;
DROP FUNCTION IF EXISTS add_game_session(m_str, TIMESTAMP);
DROP FUNCTION IF EXISTS add_game_session(m_str, BIGINT);
DROP FUNCTION IF EXISTS add_game_scene;


-- Function to add a user to the database. It returns TRUE if the user was
-- successfully added, and FALSE if the user already exists. No errors are
-- thrown if the user already exists.
CREATE FUNCTION add_user(nick m_str, creation_date DATE)
	RETURNS BOOLEAN
	LANGUAGE plpgsql
AS $$
DECLARE
    already_exists BOOLEAN;
BEGIN
    SELECT EXISTS(
        SELECT * 
        FROM Profile 
        WHERE nickname = nick) INTO already_exists;
    
    IF already_exists THEN
        RAISE INFO 'User % already exists', nick;
        RETURN FALSE;
    END IF;
    
    INSERT INTO Profile(nickname, creation)
    VALUES
        (nick, creation_date);
    
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
        RAISE INFO 'Session played by % at % already exists', pl, startt;
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
	session_id INT;
BEGIN
	SELECT TO_TIMESTAMP(start_epoch / 1000)  INTO start_stamp;
	SELECT add_game_session(pl, start_stamp) INTO session_id;
	RETURN session_id;
END;
$$;

COMMIT;

CREATE FUNCTION add_game_scene(
    session_id  INT, 
    scene_order INT,
	stype       scene_type,
	duration    BIGINT,
	mw          REAL,
	aw          REAL
)
    RETURNS BOOL
    LANGUAGE plpgsql
AS $$
DECLARE
	already_exists BOOL;
	dr_int INTERVAL;
BEGIN
    SELECT EXISTS(
        SELECT * 
        FROM Scene 
        WHERE player_session = session_id AND ord = scene_order)
    INTO already_exists;
    
    IF already_exists THEN
        RAISE INFO 
            '%° scene of the session with id % already exists',
            scene_order, session_id;
        RETURN FALSE;
    END IF;
	
	SELECT MAKE_INTERVAL(secs => duration) INTO dr_int;

    INSERT INTO Scene(player_session, ord, type_name, 
                      time_in, max_wait, avg_wait)
    VALUES 
        (session_id, scene_order, stype, dr_int, mw, aw);
	
	RETURN TRUE;
END;
$$;

COMMIT;
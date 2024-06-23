BEGIN TRANSACTION;

DROP FUNCTION IF EXISTS quest_played;
DROP FUNCTION IF EXISTS quest_won;

-- Find how many quest a player has played.
CREATE FUNCTION quest_played(player_nick m_str)
    RETURNS  INT
    LANGUAGE plpgsql
AS $$
DECLARE
    result INT;
BEGIN
    SELECT COUNT(*)
    INTO result
    FROM Quest q, Session s
    WHERE q.scene_session = s.id AND s.player = player_nick;

    RETURN result;
END;
$$;

-- Find how many quest a player has won.
CREATE FUNCTION quest_won(player_nick m_str)
    RETURNS  INT
    LANGUAGE plpgsql
AS $$
DECLARE
    result INT;
BEGIN
    SELECT COUNT(*)
    INTO result
    FROM Quest q, Session s
    WHERE q.scene_session = s.id AND s.player = player_nick
          AND q.result =  q.goal;
    
    RETURN result;
END;
$$;


COMMIT;
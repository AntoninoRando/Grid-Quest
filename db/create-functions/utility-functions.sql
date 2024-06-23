BEGIN TRANSACTION;

DROP FUNCTION IF EXISTS find_session;

-- Find a session of a player that token place inside a range of time that 
-- includes 'start_epoch' to timestamp. Returns session ID if the session was
-- found, -1 otherwise. 
CREATE FUNCTION find_session(
    pplayer     m_str,
    time_epoch  BIGINT                             -- Epoch time in milliseconds
)
    RETURNS  INT
    LANGUAGE plpgsql
AS $$
DECLARE
    session_id INT = -1;
    time_ts    TIMESTAMP;
BEGIN
    -- Epoch to timestamp.
    SELECT TO_TIMESTAMP(time_epoch / 1000) INTO time_ts;

    SELECT s.id
    INTO session_id
    FROM Session s
    WHERE (
        s.player = pplayer
        AND s.startstamp <= time_ts
        AND EXISTS (
             SELECT * 
             FROM Scene sc
             WHERE sc.session = s.id AND sc.endstamp >= time_ts))
    ORDER BY s.startstamp DESC
    LIMIT 1;

    RETURN session_id;
END;
$$;


COMMIT;
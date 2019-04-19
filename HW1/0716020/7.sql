SELECT COUNT(DISTINCT ps.Id) AS numberOfPlayers, MAX(ps.heals) AS maxHeals
FROM player_statistic ps
WHERE ps.damageDealt = 0 AND ps.winPlacePerc = 1 AND ps.matchId IN (
	SELECT m.matchId
	FROM `match` m
	WHERE m.matchDuration > (
		SELECT AVG(mt.matchDuration)
		FROM `match` mt
	)
);

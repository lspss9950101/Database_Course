SELECT count(*) AS numberOfPlayers, MAX(heals) AS maxHeals
FROM player_statistic
WHERE matchId IN (
	SELECT matchId
	FROM `match`
	WHERE matchDuration > (
		SELECT AVG(matchDuration)
		FROM `match`
	)
) AND damageDealt = 0 AND winPlacePerc = 1;

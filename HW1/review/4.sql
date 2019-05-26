SELECT Id, AVG(kills) AS avgKills
FROM player_statistic
WHERE matchId in (
	SELECT matchId
	FROM `match`
	WHERE numGroups <= 10
)
GROUP BY Id
ORDER BY AVG(kills) DESC
LIMIT 20;

SELECT ps.Id, AVG(ps.kills) AS avgKills
FROM player_statistic ps
WHERE ps.matchId IN (
	SELECT m.matchId
	FROM `match` m
	WHERE m.numGroups <= 10
)
GROUP BY ps.Id
ORDER BY AVG(ps.kills) DESC
LIMIT 20;

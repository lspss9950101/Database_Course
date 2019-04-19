SELECT psg.teamRoadKills AS teamRoadKills, AVG(psg.winPlacePerc) AS avgWinPlacePerc
FROM (
	SELECT SUM(ps.roadKills) AS teamRoadKills, MAX(ps.winPlacePerc) AS winPlacePerc
	FROM player_statistic ps
	WHERE ps.matchId IN (
		SELECT m.matchId
		FROM `match` m
		WHERE m.matchType LIKE "squad%"
	)
	GROUP BY ps.groupId
) AS psg
GROUP BY psg.teamRoadKills
ORDER BY psg.teamRoadKills DESC;

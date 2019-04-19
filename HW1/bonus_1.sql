SELECT psg.Id, psg.headshotKills, psg.kills, psg.headshotRatio
FROM(
	SELECT psa.Id AS Id, psa.headshotKills AS headshotKills, psa.kills AS kills, psa.headshotKills / psa.kills AS headshotRatio
	FROM (
		SELECT ps.Id AS Id, SUM(ps.headshotKills) AS headshotKills, SUM(ps.kills) AS kills
		FROM player_statistic ps
		WHERE ps.kills >= 5
		GROUP BY ps.Id
	) AS psa
	WHERE psa.kills >= 20
) AS psg
ORDER BY psg.headshotRatio DESC
LIMIT 100;

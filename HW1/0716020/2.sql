SELECT ps.Id, ps.matchId, ps.damageDealt
FROM player_statistic ps
WHERE ps.damageDealt BETWEEN 2000 and 2010;

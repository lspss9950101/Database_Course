SELECT ps.walkDistance + ps.swimDistance + ps.rideDistance AS totalDistance, ps.walkDistance, ps.swimDistance, ps.rideDistance
FROM player_statistic ps
where ps.walkDistance != 0 AND ps.swimDistance != 0 AND ps.rideDistance != 0
ORDER BY ps.walkDistance + ps.swimDistance + ps.rideDistance DESC
LIMIT 10;

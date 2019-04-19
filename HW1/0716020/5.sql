SELECT m.matchType, AVG(m.matchDuration) AS averageDuration
FROM `match` m
GROUP BY m.matchType
ORDER BY AVG(m.matchDuration);

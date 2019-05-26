SELECT matchType, AVG(matchDuration) AS averageDuration
FROM `match`
GROUP BY matchType
ORDER BY AVG(matchDuration);

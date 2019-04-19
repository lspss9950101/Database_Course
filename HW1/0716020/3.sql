SELECT m.matchType, COUNT(*) AS `count`
FROM `match` m
WHERE m.matchType LIKE "%fpp"
GROUP BY m.matchType
ORDER BY COUNT(*);

SELECT fpp.matchType, fpp.count
FROM(
	SELECT matchType, count(*) as count
	FROM `match`
	WHERE matchType LIKE "%fpp"
	GROUP BY matchType
) as fpp
ORDER BY fpp.count;

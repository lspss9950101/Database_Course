# MySQL Basic Command #
### Create database ###
	CREATE DATABASE database_name;
___
### Change database ###
	USE database_name;
___
### Create table ###
	CREATE TABLE table_name(
	column_name_1 data_type,
	column_name_2 data_type,
	column_name_3 data_type,
	...
	);
___
### Load data from csv file ###
	LOAD DATA LOCAL INFILE "filename"
	INTO TABLE "table_name"
	FIELDS TERMINATED BY ','
	ENCLOSED BY '"'
	LINES TERMINATED BY '\n'
	IGNORE 1 ROWS;

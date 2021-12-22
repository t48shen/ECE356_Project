-- 1. Create tables to hold data from .csv files (original, unmodified)

CREATE TABLE OG_london_crime_by_lsoa (
	lsoa_code VARCHAR(9),
	borough VARCHAR(255),
	major_category VARCHAR(255),
	minor_category VARCHAR(255),
	value DECIMAL(1, 0),
	year DECIMAL(4, 0),
	month DECIMAL(2, 0)
);

CREATE TABLE OG_london_outcomes (				-- added underscores (_) in field names, replaces "-"
	Crime_ID VARCHAR(64),
	Month VARCHAR(7),				-- format: YYYY-MM (convert to DATETIME?)
	Reported_by VARCHAR(255),
	Falls_within VARCHAR(255),
	Longitude DECIMAL(8,6),
	Latitude DECIMAL(9,6),
	Location VARCHAR(255),
	LSOA_code VARCHAR(9),
	LSOA_name VARCHAR(255),
	Outcome_type VARCHAR(255)
);

CREATE TABLE OG_london_stop_and_search (				-- added underscores (_) in field names, replaces "-"
	Type VARCHAR(255),
	Date VARCHAR(25),					-- format: YYYY-MM-DDTHH:MM:SS+00:00 (convert to DATETIME?)
	Part_of_a_policing_operation VARCHAR(5),			-- mysql doesnt have Boolean, NULL data (60%) = assume true?
	Policing_operation VARCHAR(255),			-- NULL data (100%)
	Latitude DECIMAL(8,6),					-- NULL data (63%)
	Longitude DECIMAL(9,6),					-- NULL data (63%)
	Gender VARCHAR(255),					-- Male, Female, other?
	Age_Range VARCHAR(255),					-- ##-##
	Self_defined_ethnicity VARCHAR(255),
	Officer_defined_ethnicity VARCHAR(255),	
	Legislation VARCHAR(255),	
	Object_of_search VARCHAR(255),				-- NULL data (29%)
	Outcome VARCHAR(255),	
	Outcome_linked_to_object_of_search VARCHAR(5),		-- NULL data (99%)
	Removal_of_more_than_just_outer_clothing VARCHAR(5)	-- NULL data (99%)
);

CREATE TABLE OG_london_street (				-- added underscores (_) in field names, replaces "-"
	Crime_ID VARCHAR(64),				-- NULL data (24%) - ASB
	Month VARCHAR(7),				-- format: YYYY-MM (convert to DATETIME?)
	Reported_by VARCHAR(255),
	Falls_within VARCHAR(255),
	Longitude DECIMAL(8,6),
	Latitude DECIMAL(9,6),
	Location VARCHAR(255),
	LSOA_code VARCHAR(9),
	LSOA_name VARCHAR(255),
	Crime_type VARCHAR(255),
	Last_outcome_category VARCHAR(255),		-- NULL data (24%) - ASB
	Context VARCHAR(255)				-- NULL data (100%)
);

CREATE TABLE OG_Output_Area (				-- renamed, shortened
	OA11CD VARCHAR(9),
	LAD16CD VARCHAR(9),
	LAD16NM VARCHAR(255),
	LSOA11CD VARCHAR(9),
	LSOA11NM VARCHAR(255),
	MSOA11CD VARCHAR(9),
	MSOA11NM VARCHAR(255),
	LEP17CD1 VARCHAR(9),
	LEP17NM1 VARCHAR(255),
	LEP17CD2 VARCHAR(9),				-- NULL data (92%)
	LEP17NM2 VARCHAR(255),				-- NULL data (92%)
	FID DECIMAL(6,0)				-- 171k valid
);



-- 2. Load data from .csv files into created tables

SET SESSION sql_mode = '';				-- added for missing DECIMAL values, set back?

LOAD DATA INFILE '/var/lib/mysql-files/10-Crime/UKCrime/london_crime_by_lsoa.csv'
INTO TABLE OG_london_crime_by_lsoa
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA INFILE '/var/lib/mysql-files/10-Crime/UKCrime/london-outcomes.csv'
INTO TABLE OG_london_outcomes
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA INFILE '/var/lib/mysql-files/10-Crime/UKCrime/london-stop-and-search.csv'
INTO TABLE OG_london_stop_and_search
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA INFILE '/var/lib/mysql-files/10-Crime/UKCrime/london-street.csv'
INTO TABLE OG_london_street
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA INFILE '/var/lib/mysql-files/10-Crime/UKCrime/Output_Area_to_Local_Authority_District_to_Lower_Layer_Super_Output_Area_to_Middle_Layer_Super_Output_Area_to_Local_Enterprise_Partnership_April_2017_Lookup_in_England_V2.csv'
INTO TABLE OG_Output_Area
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;



-- 3. Create schema matching ERD (all new entity sets, relationship sets)

-- london_crime

CREATE TABLE london_crime (
	offense_ID int NOT NULL AUTO_INCREMENT,
	crime_ID VARCHAR(64),
	month VARCHAR(7),
	reported_by VARCHAR(255),
	falls_within VARCHAR(255),
	latitude DECIMAL(9,6),
	longitude DECIMAL(8,6),
	location VARCHAR(255),
	lsoa_code VARCHAR(9),

	PRIMARY KEY(offense_ID)
);

	-- insert from OG_london_outcomes
INSERT INTO london_crime (crime_ID, month, reported_by, falls_within, latitude, longitude, location, lsoa_code) SELECT
	Crime_ID,
	Month,
	Reported_by,
	Falls_within,
	Latitude,
	Longitude,
	Location,
	LSOA_code
FROM OG_london_outcomes;

	-- insert from OG_london_street
INSERT INTO london_crime (crime_ID, month, reported_by, falls_within, latitude, longitude, location, lsoa_code) SELECT
	Crime_ID,
	Month,
	Reported_by,
	Falls_within,
	Latitude,
	Longitude,
	Location,
	LSOA_code
FROM OG_london_street;


-- london_outcomes (OVERLAPPING GENERALIZATION)

CREATE TABLE london_outcomes (
	offense_ID int NOT NULL AUTO_INCREMENT,
	outcome_type VARCHAR(255),
	
	FOREIGN KEY(offense_ID) REFERENCES london_crime(offense_ID)
);

INSERT INTO london_outcomes (outcome_type) SELECT
	Outcome_type
FROM OG_london_outcomes;


-- london_street (OVERLAPPING GENERALIZATION)

CREATE TABLE london_street (
	offense_ID int NOT NULL AUTO_INCREMENT,
	crime_type VARCHAR(255),
	last_outcome_category VARCHAR(255),

	FOREIGN KEY(offense_ID) REFERENCES london_crime(offense_ID)
);

	-- #rows in OG_london_outcomes + 19001 (matches offense_ID in london_crime)
ALTER TABLE london_street AUTO_INCREMENT=1966051;

INSERT INTO london_street (crime_type, last_outcome_category) SELECT
	Crime_type,
	Last_outcome_category
FROM OG_london_street;


-- london_crime_by_lsoa (need to create PK)

CREATE TABLE london_crime_by_lsoa (
	crime_by_lsoa_ID int NOT NULL AUTO_INCREMENT,
	lsoa_code VARCHAR(9),
	borough VARCHAR(255),
	major_category VARCHAR(255),
	minor_category VARCHAR(255),
	value DECIMAL(1, 0),
	year DECIMAL(4, 0),
	month DECIMAL(2, 0),

	PRIMARY KEY(crime_by_lsoa_ID)
);

INSERT INTO london_crime_by_lsoa (lsoa_code, borough, major_category, minor_category, value, year, month) SELECT
	lsoa_code,
	borough,
	major_category,
	minor_category,
	value,
	year,
	month
FROM OG_london_crime_by_lsoa;


-- LSOA_lookup (remove irrelevant fields, remove duplicate LSOA11CD?)

CREATE TABLE LSOA_lookup (
	LSOA11CD VARCHAR(9),
	LSOA11NM VARCHAR(255),

	PRIMARY KEY(LSOA11CD)
);

INSERT IGNORE INTO LSOA_lookup (LSOA11CD, LSOA11NM) SELECT
	LSOA11CD,
	LSOA11NM
FROM OG_Output_Area;

SET FOREIGN_KEY_CHECKS=0;

	-- add FK references from london_crime, london_crime_by_lsoa (created earlier)
ALTER TABLE london_crime ADD FOREIGN KEY(lsoa_code) REFERENCES LSOA_lookup(LSOA11CD);
ALTER TABLE london_crime_by_lsoa ADD FOREIGN KEY(lsoa_code) REFERENCES LSOA_lookup(LSOA11CD);

SET FOREIGN_KEY_CHECKS=1;


-- setting (need to create PK)

CREATE TABLE setting (
	setting_ID int NOT NULL AUTO_INCREMENT,
	date VARCHAR(25),
	latitude DECIMAL(8,6),
	longitude DECIMAL(9,6),

	PRIMARY KEY(setting_ID)
);

INSERT INTO setting (date, latitude, longitude) SELECT
	Date,
	Latitude,
	Longitude
FROM OG_london_stop_and_search;


-- person (need to create PK)

CREATE TABLE person (
	person_ID int NOT NULL AUTO_INCREMENT,
	gender VARCHAR(255),
	age_range VARCHAR(255),
	self_defined_ethnicity VARCHAR(255),
	officer_defined_ethnicity VARCHAR(255),

	PRIMARY KEY(person_ID)
);

INSERT INTO person (gender, age_range, self_defined_ethnicity, officer_defined_ethnicity) SELECT
	Gender,
	Age_Range,
	Self_defined_ethnicity,
	Officer_defined_ethnicity
FROM OG_london_stop_and_search;


-- investigation (need to create PK)

CREATE TABLE investigation (
	investigation_ID int NOT NULL AUTO_INCREMENT,	
	part_of_a_policing_operation VARCHAR(5),
	legislation VARCHAR(255),
	
	PRIMARY KEY(investigation_ID)
);

INSERT INTO investigation (part_of_a_policing_operation, legislation) SELECT
	Part_of_a_policing_operation,
	Legislation
FROM OG_london_stop_and_search;


-- stop_and_search (RELATIONSHIP SET)

SET FOREIGN_KEY_CHECKS=0;

CREATE TABLE stop_and_search (
	setting_ID int NOT NULL AUTO_INCREMENT,
	person_ID int NOT NULL DEFAULT '0',
	investigation_ID int NOT NULL DEFAULT '0',

	date VARCHAR(25),
	latitude DECIMAL(8,6),
	longitude DECIMAL(9,6),
	gender VARCHAR(255),
	age_range VARCHAR(255),
	self_defined_ethnicity VARCHAR(255),
	officer_defined_ethnicity VARCHAR(255),
	part_of_a_policing_operation VARCHAR(5),
	legislation VARCHAR(255),

	type VARCHAR(255),
	object_of_search VARCHAR(255),
	outcome VARCHAR(255),

	PRIMARY KEY(setting_ID, person_ID, investigation_ID),
	FOREIGN KEY(setting_ID) REFERENCES setting(setting_ID),
	FOREIGN KEY(person_ID) REFERENCES person(person_ID),
	FOREIGN KEY(investigation_ID) REFERENCES investigation(investigation_ID)
);

INSERT INTO stop_and_search (
	date,
	latitude,
	longitude,
	gender,
	age_range,
	self_defined_ethnicity,
	officer_defined_ethnicity,
	part_of_a_policing_operation,
	legislation,
	type,
	object_of_search,
	outcome
) SELECT
	Date,
	Latitude,
	Longitude,
	Gender,
	Age_Range,
	Self_defined_ethnicity,
	Officer_defined_ethnicity,
	Part_of_a_policing_operation,
	Legislation,
	Type,
	Object_of_search,
	Outcome
FROM OG_london_stop_and_search;

UPDATE `stop_and_search` SET person_ID = setting_ID;
UPDATE `stop_and_search` SET investigation_ID = setting_ID;

SET FOREIGN_KEY_CHECKS=1;

-- 4. END FILE
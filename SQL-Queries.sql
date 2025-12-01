SELECT * FROM dbo.Conversions;

CREATE TABLE Conversions (
     ID         INT IDENTITY(1, 1) PRIMARY KEY,
     Fahrenheit FLOAT NOT NULL,
     Celsius    FLOAT NOT NULL,
);

INSERT INTO dbo.Conversions (Fahrenheit, Celsius) VALUES (50, 10);

DROP TABLE dbo.Conversions;
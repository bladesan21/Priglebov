CREATE TABLE Users (
    user_id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    surname VARCHAR(100) NOT NULL,
    phone VARCHAR(20) NULL,
    email VARCHAR(255) NULL
);

CREATE TABLE Categories (
    category_id SERIAL PRIMARY KEY,
    house BOOLEAN,
    flat BOOLEAN,
    townhouse BOOLEAN,
    summer_house BOOLEAN
);

CREATE TABLE Locations (
    location_id SERIAL PRIMARY KEY,
    country VARCHAR(100) NOT NULL,
    region VARCHAR(100) NULL,
    locality VARCHAR(100) NOT NULL,
    street VARCHAR(100) NOT NULL,
    house_number VARCHAR(20) NOT NULL
);

CREATE TABLE Real_estates (
    real_estate_id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES Users(user_id) ON DELETE CASCADE,
    price NUMERIC(15, 2) NOT NULL,
    area NUMERIC(10, 2) NOT NULL,
    category_id INTEGER REFERENCES Categories(category_id),
    location_id INTEGER REFERENCES Locations(location_id)
);

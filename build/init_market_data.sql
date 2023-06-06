CREATE DATABASE market_data;

\c market_data;

CREATE TYPE event AS ENUM (
    'order',
    'trade',
    'settlement'
);

CREATE TYPE status AS ENUM (
    'confirmed',
    'denied'
);

CREATE TABLE prices (
    id SERIAL PRIMARY KEY,
    security_id VARCHAR(50),
    bid NUMERIC(10,2),
    ask NUMERIC(10,2)
);

CREATE TABLE reports (
    id SERIAL PRIMARY KEY,
    event EVENT,
    status STATUS,
    order_id INTEGER,
    client_id VARCHAR(50),
    security_id VARCHAR(50),
    side VARCHAR(10),
    quantity INTEGER,
    price NUMERIC(18,2),
    reason VARCHAR(50)
);

CREATE DATABASE clearinghouse;

\c clearinghouse;

CREATE TABLE account_balances (
    id SERIAL PRIMARY KEY,
    client_id VARCHAR(255) NOT NULL UNIQUE,
    balance NUMERIC(18,2) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE securities_deposit (
    id SERIAL PRIMARY KEY,
    security_id VARCHAR(255) NOT NULL,
    client_id VARCHAR(255) NOT NULL,
    quantity INTEGER NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

INSERT INTO account_balances (client_id, balance)
VALUES ('DTCC', 900000), ('MARKET_MAKER', 40000), ('FLOOD', 10000), ('GAUSS', 10000), ('RANDOM_WALK', 10000), ('RETAIL', 10000), ('UNIFORM', 10000), ('SINGLE', 10000);

INSERT INTO securities_deposit (security_id, client_id, quantity)
VALUES
    ('TSLA', 'DTCC', 100000),
    ('AAPL', 'DTCC', 100000),
    ('JPM', 'DTCC', 100000),
    ('SHEL', 'DTCC', 100000),
    ('MSFT', 'DTCC', 90000),
    ('MSFT', 'MARKET_MAKER', 4000),
    ('MSFT', 'FLOOD', 1000),
    ('MSFT', 'GAUSS', 1000),
    ('MSFT', 'RANDOM_WALK', 1000),
    ('MSFT', 'RETAIL', 1000),
    ('MSFT', 'SINGLE', 1000);
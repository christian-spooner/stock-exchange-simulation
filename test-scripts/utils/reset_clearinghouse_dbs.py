import psycopg2

conn = psycopg2.connect(
    host="localhost",
    port=5500,
    database="clearinghouse",
    user="postgres",
    password="postgres",
)

cursor = conn.cursor()

cursor.execute("TRUNCATE TABLE account_balances")
entries = [
    ("DTCC", 960000),
    ("MARKET_MAKER", 10000),
    ("RANDOM_WALK", 10000),
    ("GAUSS", 10000),
    ("RETAIL", 10000),
]
sql = """
    INSERT INTO account_balances (client_id, balance)
    VALUES (%s, %s)
"""
try:
    for entry in entries:
        cursor.execute(sql, entry)
except (Exception, psycopg2.Error) as error:
    print("Error while replicating entries:", error)


cursor.execute("TRUNCATE TABLE securities_deposit")
entries = [
    ("TSLA", "DTCC", 100000),
    ("AAPL", "DTCC", 100000),
    ("JPM", "DTCC", 100000),
    ("SHEL", "DTCC", 100000),
    ("MSFT", "DTCC", 96000),
    ("MSFT", "MARKET_MAKER", 1000),
    ("MSFT", "RANDOM_WALK", 1000),
    ("MSFT", "GAUSS", 1000),
    ("MSFT", "RETAIL", 1000),
]
sql = """
    INSERT INTO securities_deposit (security_id, client_id, quantity)
    VALUES (%s, %s, %s)
"""
try:
    for entry in entries:
        cursor.execute(sql, entry)
except (Exception, psycopg2.Error) as error:
    print("Error while replicating entries:", error)

conn.commit()
cursor.close()
conn.close()

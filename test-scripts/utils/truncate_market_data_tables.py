import psycopg2

conn = psycopg2.connect(
    host="localhost",
    port=5500,
    database="market_data",
    user="postgres",
    password="postgres",
)

cursor = conn.cursor()

cursor.execute("TRUNCATE TABLE reports")
cursor.execute("TRUNCATE TABLE prices")

conn.commit()
cursor.close()
conn.close()

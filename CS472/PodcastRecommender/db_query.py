## This script is used to query the database
import os
import psycopg2
from dotenv import load_dotenv

load_dotenv()

CONNECTION = ''

conn = psycopg2.connect(CONNECTION)

conn.autocommit = True
cursor = conn.cursor()

sql = '''SELECT * FROM podcast'''

cursor.execute(sql)
results = cursor.fetchall()
print("Contents from podcast:")
print(results)

sql = '''SELECT * FROM podcast_segment'''

cursor.execute(sql)
results = cursor.fetchall()
print("Contents of podcast_segment:")
print(results)

conn.commit()
conn.close()
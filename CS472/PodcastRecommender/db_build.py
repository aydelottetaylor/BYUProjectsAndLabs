import os
from dotenv import load_dotenv
import psycopg2

load_dotenv()

CONNECTION = ''

CREATE_EXTENSION = "CREATE EXTENSION vector"

CREATE_PODCAST_TABLE = """
CREATE TABLE podcast (
    podcast_id TEXT PRIMARY KEY,
    title TEXT 
);
"""

CREATE_SEGMENT_TABLE = """
    CREATE TABLE podcast_segment (
        id VARCHAR(10) PRIMARY KEY,
        podcast_id VARCHAR(20),
        start_time DOUBLE PRECISION,
        end_time DOUBLE PRECISION,
        embedding VECTOR(128),
        content VARCHAR(10),
        FOREIGN KEY (podcast_id) REFERENCES podcast(podcast_id)
    );
"""

conn = psycopg2.connect(CONNECTION)
conn.autocommit = True
cursor = conn.cursor()

# cursor.execute(CREATE_EXTENSION)
cursor.execute(CREATE_PODCAST_TABLE)
cursor.execute(CREATE_SEGMENT_TABLE)

conn.commit()
conn.close()

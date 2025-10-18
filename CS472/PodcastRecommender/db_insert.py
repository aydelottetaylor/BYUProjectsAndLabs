## This script is used to insert data into the database
import os
import json
from dotenv import load_dotenv
from datasets import load_dataset
import pandas as pd

from utils import fast_pg_insert

CONNECTION = ''
DOCUMENTS_PATH = 'documents'
EMBEDDINGS_PATH = 'embedding'

def read_jsonl(filepath: str) -> pd.DataFrame:
    with open(filepath, 'r', encoding='utf-8') as f:
        return pd.DataFrame([json.loads(line) for line in f])

embedding_files = [os.path.join(EMBEDDINGS_PATH, f) for f in os.listdir(EMBEDDINGS_PATH) if f.endswith('.jsonl')]
embedding_dfs = [read_jsonl(f) for f in embedding_files]
embedding_df = pd.concat(embedding_dfs, ignore_index=True)

document_files = [os.path.join(DOCUMENTS_PATH, f) for f in os.listdir(DOCUMENTS_PATH) if f.endswith('.jsonl')]
document_dfs = [read_jsonl(f) for f in document_files]
segment_df = pd.concat(document_dfs, ignore_index=True)

ds = load_dataset('Whispering-GPT/lex-fridman-podcast')
podcast_df = pd.DataFrame(ds['train'])
final_podcast_df = podcast_df[['id', 'title']]

# Load all of the data for podcast_segment into a dataframe
final_segment_df = pd.DataFrame()
final_segment_df['podcast_id'] = segment_df['body'].apply(lambda x: x['metadata']['podcast_id'])
final_segment_df['start_time'] = segment_df['body'].apply(lambda x: x['metadata']['start_time'])
final_segment_df['end_time'] = segment_df['body'].apply(lambda x: x['metadata']['stop_time'])
final_segment_df['embedding'] = embedding_df['response'].apply(lambda x: x['body']['data'][0]['embedding'])
final_segment_df = final_segment_df.merge(
    podcast_df[['id', 'description']].rename(columns={'description': 'content'}),
    left_on='podcast_id',
    right_on='id',
    how='left'
)
final_segment_df.drop(columns=['id'], inplace=True)
final_segment_df['id'] = segment_df['custom_id']
final_segment_df = final_segment_df[['id'] + [col for col in final_segment_df.columns if col != 'id']]

print(final_segment_df)

print(final_segment_df.loc[(final_segment_df['podcast_id'] == 'U_AREIyd0Fc') & (final_segment_df['start_time'] == 484.52) & (final_segment_df['end_time'] == 487.08)])


    
# TODO: Insert into postgres
# HINT: use the recommender.utils.fast_pg_insert function to insert data into the database
# otherwise inserting the 800k documents will take a very, very long time
# fast_pg_insert(final_podcast_df, CONNECTION, 'podcast', ['podcast_id', 'title'])
fast_pg_insert(final_segment_df, CONNECTION, 'podcast_segment', ['id', 'podcast_id', 'start_time', 'end_time', 'embedding', 'content'])

print('Data insertion completed successfully!')
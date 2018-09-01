import click
from flask import current_app, g
from flask.cli import with_appcontext

from pymongo import MongoClient, ASCENDING, DESCENDING
from pymongo.errors import DuplicateKeyError
from bson.objectid import ObjectId
from bson.errors import InvalidId
from sys import maxsize
from datetime import datetime
# from os import environ

#-------------------------added by Yucheng
import json
from skbm.config import cfg
from os import path as osp

def get_db():
    if 'db' not in g:
        g.db = MongoClient(current_app.config['DATABASE'])['sketch']
    return g.db

def close_db(e=None):
    db = g.pop('db', None)
    if db is not None:
        db.client.close()

def init_db():
    db = get_db()
    db.client.drop_database('sketch')

@click.command('init-db')
@with_appcontext
def init_db_command():
    init_db()
    init_existing_dataset()
    init_existing_sketch()
    click.echo('Initialized the mongo database.')

def init_app(app):
    app.teardown_appcontext(close_db)
    app.cli.add_command(init_db_command)

def init_existing_dataset():
    db = get_db()
    db.drop_collection('dataset_info')
    from pathlib import Path
    iter_dataset = Path(cfg.PATH.dataset_dir).glob('*')
    lst = []
    for d in iter_dataset:
        lst.append({
                'name': d.name,
                'path': str(d),
            })
    db.dataset_info.insert_many(lst)
    print('Initiated existing datasets!')

def init_existing_sketch():
    db = get_db()
    db.drop_collection('sketch_info')
    info = [
        {
            'name': 'CmSketch',
            'path': osp.join(cfg.PATH.sketch_dir, 'CmSketch.h'),
            'params': [
                    {
                        'field': 'hash_num',
                        'type': 'int',
                        'help': 'xxx',
                    },
                    {
                        'field': 'bit_per_counter',
                        'type': 'int',
                        'help': 'xxx',
                    },
                    {
                        'field': 'counter_per_array',
                        'type': 'int',
                        'help': 'xxx',
                    },
                ],
            'tasks': ['freq', 'topk'],
        },
    ]
    db.sketch_info.insert_many(info);

















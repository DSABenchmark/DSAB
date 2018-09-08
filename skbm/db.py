import click
from flask import current_app, g
from flask.cli import with_appcontext

from pymongo import MongoClient, ASCENDING, DESCENDING
from pymongo.errors import DuplicateKeyError
from bson.objectid import ObjectId
from bson.errors import InvalidId
from sys import maxsize
from datetime import datetime

from pprint import pprint
# from os import environ

#-------------------------added by Yucheng
import json
from skbm.config import cfg
from os import path as osp
import subprocess
import numpy as np

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
    # info = [
    #     {
    #         'name': 'CmSketch',
    #         'path': osp.join(cfg.PATH.sketch_dir, 'sketch', 'CmSketch.h'),
    #         'params': [
    #                 {
    #                     'field': 'hash_num',
    #                     'type': 'int',
    #                     'help': 'xxx',
    #                 },
    #                 {
    #                     'field': 'bit_per_counter',
    #                     'type': 'int',
    #                     'help': 'xxx',
    #                 },
    #                 {
    #                     'field': 'counter_per_array',
    #                     'type': 'int',
    #                     'help': 'xxx',
    #                 },
    #             ],
    #         'tasks': [
    #             {
    #                 'name': 'freq',
    #                 'params': [],
    #             },
    #             {
    #                 'name': 'topk',
    #                 'params': [{
    #                     'field': 'k',
    #                     'type': 'int',
    #                     'help': 'xxx',
    #                 }],
    #             },
    #             {
    #                 'name': 'speed',
    #                 'params': [],
    #             },
    #         ],
    #     },
    # ]
    # db.sketch_info.insert_many(info);
    print('Initiated existing sketches!')

def query_results(args_per_dataset):
    db = get_db()
    results = []
    args_to_experiment = []
    for arg in args_per_dataset:
        result = db.experiment.find_one(arg)
        if result:
            result.pop('_id')
        if result:
            results.append(result)
        else :
            args_to_experiment.append(arg)
    # pprint(args_to_experiment)
    if arg['taskName'] == 'topk':
        alltasks = ['topk','speed','freq']
    else :
        alltasks = ['freq', 'speed']
    cmd_args = set()
    for arg in args_to_experiment:
        lst = [
            arg['datasetName'],
            arg['sketchName'],
        ]
        lst += alltasks
        kvpairsList = []
        for key in arg:
            if key not in ['datasetName', 'sketchName', 'taskName']:
                kvpairsList.append(key+'='+str(arg[key]))
        kvpairsList.sort()
        lst += kvpairsList
        cmd_args.add('+'.join(lst))
        arg['output_filename'] = (
                arg['datasetName'] + '+' +
                arg['sketchName'] + '+' +
                arg['taskName'] + '+' +
                '+'.join(kvpairsList)
            )
    cmd_args = list(cmd_args)
    # cmd = '{} {}'.format(cfg.PATH.execute_file, ' '.join(cmd_args))
    # pprint(cmd)
    # pprint(list(map(lambda arg: arg['output_filename'], args_to_experiment)))
    # print("start experiments!")
    # p = subprocess.Popen(cmd, shell=True, stderr=subprocess.STDOUT)
    # p.wait()
    # print("finished all experiments!")
    for cmd_arg in cmd_args:
        cmd = '{} {}'.format(cfg.PATH.execute_file, cmd_arg)
        print('start ',cmd)
        p = subprocess.Popen(cmd, shell=True, stderr=subprocess.STDOUT)
        p.wait()
        print("finished ",cmd)
        print('-'*10)

    for arg in args_to_experiment:
        # lst = [
        #     arg['datasetName'],
        #     arg['sketchName'],
        #     arg['taskName'],
        # ]
        # for key in arg:
        #     if key not in ['datasetName', 'sketchName', 'taskName','output_filename']:
        #         lst.append(key+'='+str(arg[key]))
        # resultFile = '+'.join(lst)
        resultFile = arg['output_filename']
        result = parseResultFile(resultFile, arg)
        # pprint(result)
        result = eval(str(result))
        db.experiment.insert_one(result.copy())
        results.append(result)

    return results

def parseResultFile(filename, arg):
    result = arg.copy()
    taskName = arg['taskName']
    if taskName == 'speed':
        with open(osp.join(cfg.PATH.output_dir, filename)) as hd:
            line = hd.read()
            lst = line.strip().split()
            result['taskResult'] = {
                'totalNum': int(lst[0]),
                'time': double(lst[1]),
            }
    elif taskName == 'freq':
        with open(osp.join(cfg.PATH.output_dir, filename)) as hd:
            trueValueList, estimatedValueList = [],[]
            for line in hd:
                trueValue, estimatedValue = list(map(int, line.strip().split()))
                trueValueList.append(trueValue)
                estimatedValueList.append(estimatedValue)
            trueValueList = np.array(trueValueList)
            estimatedValueList = np.array(estimatedValueList)

            totalNum = len(trueValueList)
            accNum = np.sum(trueValueList == estimatedValueList)
            accuracy = accNum / totalNum
            AAE = np.mean(estimatedValueList - trueValueList)
            ARE = np.mean((estimatedValueList - trueValueList) / trueValueList)
            result['taskResult'] = {
                'totalNum': totalNum,
                'accNum': accNum,
                'accuracy': accuracy,
                'AAE': AAE,
                'ARE': ARE,
            }
    elif taskName == 'topk':
        with open(osp.join(cfg.PATH.output_dir, filename)) as hd:
            trueSet, querySet = set(), set()
            for line in hd:
                trueItem, queryItem = list(map(int, line.strip().split()))
                trueSet.add(trueItem)
                querySet.add(queryItem)
            totalNum = len(trueSet)
            accNum = len(trueSet & querySet)
            accuracy = accNum / totalNum
            result['taskResult'] = {
                'totalNum': totalNum,
                'accNum': accNum,
                'accuracy': accuracy,
            }
    return result



















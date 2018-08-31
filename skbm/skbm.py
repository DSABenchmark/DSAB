from flask import (
    Blueprint, flash, g, redirect, render_template, request, url_for
)
from werkzeug.exceptions import abort

#----------------------added by Yucheng
from skbm.db import get_db
from skbm.config import cfg
import json

bp = Blueprint('skbm', __name__, url_prefix='/skbm')

@bp.route('/index.html')
def index():
    return render_template('index.html')

@bp.route('/api', methods=['GET','POST'])
def api():
    arg_get = request.args.get('get','');
    if arg_get:
        if arg_get == 'datasetList':
            db = get_db()
            ret = []
            for dct in db.dataset_info.aggregate([{'$project': {'name': 1,'_id':0}}]):
                ret.append(dct['name'])
            return json.dumps(ret)
        elif arg_get == 'sketchList':
            db = get_db()
            ret = []
            for dct in db.sketch_info.aggregate([{'$project': {'name': 1,'_id':0}}]):
                ret.append(dct['name'])
            return json.dumps(ret)
        elif arg_get == 'taskList' and request.args.get('sketch',''):
            sketchName = request.args.get('sketch','')
            db = get_db()
            dct = db.sketch_info.find_one({'name': sketchName})
            return json.dumps({'tasks': dct['tasks']})
        elif arg_get == 'args' and request.args.get('sketch','') and request.args.get('task',''):
            sketchName = request.args.get('sketch','')
            taskName = request.args.get('task','')
            db = get_db()
            dct = db.sketch_info.find_one({'name': sketchName})
            return json.dumps({'params': dct['params']})

    return 'NoNoNo...'

@bp.route('/test')
def test():
    init_existing_dataset()
    return str(get_db().dataset_info.find_one())


def single_experiment(kwargs):
    db = get_db()
    ret = db.experiment.find_one(kwargs)
    if ret:
        # get result from mondodb
        raise NotImplementedError
    else :
        # perform experiment according to kwargs
        raise NotImplementedError




# from . import auth
# app.register_blueprint(auth.bp)

# @bp.route('/')
# def index():
#     db = get_db()
#     datasets = db.dataset_info.find({})
#     sketches = db.sketch_info.find({})
#     return render_template('blog/index.html', 
#       datasets=datasets,
#       sketches=sketches,
#       )
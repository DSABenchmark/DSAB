from easydict import EasyDict as edict
from os import path as osp
from pathlib import Path

__C = edict()

__C.PATH = edict()
__C.PATH.root_dir = '/home/ubuntu/pku-sketch-benchmark'
__C.PATH.pkg_dir = osp.join(__C.PATH.root_dir, 'skbm')
__C.PATH.dataset_dir = osp.join(__C.PATH.root_dir, 'dataset')

__C.PATH.sketch_dir = osp.join(__C.PATH.pkg_dir, 'new_sketch')
__C.PATH.execute_file =  osp.join(__C.PATH.sketch_dir, 'task', 'AAA.out')
__C.PATH.output_dir = osp.join(__C.PATH.root_dir, 'experiment', 'output')
__C.PATH.graph_dir = osp.join(__C.PATH.root_dir, 'experiment', 'graph')
Path(__C.PATH.graph_dir).mkdir(exist_ok=True, parents=True)

__C.TEMP = edict()
__C.TEMP.tasks = [
	                {
	                    'name': 'freq',
	                    'params': [],
	                },
	                {
	                    'name': 'topk',
	                    'params': [{
	                        'field': 'k',
	                        'type': 'int',
	                        'help': 'xxx',
	                    }],
	                },
	                {
	                    'name': 'speed',
	                    'params': [],
	                },
	            ]

cfg = __C
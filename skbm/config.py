from easydict import EasyDict as edict
from os import path as osp

__C = edict()

__C.PATH = edict()
__C.PATH.root_dir = '/home/ubuntu/pku-sketch-benchmark'
__C.PATH.pkg_dir = osp.join(__C.PATH.root_dir, 'skbm')
__C.PATH.dataset_dir = osp.join(__C.PATH.root_dir, 'dataset')

__C.PATH.sketch_dir = osp.join(__C.PATH.pkg_dir, 'sketch')

cfg = __C
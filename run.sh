#!/bin/sh

mkdir dataset
export ROOT_DIR=$(pwd)
export FLASK_APP=skbm
export FLASK_ENV=development
.env/bin/flask run --host=0.0.0.0 --port=8086

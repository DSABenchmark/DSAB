# pku-sketch-benchmark
a benchmark used to test the performance of various sketch algorithms for multiple datasets in a simple way

## Requirements

- Both python2 and python3 are required.
- For python2, matplotlib and future packages are needed:
	- `pip2 install matplotlib==2 future`
- Latex
- MongoDB

## How to build

First, you need to clone this repo, and install the python3 dependencies.
```bash
git clone https://github.com/Yuol96/pku-sketch-benchmark
cd pku-sketch-benchmark
pip3 install -r requirements.txt
```
Then, download the default datasets and put it under `pku-sketch-benchmark/dataset`

## How to start

```bash
source env.sh
flask init-db
sh run.sh
```
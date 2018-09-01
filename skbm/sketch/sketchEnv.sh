export sketch_dir="/home/ubuntu/pku-sketch-benchmark/skbm/sketch"

# rewrite sketchList.h file
echo "#ifndef SKETCHLIST_H" > $sketch_dir/sketchList.h
echo "#define SKETCHLIST_H" >> $sketch_dir/sketchList.h
for file in `ls $sketch_dir/algorithm`
do
	echo -e '#include "algorithm/'$file'"' >> $sketch_dir/sketchList.h
done
echo "#endif" >> $sketch_dir/sketchList.h
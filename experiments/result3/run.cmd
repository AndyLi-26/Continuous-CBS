mkdir sparse
mkdir dense
mkdir super
for f in sparse-*.csv; do mv -i "$f" "sparse\${f%%.*}.csv"
for f in super-*.csv; do mv -i "$f" "super\${f%%.*}.csv"
for f in dense-*.csv; do mv -i "$f" "dense\${f%%.*}.csv"


copy sep.py 0.5\sep.py
cd 0.5
python sep.py
del sep.py
cd ..

copy sep.py 1.5\sep.py
cd 1.5
python sep.py
del sep.py
cd ..

copy sep.py 2.5\sep.py
cd 2.5
python sep.py
del sep.py
cd ..

copy sep.py 3.5\sep.py
cd 3.5
python sep.py
del sep.py
cd ..

copy sep.py 4.5\sep.py
cd 4.5
python sep.py
del sep.py
cd ..

pause
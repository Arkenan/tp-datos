
*Dependencias*

```bash
$ apt-get install libboost-dev libboost-iostreams1.54.0
```

*Compilación*

```bash
# Full compile
$ make
# Debug only
$ make debug
# Release only
$ make release
```

*Ejecución*

```bash
$ ./release/bin/huemul ../data/train.csv.gz ../data/test.csv.gz
```

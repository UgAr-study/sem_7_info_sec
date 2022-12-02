#!/bin/bash
#echo $Loadings

rm -f time_encrypt.txt
rm -f time_decrypt.txt

#echo $'p n lengthKey' > length_key.txt
#echo $'p n timeEncrypt' > time_encrypt.txt
#echo $'p n timeDencrypt' > time_dencrypt.txt

for p in 64 128 256 512 1024; do
  for n in 1 2 3 4 5; do
    ../../release/mdpc/mdpc_key_gen $n $p 5 7 $PWD/outK.txt
    cat outK.txt >> length_key.txt
    ../../release/mdpc/mdpc_encrypt 4 5 6 7 8 $PWD/outE.txt
    cat outE.txt >> time_encrypt.txt
    ../../release/mdpc/mdpc_decrypt 9 2 6 7 9 $PWD/outD.txt
    cat outD.txt >> time_decrypt.txt
    done
done
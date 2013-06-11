#! /bin/sh
while read key
do
  echo -n $key
  echo -n '	'
  read text
  echo $text
done
